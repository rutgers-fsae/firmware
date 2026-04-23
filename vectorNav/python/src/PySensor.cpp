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

#include <atomic>

#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/Interface/CompositeData.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/Interface/BridgeSensor.hpp"

#include "PyErrors.hpp"

namespace py = pybind11;
namespace VN
{

void init_sensor(py::module& m)
{
    m.def("now",[]() { return VN::now(); });

    py::class_<Sensor>(m, "Sensor_Base")
        .def("verifySensorConnectivity", &Sensor::verifySensorConnectivity)
        .def("connectedPortName", &Sensor::connectedPortName)
        .def("connectedBaudRate", &Sensor::connectedBaudRate)
        .def("changeBaudRate",
        [](Sensor& vs, Sensor::BaudRate newBaudRate) {
            Error error = vs.changeBaudRate(newBaudRate);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("changeBaudRate",
            [](Sensor& vs, Sensor::BaudRate newBaudRate,  Registers::System::BaudRate::SerialPort serialPort) {
              Error error = vs.changeBaudRate(newBaudRate, serialPort);
              if (error != Error::None) { throwError(error); }
            }
        )
        .def("changeHostBaudRate",
        [](Sensor& vs, Sensor::BaudRate newBaudRate) {
            Error error = vs.changeHostBaudRate(newBaudRate);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("disconnect", [] (Sensor& vs) {
            vs.disconnect();
        } 
        )
        // Measurement Accessor
        .def("hasMeasurement", &Sensor::hasMeasurement)
        .def("getNextMeasurement",
        [](Sensor& vs) -> std::optional<VN::CompositeData> {
            auto ownPtr = vs.getNextMeasurement();
            if (ownPtr) { return std::make_optional(*ownPtr); } else { return std::nullopt; }      
        }
        )
        .def("getNextMeasurement",
        [](Sensor& vs, const bool blocking) -> std::optional<VN::CompositeData> {
            auto ownPtr = vs.getNextMeasurement(blocking);
            if (ownPtr) { return std::make_optional(*ownPtr); } else { return std::nullopt; }      
        }
        )
        .def("getMostRecentMeasurement",
        [](Sensor& vs) -> std::optional<VN::CompositeData> {
            auto ownPtr = vs.getMostRecentMeasurement();
            if (ownPtr) { return std::make_optional(*ownPtr); } else { return std::nullopt; }      
        }
        )
        .def("getMostRecentMeasurement",
        [](Sensor& vs, const bool blocking) -> std::optional<VN::CompositeData> {
            auto ownPtr = vs.getMostRecentMeasurement(blocking);
            if (ownPtr) { return std::make_optional(*ownPtr); } else { return std::nullopt; }      
        }
        )
        // Command Sending
        .def("readRegister",
        [](Sensor& vs, Register* registerToRead, const bool retryOnFailure) {
            Error error = vs.readRegister(registerToRead, retryOnFailure);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("readRegister",
        [](Sensor& vs, Register* registerToRead) {
            Error error = vs.readRegister(registerToRead);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("writeRegister",
        [](Sensor& vs, ConfigurationRegister* registerToWrite, const bool retryOnFailure) {
            Error error = vs.writeRegister(registerToWrite, retryOnFailure);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("writeRegister",
        [](Sensor& vs, ConfigurationRegister* registerToWrite) {
            Error error = vs.writeRegister(registerToWrite);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("writeSettings",
        [](Sensor& vs) {
            Error error = vs.writeSettings();
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("restoreFactorySettings",
        [](Sensor& vs) {
            Error error = vs.restoreFactorySettings();    
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("knownMagneticDisturbance",
        [](Sensor& vs, const KnownMagneticDisturbance::State state) {
            Error error = vs.knownMagneticDisturbance(state);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("knownAccelerationDisturbance",
        [](Sensor& vs, const KnownAccelerationDisturbance::State state) {
            Error error = vs.knownAccelerationDisturbance(state);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("setInitialHeading",
        [](Sensor& vs, const float heading) {
            Error error = vs.setInitialHeading(heading);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("setInitialHeading",
        [](Sensor& vs, Ypr ypr) {
            Error error = vs.setInitialHeading(ypr);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("setInitialHeading",
        [](Sensor& vs, Quat quat) {
            Error error = vs.setInitialHeading(quat);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("asyncOutputEnable",
        [](Sensor& vs, const AsyncOutputEnable::State state) {
            Error error = vs.asyncOutputEnable(state);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("setFilterBias",
        [](Sensor& vs) {
            Error error = vs.setFilterBias();
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("sendCommand", 
        [](Sensor& vs, GenericCommand* commandToSend, Sensor::SendCommandBlockMode waitMode, const Microseconds waitLength, const Microseconds timeoutThreshUs) {
            Error error = vs.sendCommand(commandToSend, waitMode, waitLength, timeoutThreshUs);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("sendCommand", 
        [](Sensor& vs, GenericCommand* commandToSend, Sensor::SendCommandBlockMode waitMode, const Microseconds waitLength) {
            Error error = vs.sendCommand(commandToSend, waitMode, waitLength);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("sendCommand", 
        [](Sensor& vs, GenericCommand* commandToSend, Sensor::SendCommandBlockMode waitMode) {
            Error error = vs.sendCommand(commandToSend, waitMode, Config::Sensor::commandSendTimeoutLength);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("serialSend",
        [](Sensor& vs, const char* msgToSend, const size_t len) {
            Error error = vs.serialSend(msgToSend, len);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("serialSend",
        [](Sensor& vs, const std::string& msgToSend) {
            Error error = vs.serialSend(msgToSend.c_str(), msgToSend.length());
            if (error != Error::None) { throwError(error); }
        }
        )
        // Additional Logging
        .def("registerReceivedByteBuffer", &Sensor::registerReceivedByteBuffer)
        .def("deregisterReceivedByteBuffer", &Sensor::deregisterReceivedByteBuffer)
        .def("subscribeToMessage",
            [](Sensor& vs, PacketQueue_Interface* q, const Sensor::SyncByte& b) {
                vs.subscribeToMessage(q, b);
            }
        )
        .def("subscribeToMessage",
        [](Sensor& vs, PacketQueue_Interface* q, const Sensor::BinaryOutputMeasurements& b, Sensor::FaSubscriberFilterType c = Sensor::FaSubscriberFilterType::ExactMatch) {
            Error error = vs.subscribeToMessage(q, b, c);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("subscribeToMessage",
        [](Sensor& vs, PacketQueue_Interface* q, const AsciiHeader& b, Sensor::AsciiSubscriberFilterType c = Sensor::AsciiSubscriberFilterType::StartsWith) {
            Error error = vs.subscribeToMessage(q, b, c);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("subscribeToMessage",
        [](Sensor& vs, PacketQueue_Interface* q, Sensor::Fb00SubscriberFilter filter) {
            Error error = vs.subscribeToMessage(q, filter);
            if (error != Error::None) { throwError(error); }
        }
        )
        .def("unsubscribeFromMessage",
            [](Sensor& vs, PacketQueue_Interface* q, const Registers::System::BinaryOutputMeasurements& b) {
                vs.unsubscribeFromMessage(q, b);
            }
        )
        .def("unsubscribeFromMessage",
            [](Sensor& vs, PacketQueue_Interface* q, const AsciiHeader& b) {
                vs.unsubscribeFromMessage(q, b);
            }
        )
        .def("unsubscribeFromMessage",
            [](Sensor& vs, PacketQueue_Interface* q, const Sensor::SyncByte& b) {
                vs.unsubscribeFromMessage(q, b);
            }
        )
        // Error Handling
        .def("asyncErrorQueueSize", &Sensor::asyncErrorQueueSize)
        .def("clearAsyncErrorQueue", &Sensor::clearAsyncErrorQueue)
            
        .def("throwIfAsyncError",
        [](Sensor& vs) {
            auto asyncError = vs.getNextAsyncError();
            if (asyncError) 
            {
                throwError(asyncError->error);
            }
        }
        )
        
        .def("__enter__", [](Sensor& vs) {
        return &vs;
        }
        )
        .def("__exit__",
        [](Sensor& vs,
            const std::optional<pybind11::type> &exc_type,
            const std::optional<pybind11::object> &exc_value,
            const std::optional<pybind11::object> &traceback) {
        
            vs.disconnect();
        }
        )
        ;
    py::class_<BridgeSensor, Sensor> sensor(m, "Sensor");

    py::implicitly_convertible<py::int_, BridgeSensor::BaudRate>();

    py::enum_<BridgeSensor::MeasQueueMode>(sensor, "MeasQueueMode", py::module_local())
    .value("Off", BridgeSensor::MeasQueueMode::Off)
    .value("Force", BridgeSensor::MeasQueueMode::Force)
    .value("Try", BridgeSensor::MeasQueueMode::Try)
#if THREADING_ENABLE
    .value("Retry", BridgeSensor::MeasQueueMode::Retry)
#endif  
    ;

    sensor.def(py::init<BridgeSensor::MeasQueueMode>(), py::arg("mode") = BridgeSensor::MeasQueueMode::Force)
        .def(py::init<ByteBuffer&, ByteBuffer&, BridgeSensor::MeasQueueMode>(),
            py::arg("mainBuffer"), py::arg("fbBuffer"), py::arg("mode") = BridgeSensor::MeasQueueMode::Force)
        // Serial Connectivity
        .def("connect",
              [](BridgeSensor& vs, Serial_Base::PortName portName, BridgeSensor::BaudRate baudRate, bool monitorAsyncErrors) {
                Error error = vs.connect(portName, baudRate, monitorAsyncErrors);
                if (error != Error::None) { throwError(error); }
              },
              py::arg("portName"), py::arg("baudRate"), py::arg("monitorAsyncErrors") = false
        )
        .def("connect",
              [](BridgeSensor& vs, Filesystem::FilePath& fileName) {
                Error error = vs.connect(fileName);
                if (error != Error::None) { throwError(error); }
              }
        )
        .def("autoConnect",
              [](BridgeSensor& vs, Serial_Base::PortName portName, bool monitorAsyncErrors) {
                Error error = vs.autoConnect(portName, monitorAsyncErrors);
                if (error != Error::None) { throwError(error); }
              },
              py::arg("portName"), py::arg("monitorAsyncErrors") = false
        )
        .def("disconnect", [] (BridgeSensor& vs) {
            vs.disconnect();
            vs.disableMonitor();
        } 
        )
        .def("reset",
        [](BridgeSensor& vs) {
            Error error = vs.reset(vs.asyncErrorThrowingEnabled());
            if (error != Error::None) { throwError(error); }
        }
        )
        ;

    // Sensor aliases from Register module
    m.attr("Sensor").attr("BinaryOutputMeasurements") = m.attr("Registers").attr("System").attr("BinaryOutputMeasurements");
    m.attr("Sensor").attr("BaudRate") = m.attr("Registers").attr("System").attr("BaudRate").attr("BaudRates");
    
    // Sensor aliases from Packet module
    m.attr("Sensor").attr("SyncByte") = m.attr("PacketDetails").attr("SyncByte");

    py::enum_<Registers::System::BaudRate::SerialPort>(sensor, "SerialPort", py::module_local())
        .value("ActiveSerial", Registers::System::BaudRate::SerialPort::ActiveSerial)
        .value("Serial1", Registers::System::BaudRate::SerialPort::Serial1)
        .value("Serial2", Registers::System::BaudRate::SerialPort::Serial2)
        .value("Poll", Registers::System::BaudRate::SerialPort::Poll);

    // Sensor subtype definitions
    py::enum_<BridgeSensor::SendCommandBlockMode>(sensor, "SendCommandBlockMode", py::module_local())
        .value("none", BridgeSensor::SendCommandBlockMode::None)
        .value("Block", BridgeSensor::SendCommandBlockMode::Block)
        .value("BlockWithRetry", BridgeSensor::SendCommandBlockMode::BlockWithRetry);

    py::enum_<BridgeSensor::FaSubscriberFilterType>(sensor, "FaSubscriberFilterType", py::module_local())
        .value("ExactMatch", BridgeSensor::FaSubscriberFilterType::ExactMatch)
        .value("AnyMatch", BridgeSensor::FaSubscriberFilterType::AnyMatch)
        .value("NotExactMatch", BridgeSensor::FaSubscriberFilterType::NotExactMatch);
    
    py::enum_<BridgeSensor::AsciiSubscriberFilterType>(sensor, "AsciiSubscriberFilterType", py::module_local())
        .value("StartsWith", BridgeSensor::AsciiSubscriberFilterType::StartsWith)
        .value("DoesNotStartsWith", BridgeSensor::AsciiSubscriberFilterType::DoesNotStartWith);

    py::class_<BridgeSensor::Fb00SubscriberFilter>(sensor, "Fb00SubscriberFilter", py::module_local())
        .def(py::init<>())
        .def(py::init<bool, bool>())
        .def_property("packet",
            [](const BridgeSensor::Fb00SubscriberFilter& filter) -> bool { return filter.packet; },
            [](BridgeSensor::Fb00SubscriberFilter& filter, bool value) -> void { filter.packet = value; }
        )
        .def_property("completedFaMessage",
            [](const BridgeSensor::Fb00SubscriberFilter& filter) -> bool { return filter.completedFaMessage; },
            [](BridgeSensor::Fb00SubscriberFilter& filter, bool value) -> void { filter.completedFaMessage = value; }
        )
        // .def("__repr__", [](const BridgeSensor::Fb00SubscriberFilter& filter) {
        //     return "<Fb00SubscriberFilter(Packet=" + std::to_string(filter.Packet) +
        //            ", CompletedFaMessage=" + std::to_string(filter.CompletedFaMessage) + ")>";
        // })
        ;

    py::class_<BinaryHeader>(m, "BinaryHeader")
        .def(py::init<>());

}
}
// clang-format on
