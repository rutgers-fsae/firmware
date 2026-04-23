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

#ifndef VN_CLI_FIRMWAREUPDATE_HPP_
#define VN_CLI_FIRMWAREUPDATE_HPP_

#include "vectornav/CLI_Sensor.hpp"

#include <msclr/marshal_cppstd.h>

#pragma managed(push, off)
#include "vectornav/FirmwareUpdater.hpp"
#include "vectornav/HAL/File.hpp"
#pragma managed(pop)

using namespace System;
namespace VNSDK
{
public ref class FirmwareProgrammer
{
    public:

    ref class FirmwareUpdater
    {
      private:
        VN::FirmwareProgrammer::FirmwareUpdater &_firmwareUpdater = VN::FirmwareProgrammer::FirmwareUpdater();

      public:
        enum class Processor {
            Nav = VN::SetBootLoader::Processor::Nav,
            Gnss = VN::SetBootLoader::Processor::Gnss,
            Imu = VN::SetBootLoader::Processor::Imu
        };

        ref class FirmwareFile {
          private:
            Processor _processorId;
            String^ _filePath;

          public:

            FirmwareFile() {
                _processorId = Processor::Nav;
                _filePath = "";
            }

            FirmwareFile(Processor ProcessorId, String^ FilePath) {
                _processorId = ProcessorId;
                _filePath = FilePath;
            }


            VN::FirmwareProgrammer::FirmwareUpdater::File ConvertToVnFile() {
                VN::FirmwareProgrammer::FirmwareUpdater::File file;
                file.processor = static_cast<VN::FirmwareProgrammer::FirmwareUpdater::Processor>(_processorId);
                msclr::interop::marshal_context context;
                file.filePath = context.marshal_as<std::string>(_filePath);

                return file;
            }
        };

        value class Params
        {
          private:
            UInt32 FirmwareBaudRate_;
            UInt32 BootloaderBaudRate_;

          public:
            Params(UInt32 FirmwareBaudRate, UInt32 BootloaderBaudRate) {
                FirmwareBaudRate_ = FirmwareBaudRate;
                BootloaderBaudRate_ = BootloaderBaudRate;
            }

            VN::FirmwareProgrammer::FirmwareUpdater::Params ConvertToVnParams() {
                VN::FirmwareProgrammer::FirmwareUpdater::Params params;
                params.bootloaderBaudRate = static_cast<VN::Sensor::BaudRate>(BootloaderBaudRate_);
                params.firmwareBaudRate = static_cast<VN::Sensor::BaudRate>(FirmwareBaudRate_);

                return params;
            }
        };


        FirmwareUpdater() {}

        void UpdateFirmware(Sensor ^ % sensor, String^ vnxmlPath, Params params)
        {
            std::string unmanagedPath = msclr::interop::marshal_as<std::string>(vnxmlPath);
            VN::ErrorAll error = _firmwareUpdater.updateFirmware(&sensor->GetNativeInstance(), VN::Filesystem::FilePath(unmanagedPath), params.ConvertToVnParams());
            if (error != VN::Error::None) { throw gcnew VnException(error); }
        }

        void UpdateFirmware(Sensor ^ % sensor, String^ vnxPath, Processor processorId, Params params) {
            std::string unmanagedPath = msclr::interop::marshal_as<std::string>(vnxPath);
            VN::InputFile vnxFile{VN::Filesystem::FilePath(unmanagedPath)};
            VN::ErrorAll error = _firmwareUpdater.updateFirmware(&sensor->GetNativeInstance(), vnxFile, static_cast<VN::FirmwareProgrammer::FirmwareUpdater::Processor>(processorId), params.ConvertToVnParams());
            if (error != VN::Error::None) { throw gcnew VnException(error); }
        }

        void UpdateFirmware(Sensor ^ % sensor, List<FirmwareFile^>^ files, Params params) {
            VN::FirmwareProgrammer::FirmwareUpdater::FilePaths vnFiles{};
            for (int i{0}; i != files->Count; ++i) {
                vnFiles.push_back(files[i]->ConvertToVnFile());
            }
            VN::ErrorAll error = _firmwareUpdater.updateFirmware(&sensor->GetNativeInstance(), vnFiles, params.ConvertToVnParams());
            if (error != VN::Error::None) { throw gcnew VnException(error); }
        }
    };
}; // public ref class FirmwareProgrammer
} // namespace VNSDK

#endif  // VN_CLI_FIRMWAREUPDATE_HPP_
