// VectorNav SDK (v1.0.0)
// Copyright (c) 2024 VectorNav Technologies, LLC
// 
// WARNING â€“ This software contains Controlled Technical Data, export of which
// is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
// Disclosure to foreign persons contrary to the EAR is prohibited. Violations
// of these export laws and regulations are subject to severe civil and criminal
// penalties.
// 
// The MIT License (MIT)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "vectornav/FirmwareUpdater.hpp"

#include "vectornav/Bootloader.hpp"
#include "vectornav/Debug.hpp"
#include "vectornav/HAL/Thread.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Interface/GenericCommand.hpp"
#include "vectornav/Interface/Sensor.hpp"

namespace VN
{
namespace FirmwareProgrammer
{

ErrorAll FirmwareUpdater::updateFirmware(Sensor* sensor, Filesystem::FilePath vnXmlPath, Params params)
{
    if (!Filesystem::exists(vnXmlPath))
    {
        VN_DEBUG_0("File does not exist: " << vnXmlPath << std::endl);
        return ErrorAll{Error::FileDoesNotExist};
    }

    InputFile vnXmlFile{true};
    if (vnXmlFile.open(vnXmlPath))
    {
        VN_DEBUG_0("Failed to open file: " << vnXmlPath << std::endl);
        return ErrorAll{Error::FileOpenFailed};
    }

    ErrorAll errors = updateFirmware(sensor, vnXmlFile, params);
    if (errors == Error::None) { VN_DEBUG_0("Firmware update complete." << std::endl); }

    return errors;
}

ErrorAll FirmwareUpdater::updateFirmware(Sensor* sensor, InputFile& vnXmlFile, Params params)
{
    if (!vnXmlFile.is_open())
    {
        VN_DEBUG_0("Firmware file is not open. Open before calling updateFirmware." << std::endl);
        return ErrorAll{Error::FileReadFailed};
    }
    VnXml::Metadata vnXmlMetadata = VnXml::extractMetadata(vnXmlFile);

    _sensor = sensor;
    _navBaudRate = params.firmwareBaudRate;
    _bootloaderBaudRate = params.bootloaderBaudRate;
    // Reset the input stream to the beginning of the file
    vnXmlFile.reset();

    ErrorAll errors = _tryUpdateFirmwareFromBootloader(vnXmlFile, vnXmlMetadata, params.firmwareBaudRate);
    if (errors == Error::None) { VN_DEBUG_0("Successfully recovered firmware.\n"); }
    else if (errors != ErrorBL::EnterBootloaderFailed)
    {
        VN_DEBUG_0("Failed to recover firmware." << std::endl);
        return errors;
    }

    // Todo: Add handling of skipping firmware update when in recovery mode
    Error latestError = _pollSensorModelAndFirmwareVersion();
    if (latestError != Error::None) { return ErrorAll{latestError}; }
    VN_DEBUG_0("Sensor model : " << _model << std::endl);

    latestError = _sensor->restoreFactorySettings();  // Has to happen before current processor polling because some firmware has a bug preventing a valid
                                                      // response outside of 115200
    if (latestError != Error::None) { VN_DEBUG_0(latestError << " encountered when attempting to restore factory settings." << std::endl); }
    _navBaudRate = Sensor::BaudRate::Baud115200;
    if (_getCurrentProcessor() != Error::None)
    {
        VN_DEBUG_0("Failed to get current processor, assuming Nav." << std::endl);
        _currentProcessor = Processor::Nav;
    }

    // Skip to the beginning of the processor's firmware data
    vnXmlFile.reset();
    AsciiMessage discard;
    size_t prevLineNum = 1;
    for (const auto& component : vnXmlMetadata)
    {
        // Skip to the beginning of the processor's firmware data
        const size_t numLinesToSkip = component.dataLineBegin - prevLineNum;
        for (size_t lineNo = 0; lineNo < numLinesToSkip; ++lineNo) { vnXmlFile.getLine(discard.begin(), discard.capacity()); }

        Sensor::BaudRate currBaudRate, currbootloaderBaudRate;
        if (VnXml::NavProcessorCheck::is_value(component.hardwareId))
        {
            latestError = _switchToNavProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            currBaudRate = _navBaudRate;
            currbootloaderBaudRate = _bootloaderBaudRate;
        }
        else if (VnXml::GnssProcessorCheck::is_value(component.hardwareId))
        {
            latestError = _switchToGnssProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            currBaudRate = _gnssBaudRate;
            currbootloaderBaudRate =
                static_cast<Sensor::BaudRate>(std::min(static_cast<uint32_t>(_bootloaderBaudRate), static_cast<uint32_t>(Sensor::BaudRate::Baud460800)));
            // SBL is not reliable over 460800
        }
        else if (VnXml::ImuProcessorCheck::is_value(component.hardwareId))
        {
            latestError = _switchToImuProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            currBaudRate = _imuBaudRate;
            currbootloaderBaudRate =
                static_cast<Sensor::BaudRate>(std::min(static_cast<uint32_t>(_bootloaderBaudRate), static_cast<uint32_t>(Sensor::BaudRate::Baud460800)));
            // SBL is not reliable over 460800
        }
        else { return ErrorBL::IncompatibleVNX; }

        // Send records
        ErrorAll errors = _updateProcessor(_sensor, vnXmlFile, currBaudRate, currbootloaderBaudRate, component.dataLineBegin,
                                           component.dataLineEnd - component.dataLineBegin);
        if (errors != Error::None) { return errors; }

        prevLineNum = component.dataLineEnd;  // Assume the bootloader consumed the whole file to set up for the next loop
    }

    latestError = _switchToNavProcessor();  // Finish by exiting to nav processor
    if (latestError != Error::None) { return ErrorAll{latestError}; }

    latestError = _sensor->reset();
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when resetting the sensor." << std::endl);
        return ErrorAll{latestError};
    }
    VN_DEBUG_0("Firmware update complete." << std::endl);
    return ErrorAll{Error::None};
}

ErrorAll FirmwareUpdater::updateFirmware(Sensor* sensor, FilePaths files, Params params)
{
    for (const auto& currentFile : files)
    {
        if (!Filesystem::exists(currentFile.filePath))
        {
            VN_DEBUG_0("File does not exist: " << currentFile.filePath << std::endl);
            return ErrorAll{Error::FileDoesNotExist};
        }
        if (_tryOpenFile(currentFile.filePath)) { return ErrorAll{Error::FileOpenFailed}; }
        ErrorAll errors = updateFirmware(sensor, _fileH, currentFile.processor, params);
        _fileH.close();
        if (errors != Error::None) { return errors; }
    }

    VN_DEBUG_0("Firmware update complete." << std::endl);
    return ErrorAll{Error::None};
}

ErrorAll FirmwareUpdater::updateFirmware(Sensor* sensor, InputFile& vnxFile, const Processor processor, Params params)
{
    if (!vnxFile.is_open())
    {
        VN_DEBUG_0("Firmware file is not open. Update before calling updateFirmware" << std::endl);
        return ErrorAll{Error::FileReadFailed};
    }
    _sensor = sensor;
    _navBaudRate = params.firmwareBaudRate;
    _bootloaderBaudRate = params.bootloaderBaudRate;
    _totalLinesInFile = _calculateNumberOfLinesInFile(vnxFile);
    // Assume that we have a sensor in an invalid state due to a previously failed bootload attempt
    ErrorAll errors = _tryUpdateFirmwareFromBootloader(vnxFile, params.firmwareBaudRate);
    if (errors != ErrorBL::EnterBootloaderFailed) { return errors; }  // Returns on both success and failure

    Error latestError = _pollSensorModelAndFirmwareVersion();
    if (latestError != Error::None) { return ErrorAll{latestError}; }
    VN_DEBUG_0("Sensor model : " << _model << std::endl);

    if (!_isCompatible(processor)) { return ErrorAll{ErrorBL::IncompatibleVNX}; }

    latestError = _sensor->restoreFactorySettings();  // Has to happen before current processor polling because some firmware has a bug preventing a valid
                                                      // response outside of 115200
    _navBaudRate = Sensor::BaudRate::Baud115200;
    if (latestError != Error::None) { VN_DEBUG_0(latestError << " encountered when attempting to restore factory settings." << std::endl); }
    if (_getCurrentProcessor() != Error::None)
    {
        VN_DEBUG_0("Failed to get current processor, assuming Nav." << std::endl);
        _currentProcessor = Processor::Nav;
    }

    Sensor::BaudRate firmwareBaudRateToUse;
    Sensor::BaudRate bootloaderBaudRateToUse;
    switch (processor)
    {
        case (Processor::Nav):
        {
            latestError = _switchToNavProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            firmwareBaudRateToUse = _navBaudRate;
            bootloaderBaudRateToUse = _bootloaderBaudRate;
            break;
        }
        case (Processor::Imu):
        {
            latestError = _switchToImuProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            firmwareBaudRateToUse = _imuBaudRate;
            bootloaderBaudRateToUse =
                static_cast<Sensor::BaudRate>(std::min(static_cast<uint32_t>(_bootloaderBaudRate), static_cast<uint32_t>(Sensor::BaudRate::Baud460800)));
            // SBL is not reliable over 460800
            break;
        }
        case (Processor::Gnss):
        {
            latestError = _switchToGnssProcessor();
            if (latestError != Error::None) { return ErrorAll{latestError}; }
            firmwareBaudRateToUse = _gnssBaudRate;
            bootloaderBaudRateToUse =
                static_cast<Sensor::BaudRate>(std::min(static_cast<uint32_t>(_bootloaderBaudRate), static_cast<uint32_t>(Sensor::BaudRate::Baud460800)));
            // SBL is not reliable over 460800
            break;
        }
        default:
        {
            return ErrorAll{ErrorBL::IncompatibleVNX};
        }
    }

    errors = _updateProcessor(sensor, vnxFile, firmwareBaudRateToUse, bootloaderBaudRateToUse, 0, _totalLinesInFile);
    if (errors != Error::None) { return errors; }

    latestError = _switchToNavProcessor();  // Switch back to nav processor after the update in complete
    if (latestError != Error::None) { return ErrorAll{latestError}; }

    latestError = _sensor->reset();
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when resetting the sensor." << std::endl);
        return ErrorAll{latestError};
    }
    VN_DEBUG_0("Firmware update complete." << std::endl);
    return ErrorAll{Error::None};
}

