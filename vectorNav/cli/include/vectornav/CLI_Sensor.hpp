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

#ifndef VN_CLI_SENSOR_HPP_
#define VN_CLI_SENSOR_HPP_

#include "vectornav/CLI_ByteBuffer.hpp"
#include "vectornav/CLI_Commands.hpp"
#include "vectornav/CLI_CompositeData.hpp"
#include "vectornav/CLI_Exceptions.hpp"
#include "vectornav/CLI_GenericCommand.hpp"
#include "vectornav/CLI_Registers.hpp"
#include "vectornav/CLI_SteadyClock.hpp"
#include "vectornav/CLI_QueueDefinitions.hpp"

#pragma managed(push, off)
#include "vectornav/Interface/BridgeSensor.hpp"
#include "vectornav/Interface/Sensor.hpp"
#pragma managed(pop)

using namespace System;

namespace VNSDK
{

#if (PLUGIN_DATAEXPORT)
namespace DataExport
{
    ref class Exporter;
}
#endif
#if (PLUGIN_CALIBRATION)
namespace Calibration
{
    ref class HsiCalibration_Base;
}  // namespace Calibration
#endif

public
value struct AsyncError
{
  public:

    UInt16 Error;
    String^ Message;
    System::DateTime Timestamp;
};


public enum class MeasQueueMode
{
    Off = static_cast<int>(VN::BridgeSensor::MeasQueueMode::Off),
    Force = static_cast<int>(VN::BridgeSensor::MeasQueueMode::Force),
    Try = static_cast<int>(VN::BridgeSensor::MeasQueueMode::Try),
#if THREADING_ENABLE
    Retry = static_cast<int>(VN::BridgeSensor::MeasQueueMode::Retry)
#endif
};  

inline VN::BridgeSensor::MeasQueueMode ToNativeInstance(MeasQueueMode mode)
{
    return static_cast<VN::BridgeSensor::MeasQueueMode>(static_cast<int>(mode));
}

inline MeasQueueMode ToManagedInstance(VN::BridgeSensor::MeasQueueMode mode)
{
    return static_cast<MeasQueueMode>(static_cast<int>(mode));
}

public ref class BinaryOutputMeasurements
{
    private:
        VN::BridgeSensor::BinaryOutputMeasurements* _output_measurements;

    public:
        BinaryOutputMeasurements()
        {
            _output_measurements = new VN::BridgeSensor::BinaryOutputMeasurements{};
        }

        ~BinaryOutputMeasurements()
        {
            this->!BinaryOutputMeasurements();
        }

        !BinaryOutputMeasurements()
        {
            delete _output_measurements;
            _output_measurements = nullptr;
        }

        VN::BridgeSensor::BinaryOutputMeasurements& ToNativePointer()
        {
            return *_output_measurements;
        }
};

public enum class FaSubscriberFilterType
{
    ExactMatch = static_cast<int>(VN::BridgeSensor::FaSubscriberFilterType::ExactMatch),
    AnyMatch = static_cast<int>(VN::BridgeSensor::FaSubscriberFilterType::AnyMatch),
    NotExactMatch = static_cast<int>(VN::BridgeSensor::FaSubscriberFilterType::NotExactMatch),
};

inline VN::BridgeSensor::FaSubscriberFilterType ToNativeInstance(FaSubscriberFilterType filterType)
{
    return static_cast<VN::BridgeSensor::FaSubscriberFilterType>(static_cast<int>(filterType));
}

inline FaSubscriberFilterType ToManagedInstance(VN::BridgeSensor::FaSubscriberFilterType filterType)
{
    return static_cast<FaSubscriberFilterType>(static_cast<int>(filterType));
}


public enum class AsciiSubscriberFilterType
{
    StartsWith = static_cast<int>(VN::BridgeSensor::AsciiSubscriberFilterType::StartsWith),
    DoesNotStartWith = static_cast<int>(VN::BridgeSensor::AsciiSubscriberFilterType::DoesNotStartWith)
};

inline VN::BridgeSensor::AsciiSubscriberFilterType ToNativeInstance(AsciiSubscriberFilterType filterType)
{
    return static_cast<VN::BridgeSensor::AsciiSubscriberFilterType>(static_cast<int>(filterType));
}

inline AsciiSubscriberFilterType ToManagedInstance(VN::BridgeSensor::AsciiSubscriberFilterType filterType)
{
    return static_cast<AsciiSubscriberFilterType>(static_cast<int>(filterType));
}

public enum class SyncByte
{
    Ascii = static_cast<int>(VN::BridgeSensor::SyncByte::Ascii),
    FA = static_cast<int>(VN::BridgeSensor::SyncByte::FA),
    FB = static_cast<int>(VN::BridgeSensor::SyncByte::FB),
    None = static_cast<int>(VN::BridgeSensor::SyncByte::None),
};

inline VN::BridgeSensor::SyncByte ToNativeInstance(SyncByte filterType)
{
    return static_cast<VN::BridgeSensor::SyncByte>(static_cast<int>(filterType));
}

inline SyncByte ToManagedInstance(VN::BridgeSensor::SyncByte filterType)
{
    return static_cast<SyncByte>(static_cast<int>(filterType));
}

public
ref class Sensor
{
private:
    VN::BridgeSensor * _sensor;
    SteadyClock _clock{};


public:
    using BaudRate = Registers::System::BaudRate::BaudRates;
    Sensor()
    {
        _sensor = new VN::BridgeSensor();
    }

    Sensor(MeasQueueMode mode)
    {
        VN::BridgeSensor::MeasQueueMode native_mode = ToNativeInstance(mode);
        _sensor = new VN::BridgeSensor(native_mode);
    }
    
    Sensor(ByteBuffer ^ mainBuffer, ByteBuffer ^ fbBuffer, MeasQueueMode mode)
    {
        VN::Sensor::MeasQueueMode native_mode = ToNativeInstance(mode);
        _sensor = new VN::BridgeSensor(*(mainBuffer->GetNativePointer()), *(fbBuffer->GetNativePointer()), native_mode);
    }

    ~Sensor()
    {
        this->!Sensor();
    }

    !Sensor()
    {
        delete _sensor;
    }


    VN::BridgeSensor &GetNativeInstance()
    {
        return *_sensor;
    };

    // ------------------------------------------
    /*! \name Serial Connectivity */ //@{
    // ------------------------------------------

    /// @brief Opens the serial port at the specified baud rate. This does not verify any connectivity, and as such only validates that the serial port is
    /// available available for opening.
    void Connect(String ^ portName, BaudRate baudRate);

    /// @brief Opens the serial port at the specified baud rate. This does not verify any connectivity, and as such only validates that the serial port is
    /// available available for opening.
    void Connect(String ^ portName, UInt32 baudRate);

    /// @brief Opens the serial port at the specified baud rate. This does not verify any connectivity, and as such only validates that the serial port is
    /// available available for opening. Extra flag is passed in to monitor for asynchronous errors
    void Connect(String ^ portName, UInt32 baudRate, bool monitorAsyncErrors);

    /// @brief Opens the serial port at the specified baud rate. This does not verify any connectivity, and as such only validates that the serial port is
    /// available available for opening. Extra flag is passed in to monitor for asynchronous errors
    void Connect(String ^ portName, BaudRate baudRate, bool monitorAsyncErrors);

    /// @brief Opens the file specified. If THREADING_ENABLE, this starts the Listening Thread.
    void Connect(String^ fileName);

    /// @brief Opens the serial port, scanning all possible baud rates until the sensor is verified connected. This performs a verifySensorConnectivity at each
    /// possible baud rate.
    void AutoConnect(String ^ portName);

    /// @brief Opens the serial port, scanning all possible baud rates until the sensor is verified connected. This performs a verifySensorConnectivity at each
    /// possible baud rate. Extra flag is passed in to monitor for asynchronous errors
    void AutoConnect(String ^ portName, bool moniforAsyncErrors);


    /// @brief Sends a ReadRegister for the Model register. Returns true if a valid response is received, otherwise returns false.
    bool VerifySensorConnectivity();

    /// @brief Get the port name of the open serial port. If no port is open, will return std::nullopt.
    String ^ ConnectedPortName();

    /// @brief Get the baud rate at which the serial port is opened. If no port is open, will return std::nullopt.
    uint32_t ConnectedBaudRate();

    /// @brief Sends a Write Register for the new baud rate to the sensor and reopens to the serial port under the new baud rate. Will retry on failure.
    void ChangeBaudRate(BaudRate baudRate);

    /// @brief Sends a Write Register for the new baud rate to the sensor and reopens to the serial port under the new baud rate. Will retry on failure.
    void ChangeBaudRate(UInt32 baudRate);

    /// @brief Sends a Write Register for the new baud rate to the sensor on the specified serial port. Reopens to the serial port under the new baud rate if it wrote to the active port. Will retry on failure.
    void ChangeBaudRate(BaudRate baudRate, Registers::System::BaudRate::SerialPort serialPort);
    
    /// @brief Sends a Write Register for the new baud rate to the sensor on the specified serial port. Reopens to the serial port under the new baud rate if it wrote to the active port. Will retry on failure.
    void ChangeBaudRate(UInt32 baudRate, System::Byte serialPort);

    /// @brief Changes the host (computer) baud rate without commanding a change to the VectorNav unit's baud rate. Use with caution.
    void ChangeHostBaudRate(BaudRate baudRate);
    
    /// @brief Changes the host (computer) baud rate without commanding a change to the VectorNav unit's baud rate. Use with caution.
    void ChangeHostBaudRate(UInt32 baudRate);

    /// @brief Disconnect from the sensor.
    void Disconnect();

    // ------------------------------------------
    /*! \name Accessing Measurements */ //@{
    // ------------------------------------------

    /// @brief Checks to see if there is a new measurement avialable on the MeasurementQueue.
    bool HasMeasurement();

    /// @brief Gets (and pops) the front of the MeasurementQueue.
    /// @param block If true, wait a maximum of getMeasurementTimeoutLength for a new measurement.
    Nullable<CompositeData> GetNextMeasurement();

    /// @brief Gets the back (most recent) of the MeasurementQueue, popping every measurement in the queue.
    /// @param block If true, wait a maximum of getMeasurementTimeoutLength for a new measurement.
    Nullable<CompositeData> GetMostRecentMeasurement();

    /// @brief Block mode for sending a command. @see sendCommand()
    enum class SendCommandBlockMode
    {
        None = (int)VN::BridgeSensor::SendCommandBlockMode::None,            ///< Do not wait for a response from the unit before returning.
        Block = (int)VN::BridgeSensor::SendCommandBlockMode::Block,           ///< Block upon a response from the unit until the specific timeout; will return ResponseTimeout if timeout is hit.
        BlockWithRetry = (int)VN::BridgeSensor::SendCommandBlockMode::BlockWithRetry,  ///< Block upon a response from the unit until the specific timeout; retry sending the command and blocking commandSendRetriesAllowed
    };

    // ------------------------------------------
    /*! \name Sending Commands */ //@{
    // ------------------------------------------

    /// @brief Sends a Read Register command to the unit to poll register values. This is always a blocking call.
    void ReadRegister(Registers::ConfigRegister ^ reg);
    void ReadRegister(Registers::MeasRegister ^ reg);

    /// @brief Sends a Write Register command to the unit to set register values. This is always a blocking call.
    void WriteRegister(Registers::ConfigRegister ^ reg);

    /// @brief Sends the Write Settings command to the unit and blocks on the unit's confirmation.
    void WriteSettings();

    /// @brief Sends a Reset command to the unit and blocks on the unit's confirmation. After confirmation, it sleeps for resetSleepDuration and verifies sensor
    /// connectivity, calling autoConnect if failed.
    void Reset();

    /// @brief Sends a Restore Factory Settings command to the unit, blocks on the unit's confirmation, reopens serial at the unit's default baud rate, sleeps
    /// for resetSleepDuration, then verifies unit connectivity. If THREADING_ENABLE, resets the Listening Thread.
    void RestoreFactorySettings();

    /// @brief Sends a Known Magnetic Disturbance command to the sensor and blocks on the unit's message confirmation.
    void KnownMagneticDisturbance(KnownMagneticDisturbance::State state);

    /// @brief Sends a Known Magnetic Disturbance command to the sensor and blocks on the unit's message confirmation.
    void KnownMagneticDisturbance(uint8_t state);

    /// @brief Sends a Known Acceleration Disturbance command to the sensor and blocks on the unit's message confirmation.
    void KnownAccelerationDisturbance(KnownAccelerationDisturbance::State state);

    /// @brief Sends a Known Acceleration Disturbance command to the sensor and blocks on the unit's message confirmation.
    void KnownAccelerationDisturbance(uint8_t state);

    /// @brief Sends a Set Initial Heading command to the sensor and blocks on the unit's message confirmation.
    void SetInitialHeading(float heading);
    void SetInitialHeading(Ypr ypr);
    void SetInitialHeading(Quaternion quat);

    /// @brief Sends an Async Output Enable command to the sensor and blocks on the unit's message confirmation.
    void Sensor::AsyncOutputEnable(AsyncOutputEnable::State state);

    /// @brief Sends an Async Output Enable command to the sensor and blocks on the unit's message confirmation.
    void AsyncOutputEnable(uint8_t state);

    /// @brief Sends a Set Filter Bias command to the sensor and blocks on the unit's message confirmation.
    void SetFilterBias();

    /// @brief Sends a Set Bootloader command to the sensor and blocks on the unit's message confirmation.
    // SetBootloader(const SetBootLoader::Processor processorId);  // Unnecessary for CLI.

    /// @brief Sends an arbitrary command to the unit. The direct command API should be preferred unless it necessary to send without blocking.
   
    void SendCommand(GenericCommand^% commandToSend, Sensor::SendCommandBlockMode waitMode);
    // Error Sensor::sendCommand(Command* commandToSend, SendCommandBlockMode waitMode, const Microseconds waitLengthMs, const Microseconds timeoutThreshold) noexcept

    /// @brief Sends an arbitary message to the unit without any message modification or response validation. Not recommended to use.
    void SerialSend(String ^ msgToSend);

    // ------------------------------------------
    // Error Handling
    // ------------------------------------------

    /// @brief Throws any Asynchronous Errors (as a UInt16) reported by the sensor
    void ThrowIfAsyncError();

    /// @brief Get the current timestamp. Used mostly for debugging purposes.
    System::DateTime Now();

    // -----------------------------------------
    // Additional logging
    // ------------------------------------------
    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    void SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter);

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    void SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter, FaSubscriberFilterType filterType);

    /// @brief Unsubscribes particular FA packet queue.
    void UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter);

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    void SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, System::String^ asciiHeaderFilter);

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    void SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, System::String^ asciiHeaderFilter, AsciiSubscriberFilterType filterType);

    /// @brief Unsubscribes particular ASCII packet queue.
    void UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, System::String^ asciiHeaderFilter);

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    void SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, SyncByte syncByte);

    /// @brief Unsubscribes queue based on type.
    void UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, SyncByte syncByte);

    /// @brief Registers a ByteBuffer to capture all bytes received.  Only one may be registered at a time.
    void RegisterReceivedByteBuffer(ByteBuffer^ buffer);

    /// @brief Unregisters the ReceivedByteBuffer.
    void DeregisterReceivedByteBuffer();

#if (PLUGIN_DATAEXPORT)
    /// @brief Registers a Data Exporter, subscribing it to all ASCII and Binary messages.
    void RegisterDataExporter(DataExport::Exporter^ exporter);
    /// @brief Deregisters the Data Exporter.
    void DeregisterDataExporter(DataExport::Exporter ^ exporter);
#endif
    // ------------------------------------------
    // Unthreaded Packet Processing
    // ------------------------------------------

    /// Not implemented.

}; // public ref class Sensor
} // namespace VNSDK

#include "vectornav/CLI_Plugins.hpp"

#endif  // VN_CLI_SENSOR_HPP_
