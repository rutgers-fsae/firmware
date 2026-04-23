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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "FirmwareProgrammer/include/vectornav/FirmwareUpdater.hpp"
#include "vectornav/HAL/File.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/BridgeSensor.hpp"
#include "vectornav/TemplateLibrary/String.hpp"
#include "vectornav/TemplateLibrary/Vector.hpp"

#include "PyErrors.hpp"
#include "PyTemplates.hpp"

namespace py = pybind11;

namespace VN {

void init_firmware_programmer(py::module& m) {
  py::module Plugins = m.def_submodule("Plugins", "Plugins Module");
  py::module FirmwareProgrammer =
      Plugins.def_submodule("FirmwareProgrammer", "FirmwareProgrammer Module");

  py::class_<FirmwareProgrammer::FirmwareUpdater> firmwareUpdater(
      FirmwareProgrammer, "FirmwareUpdater");

  firmwareUpdater.def(py::init<>())
      .def("updateFirmware",
        [] (FirmwareProgrammer::FirmwareUpdater& fu, BridgeSensor* sens, Filesystem::FilePath filePath, FirmwareProgrammer::FirmwareUpdater::Params params) {
                                ErrorAll errors = fu.updateFirmware(sens,filePath,params);
                                if (errors != Error::None) { throw std::runtime_error(errorCodeToString(errors)); }
        }
        )
      .def("updateFirmware",
        [] (FirmwareProgrammer::FirmwareUpdater& fu, BridgeSensor* sens, FirmwareProgrammer::FirmwareUpdater::FilePaths filePath, FirmwareProgrammer::FirmwareUpdater::Params params) {
                                ErrorAll errors = fu.updateFirmware(sens,filePath,params);
                                if (errors != Error::None) { throw std::runtime_error(errorCodeToString(errors)); }
        }
        )
        ;

  py::class_<FirmwareProgrammer::FirmwareUpdater::Params> params(
      firmwareUpdater, "Params");

  params.def(py::init<>())
      .def(py::init<BridgeSensor::BaudRate, BridgeSensor::BaudRate>())
      .def_readwrite(
          "firmwareBaudRate",
          &FirmwareProgrammer::FirmwareUpdater::Params::firmwareBaudRate)
      .def_readwrite(
          "bootloaderBaudRate",
          &FirmwareProgrammer::FirmwareUpdater::Params::bootloaderBaudRate);

  py::enum_<FirmwareProgrammer::FirmwareUpdater::Processor>(
      firmwareUpdater, "Processor", py::module_local())
      .value("Imu", FirmwareProgrammer::FirmwareUpdater::Processor::Imu)
      .value("Gnss", FirmwareProgrammer::FirmwareUpdater::Processor::Gnss)
      .value("Nav", FirmwareProgrammer::FirmwareUpdater::Processor::Nav)
      .value("Poll", FirmwareProgrammer::FirmwareUpdater::Processor::Poll);

  py::class_<FirmwareProgrammer::FirmwareUpdater::File>(firmwareUpdater, "File")
      .def(py::init<>())
      .def(py::init<Filesystem::FilePath&,
                    FirmwareProgrammer::FirmwareUpdater::Processor>())
      .def_readwrite("filePath",
                     &FirmwareProgrammer::FirmwareUpdater::File::filePath)
      .def_readwrite("processor",
                     &FirmwareProgrammer::FirmwareUpdater::File::processor);

  declare_vector<FirmwareProgrammer::FirmwareUpdater::File, 6>(m, "FilePaths");

  declare_string<Filesystem::FilePath>(m, "Path");
  py::implicitly_convertible<py::str, Filesystem::FilePath>();
}
}  // namespace VN