ErrorAll FirmwareUpdater::_tryUpdateFirmwareFromBootloader(InputFile& vnXmlFile, const VnXml::Metadata& vnXmlMetadata, const Sensor::BaudRate firmwareBaudRate)
{
    if (Bootloader::autoconfigureBootloader(_sensor, Sensor::BaudRate::Baud115200) == Error::None)  // If we are in recovery mode, don't try to go too fast.
    {
        // Bootloader detection succeeded.
        VN_DEBUG_0("Unit already in bootloader. Attempting to recover nav firmware from a corrupted previous attempt.\n");
        // We're in the bootloader and should just start by updating the nav firmware. Hope we're talking to nav.

        // Skip to the beginning of the processor's firmware data
        AsciiMessage discard;
        size_t prevLineNum = 1;
        for (const auto& component : vnXmlMetadata)
        {
            // Skip to the beginning of the processor's firmware data
            const size_t numLinesToSkip = component.dataLineBegin - prevLineNum;
            for (size_t lineNo = 0; lineNo < numLinesToSkip; ++lineNo) { vnXmlFile.getLine(discard.begin(), discard.capacity()); }

            if (component.memoryType != VnXml::MemoryType::Firmware) { continue; }

            ErrorAll errors = _updateFirmware(_sensor, vnXmlFile, component.dataLineBegin, component.dataLineEnd - component.dataLineBegin);
            if (errors != Error::None)
            {
                if (errors == ErrorBL::DecryptionError) { continue; }  // wrong processor
                else { return errors; }
            }

            return ErrorAll{Bootloader::exitBootloader(_sensor, firmwareBaudRate)};
        }
        // There wasn't a valid firmware file.
        _sensor->changeHostBaudRate(_navBaudRate);
        return ErrorAll{ErrorBL::IncompatibleVNX};
    }
    // If we couldn't confirm bootload, assume we didn't start in the bootloader and we can proceed as normal
    _sensor->changeHostBaudRate(_navBaudRate);
    return ErrorAll{ErrorBL::EnterBootloaderFailed};
}

