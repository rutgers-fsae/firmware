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

// clang-format off
#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "vectornav/Interface/Commands.hpp"
#include "vectornav/Interface/GenericCommand.hpp"

namespace py = pybind11;
namespace VN {

void init_commands(py::module& m) {

  py::class_<GenericCommand>(m, "GenericCommand")
      .def(py::init<const GenericCommand&>())
      .def(py::init<AsciiMessage&, uint8_t>(), py::arg("commandString") = "",
           py::arg("numCharToMatch") = 3)
      .def("isAwaitingResponse", &GenericCommand::isAwaitingResponse)
      .def("hasValidResponse", &GenericCommand::hasValidResponse)
      .def("getError", &GenericCommand::getError)
      .def("getResponse", [](GenericCommand& cls) -> std::string {
        return std::string(cls.getResponse().c_str());
      })
      .def("getCommandString", [](GenericCommand& cls) -> std::string {
        return std::string(cls.getCommandString().c_str());
      })
      ;

  py::class_<WriteSettings, GenericCommand>(m, "WriteSettings")
      .def(py::init<>());

  py::class_<RestoreFactorySettings, GenericCommand>(m,
                                                     "RestoreFactorySettings")
      .def(py::init<>());

  py::class_<Reset, GenericCommand>(m, "Reset").def(py::init<>());

  py::class_<FirmwareUpdate, GenericCommand>(m, "FirmwareUpdate")
      .def(py::init<>());

  py::class_<SetInitialHeading, GenericCommand>(m, "SetInitialHeading")
      .def(py::init<float>());

  py::class_<SetFilterBias, GenericCommand>(m, "SetFilterBias")
      .def(py::init<>());

  py::class_<KnownMagneticDisturbance, GenericCommand> knownMagneticDisturbance(
      m, "KnownMagneticDisturbance");

  py::enum_<KnownMagneticDisturbance::State>(knownMagneticDisturbance, "State")
      .value("NotPresent", KnownMagneticDisturbance::State::NotPresent)
      .value("Present", KnownMagneticDisturbance::State::Present);

  knownMagneticDisturbance.def(py::init<KnownMagneticDisturbance::State>());

  py::class_<KnownAccelerationDisturbance, GenericCommand>
      knownAccelerationDisturbance(m, "KnownAccelerationDisturbance");

  knownAccelerationDisturbance.def(
      py::init<KnownAccelerationDisturbance::State>());

  py::enum_<KnownAccelerationDisturbance::State>(knownAccelerationDisturbance,
                                                 "State")
      .value("NotPresent", KnownAccelerationDisturbance::State::NotPresent)
      .value("Present", KnownAccelerationDisturbance::State::Present);

  py::class_<AsyncOutputEnable, GenericCommand> asyncOutputEnable(
      m, "AsyncOutputEnable");

  py::enum_<AsyncOutputEnable::State>(asyncOutputEnable, "State")
      .value("Disable", AsyncOutputEnable::State::Disable)
      .value("Enable", AsyncOutputEnable::State::Enable);

  asyncOutputEnable.def(py::init<AsyncOutputEnable::State>());
}

}  // namespace VN
// clang-format on