ErrorAll FirmwareUpdater::_tryUpdateFirmwareFromBootloader(InputFile& vnxFile, const Sensor::BaudRate firmwareBaudRate)
{
    if (Bootloader::autoconfigureBootloader(_sensor, Sensor::BaudRate::Baud115200) == Error::None)  // If we are in recovery mode, don't try to go too fast.
    {
        // Bootloader detection succeeded.
        VN_DEBUG_0("Unit already in bootloader. Attempting to recover nav firmware from a corrupted previous attempt.\n");

        ErrorAll errors = _updateFirmware(_sensor, vnxFile, 0, _totalLinesInFile);
        if (errors != Error::None) { return errors; }

        Error err = Bootloader::exitBootloader(_sensor, firmwareBaudRate);
        if (err == Error::None) { VN_DEBUG_0("Recovered firmware version.\n"); }
        else { VN_DEBUG_0("Unable to recover firmware.\n"); }

        return ErrorAll{err};
    }
    // If we couldn't confirm bootload, assume we didn't start in the bootloader and we can proceed as normal
    return ErrorAll{ErrorBL::EnterBootloaderFailed};
}

bool FirmwareUpdater::_isCompatible(const Processor processor) const noexcept
{
    if (_model.length() < 6) { return false; }
    switch (processor)
    {
        case (Processor::Imu):
        {
            if (_model[4] == '0')
            {
                // Is industrial
                VN_DEBUG_0("Error: Industrial sensors do not have an IMU processor.\n");
                return false;
            }
            break;
        }
        case (Processor::Gnss):
        {
            if (_model[3] != '3')
            {
                // Is not a VN-3X0
                VN_DEBUG_0("Error: Only VN-3X0 have GNSS processors.\n");
                return false;
            }
            break;
        }
        case (Processor::Nav):
        {
            // All have nav, no failures here.
            break;
        }
        default:  // Unknown processor type
            return false;
    }
    return true;
}

Error FirmwareUpdater::_pollSensorModelAndFirmwareVersion()
{
    VN::Registers::System::Model modelRegister;
    Error latestError = _sensor->readRegister(&modelRegister);
    if (latestError == Error::ResponseTimeout)
    {
        // Try to recover by autoconnecting (ResponseTimeout error means serial port is open)
        latestError = _sensor->autoBaud();
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " : encountered when establishing communication." << std::endl);
            return latestError;
        }
        VN_DEBUG_0("Warning: connected at " << (*_sensor->connectedBaudRate()) << " bps." << std::endl);
        latestError = _sensor->readRegister(&modelRegister);
    }
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " : encountered when reading model register." << std::endl);
        return latestError;
    }
    _model = modelRegister.model;

    VN::Registers::System::FwVer firmwareRegister;
    latestError = _sensor->readRegister(&firmwareRegister);
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " : encountered when reading firmware register." << std::endl);
        return latestError;
    }
    _firmwareVersion = firmwareRegister.fwVer;
    return Error::None;
}

ErrorAll FirmwareUpdater::_updateProcessor(Sensor* sensor, InputFile& firmwareFile, const Sensor::BaudRate firmwareBaudRate,
                                           const Sensor::BaudRate bootloaderBaudRate, const size_t beginningLineNumber, const size_t numLinesInFirmware)
{
    VN::Registers::System::FwVer firmwareReg;
    Error latestError = sensor->readRegister(&firmwareReg);
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " : encountered when reading firmware register." << std::endl);
        return ErrorAll{latestError};
    }
    VN_DEBUG_0("Initial firmware version: " << firmwareReg.fwVer << std::endl);

    // Entering Bootloader
    if (Bootloader::tryEnterBootloader(sensor, firmwareBaudRate, bootloaderBaudRate)) { return ErrorAll{ErrorBL::EnterBootloaderFailed}; }

    // Flashing the .vnx file
    VN_DEBUG_0("Updating processor.\n");
    ErrorAll errors = _updateFirmware(sensor, firmwareFile, beginningLineNumber, numLinesInFirmware);
    if (errors != Error::None) { return errors; }

    VN_DEBUG_0("Exiting bootloader.\n");
    latestError = Bootloader::exitBootloader(sensor, firmwareBaudRate);
    if (latestError != Error::None) { return ErrorAll{latestError}; }

    VN_DEBUG_0("Processor updated.\n");

    latestError = sensor->readRegister(&firmwareReg);
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " : encountered when reading firmware register." << std::endl);
        return ErrorAll{latestError};
    }
    VN_DEBUG_0("Updated firmware version: " << firmwareReg.fwVer << "\n" << std::endl);
    return ErrorAll{Error::None};
}

// Switch processors
Error FirmwareUpdater::_getCurrentProcessor()
{
    SetBootLoader sbl{SetBootLoader::Processor::Poll};
    Error latestError = _sensor->sendCommand(&sbl, Sensor::SendCommandBlockMode::BlockWithRetry);
    if (latestError != Error::None) { return latestError; }
    if (sbl.getError().has_value()) { return sbl.getError().value(); }
    _currentProcessor = sbl.processorId;

    return Error::None;
}

Error FirmwareUpdater::_switchToNavProcessor()
{
    if (_currentProcessor == Processor::Nav) { return Error::None; }
    auto splitModel = StringUtils::split(_model, '-');
    VN_ASSERT(splitModel.size() > 1);
    Error latestError = _sensor->setBootloader(Processor::Nav);
    if (latestError == Error::InvalidCommand && splitModel[1][0] == '3')
    {  // Is VN-300
        DebugSwitches dbs{DebugSwitches::Switch::Processor, DebugSwitches::State::Main};
        latestError = _sensor->sendCommand(&dbs, Sensor::SendCommandBlockMode::Block, 6s);
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when switching to the Nav processor" << std::endl);
            return latestError;
        }
    }
    else if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when switching to the Nav processor." << std::endl);
        return latestError;
    }
    if (latestError = _sensor->changeHostBaudRate(_navBaudRate); latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when setting baud rate." << std::endl);
        return latestError;
    }  // verifies serial port is open/connected so connectedPortName and connectedBaudRate exist
    thisThread::sleepFor(1s);
    if (!_sensor->verifySensorConnectivity())
    {
        latestError = _sensor->autoBaud();
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when autoconnecting." << std::endl);
            return latestError;
        }
        VN_DEBUG_0("Warning: Nav baud rate set at " << (*_sensor->connectedBaudRate()) << std::endl);
    }

    _currentProcessor = Processor::Nav;
    VN_DEBUG_0("Connected to Nav processor.\n");
    return Error::None;
}

Error FirmwareUpdater::_switchToImuProcessor()
{
    if (_currentProcessor == Processor::Imu) { return Error::None; }
    Error latestError = _switchToNavProcessor();
    if (latestError != Error::None)
    {  // It's not always possible to switch directly between GNSS and IMU
        VN_DEBUG_0("Failed to switch to Nav processor while switching to IMU processor" << std::endl);
        return latestError;
    }
    latestError = _sensor->setBootloader(Processor::Imu);
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when switching to the Imu processor." << std::endl);
        return latestError;
    }
    if (latestError = _sensor->changeHostBaudRate(_imuBaudRate); latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when setting baud rate." << std::endl);
        return latestError;
    }  // verifies serial port is open/connected so connectedPortName and connectedBaudRate exist
    thisThread::sleepFor(1s);
    if (!_sensor->verifySensorConnectivity())
    {
        latestError = _sensor->autoBaud();
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when autoconnecting." << std::endl);
            return latestError;
        }
        VN_DEBUG_0("Warning: IMU baud rate set at " << (*_sensor->connectedBaudRate()) << std::endl);
    }

    _currentProcessor = Processor::Imu;
    VN_DEBUG_0("Connected to IMU processor.\n");
    return Error::None;
}

Error FirmwareUpdater::_switchToGnssProcessor()
{
    if (_currentProcessor == Processor::Gnss) { return Error::None; }
    Error latestError = _switchToNavProcessor();
    if (latestError != Error::None)
    {  // It's not always possible to switch directly between GNSS and IMU
        VN_DEBUG_0("Failed to switch to Nav processor while switching to GNSS processor" << std::endl);
        return latestError;
    }

    auto splitModel = StringUtils::split(_model, '-');
    VN_ASSERT(splitModel.size() > 1);

    // Check family for presence of GNSS processor
    if (!(splitModel[1][0] == '3') &&  // All 3XX products
        !(splitModel[1] == "210E"))
    {
        return Error::InvalidCommand;
    }

    latestError = _sensor->setBootloader(Processor::Gnss);
    if (latestError == Error::InvalidCommand)
    {
        // For VN-300 Legacy firmware 0.5.0.5 processor switch commands
        if (splitModel[1][1] != '0')
        {  // Tactical
            VN_DEBUG_0(latestError << " encountered when switching to the Gnss processor" << std::endl);
            return latestError;
        }

        VN_ASSERT(splitModel.size() > 2);
        if (splitModel[2][0] == 'C')
        {  // Rugged
            DebugSwitches dbs{DebugSwitches::Switch::Processor, DebugSwitches::State::GPS};
            latestError = _sensor->sendCommand(&dbs, Sensor::SendCommandBlockMode::Block);
            if (latestError != Error::None)
            {
                VN_DEBUG_0(latestError << " encountered when switching to the Gnss processor" << std::endl);
                return latestError;
            }
        }
        else
        {  // SMD
            ServicePortSwitch sps{};
            latestError = _sensor->sendCommand(&sps, Sensor::SendCommandBlockMode::Block);
            if (latestError != Error::None)
            {
                VN_DEBUG_0(latestError << " encountered when switching to the Gnss processor" << std::endl);
                return latestError;
            }
        }
        VN_DEBUG_0("WARNING: A hardware power cycle is required to connect to the Nav processor" << std::endl);
    }
    else if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when switching to the Gnss processor" << std::endl);
        return latestError;
    }
    if (latestError = _sensor->changeHostBaudRate(_gnssBaudRate); latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when setting baud rate." << std::endl);
        return latestError;
    }  // verifies serial port is open/connected so connectedPortName and connectedBaudRate exist
    thisThread::sleepFor(1s);
    if (!_sensor->verifySensorConnectivity())
    {
        latestError = _sensor->autoBaud();
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when autoconnecting." << std::endl);
            return latestError;
        }
        VN_DEBUG_0("Warning: GNSS baud rate set at " << (*_sensor->connectedBaudRate()) << std::endl);
    }

    _currentProcessor = Processor::Gnss;
    VN_DEBUG_0("Connected to GNSS processor.\n");
    return Error::None;
}

// Firmware update helpers
ErrorAll FirmwareUpdater::_updateFirmware(Sensor* sensor, InputFile& firmwareFile, const size_t lineNumberBeginning, const size_t numLinesInFirmware)
{
    Bootloader::BootloaderReturn failure;
    const uint8_t numBootloaderTries = 2;
    for (uint8_t i = 0; i < numBootloaderTries; ++i)
    {
        failure = Bootloader::sendRecords(sensor, firmwareFile, numLinesInFirmware);
        switch (failure.failureMode)
        {
            case Bootloader::BootloaderReturn::FailureMode::None:
            {
                return ErrorAll{Error::None};
            }
            case Bootloader::BootloaderReturn::FailureMode::Retry:
            {
                firmwareFile.reset();
                for (size_t i = 0; i < lineNumberBeginning; ++i)
                {
                    AsciiMessage discard;
                    firmwareFile.getLine(discard.begin(), discard.capacity());
                }
                continue;
            }
            case Bootloader::BootloaderReturn::FailureMode::Abort:
            {
                VN_DEBUG_0("Sending records failed.\n");
                return failure.error;
            }
        }
    }
    return failure.error;  // Max retires reached
}

Errored FirmwareUpdater::_tryOpenFile(const Filesystem::FilePath& filePath)
{
    if (!Filesystem::exists(filePath))
    {
        VN_DEBUG_0("File does not exist: " << filePath << std::endl);
        return true;
    }
    if (_fileH.open(filePath))
    {
        VN_DEBUG_0("Error opening the file." << std::endl);
        _fileH.close();
        return true;
    }
    return false;
}

size_t FirmwareUpdater::_calculateNumberOfLinesInFile(InputFile& file) const
{
    size_t numberOfLines = 0;
    AsciiMessage line;
    while (!file.getLine(line.begin(), line.capacity())) { ++numberOfLines; }
    file.reset();
    return numberOfLines;
}

}  // namespace FirmwareProgrammer
};  // namespace VN
