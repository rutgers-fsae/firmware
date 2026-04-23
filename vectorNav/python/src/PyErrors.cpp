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
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include <sstream>

#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Interface/Errors.hpp"

namespace py = pybind11;

namespace VN
{

    
// Structures to allow visibility in Python
// Sensor VN Error
struct HardFault : public std::runtime_error {HardFault() : std::runtime_error("HardFault") {}};
struct SerialBufferOverflow : public std::runtime_error {SerialBufferOverflow() : std::runtime_error("SerialBufferOverflow") {}};
struct InvalidChecksum : public std::runtime_error {InvalidChecksum() : std::runtime_error("InvalidChecksum") {}};
struct InvalidCommand : public std::runtime_error {InvalidCommand() : std::runtime_error("InvalidCommand") {}};
struct NotEnoughParameters : public std::runtime_error {NotEnoughParameters() : std::runtime_error("NotEnoughParameters") {}};
struct TooManyParameters : public std::runtime_error {TooManyParameters() : std::runtime_error("TooManyParameters") {}};
struct InvalidParameter : public std::runtime_error {InvalidParameter() : std::runtime_error("InvalidParameter") {}};
struct InvalidRegister : public std::runtime_error {InvalidRegister() : std::runtime_error("InvalidRegister") {}};
struct UnauthorizedAccess : public std::runtime_error {UnauthorizedAccess() : std::runtime_error("UnauthorizedAccess") {}};
struct WatchdogReset : public std::runtime_error {WatchdogReset() : std::runtime_error("WatchdogReset") {}};
struct OutputBufferOverflow : public std::runtime_error {OutputBufferOverflow() : std::runtime_error("OutputBufferOverflow") {}};
struct InsufficientBaudRate : public std::runtime_error {InsufficientBaudRate() : std::runtime_error("InsufficientBaudRate") {}};
struct ErrorBufferOverflow : public std::runtime_error {ErrorBufferOverflow() : std::runtime_error("ErrorBufferOverflow") {}};

// Command Processor Errors
struct CommandResent : public std::runtime_error {CommandResent() : std::runtime_error("CommandResent") {}};
struct CommandQueueFull : public std::runtime_error {CommandQueueFull() : std::runtime_error("CommandQueueFull") {}};
struct ResponseTimeout : public std::runtime_error {ResponseTimeout() : std::runtime_error("ResponseTimeout") {}};
struct ReceivedUnexpectedMessage : public std::runtime_error {ReceivedUnexpectedMessage() : std::runtime_error("ReceivedUnexpectedMessage") {}};

// Sensor Errors
struct MeasurementQueueFull : public std::runtime_error {MeasurementQueueFull() : std::runtime_error("MeasurementQueueFull") {}};
struct PrimaryBufferFull : public std::runtime_error {PrimaryBufferFull() : std::runtime_error("PrimaryBufferFull") {}};
struct MessageSubscriberCapacityReached : public std::runtime_error {MessageSubscriberCapacityReached() : std::runtime_error("MessageSubscriberCapacityReached") {}};
struct ReceivedInvalidResponse : public std::runtime_error {ReceivedInvalidResponse() : std::runtime_error("ReceivedInvalidResponse") {}};
struct InvalidAccessPrimaryBuffer : public std::runtime_error {InvalidAccessPrimaryBuffer() : std::runtime_error("InvalidAccessPrimaryBuffer") {}};
struct BufferFull : public std::runtime_error {BufferFull() : std::runtime_error("BufferFull") {}};
struct AlreadyConnected : public std::runtime_error {AlreadyConnected() : std::runtime_error("AlreadyConnected") {}};

// Serial Errors
struct InvalidPortName : public std::runtime_error {InvalidPortName() : std::runtime_error("InvalidPortName") {}};
struct AccessDenied : public std::runtime_error {AccessDenied() : std::runtime_error("AccessDenied") {}};
struct SerialPortClosed : public std::runtime_error {SerialPortClosed() : std::runtime_error("SerialPortClosed") {}};
struct UnsupportedBaudRate : public std::runtime_error {UnsupportedBaudRate() : std::runtime_error("UnsupportedBaudRate") {}};
struct SerialReadFailed : public std::runtime_error {SerialReadFailed() : std::runtime_error("SerialReadFailed") {}};
struct SerialWriteFailed : public std::runtime_error {SerialWriteFailed() : std::runtime_error("SerialWriteFailed") {}};
struct UnexpectedSerialError : public std::runtime_error {UnexpectedSerialError() : std::runtime_error("UnexpectedSerialError") {}};

// Packet Sync Errors
struct ReceivedByteBufferFull : public std::runtime_error {ReceivedByteBufferFull() : std::runtime_error("ReceivedByteBufferFull") {}};
struct ParsingFailed : public std::runtime_error {ParsingFailed() : std::runtime_error("ParsingFailed") {}};
struct PacketQueueFull : public std::runtime_error {PacketQueueFull() : std::runtime_error("PacketQueueFull") {}};
struct PacketQueueOverrun : public std::runtime_error {PacketQueueOverrun() : std::runtime_error("PacketQueueOverrun") {}};
struct PacketQueueNull : public std::runtime_error {PacketQueueNull() : std::runtime_error("PacketQueueNull") {}};

// File Errors
struct FileDoesNotExist : public std::runtime_error {FileDoesNotExist() : std::runtime_error("FileDoesNotExist") {}};
struct FileOpenFailed : public std::runtime_error {FileOpenFailed() : std::runtime_error("FileOpenFailed") {}};
struct FileReadFailed : public std::runtime_error {FileReadFailed() : std::runtime_error("FileReadFailed") {}};
struct FileWriteFailed : public std::runtime_error {FileWriteFailed() : std::runtime_error("FileWriteFailed") {}};

void init_errors(py::module& m)
{
    // Sensor VN Errors
    py::register_exception<HardFault>(m, "HardFault");
    py::register_exception<SerialBufferOverflow>(m, "SerialBufferOverflow");
    py::register_exception<InvalidChecksum>(m, "InvalidChecksum");
    py::register_exception<InvalidCommand>(m, "InvalidCommand");
    py::register_exception<NotEnoughParameters>(m, "NotEnoughParameters");
    py::register_exception<TooManyParameters>(m, "TooManyParameters");
    py::register_exception<InvalidParameter>(m, "InvalidParameter");
    py::register_exception<InvalidRegister>(m, "InvalidRegister");
    py::register_exception<UnauthorizedAccess>(m, "UnauthorizedAccess");
    py::register_exception<WatchdogReset>(m, "WatchdogReset");
    py::register_exception<OutputBufferOverflow>(m, "OutputBufferOverflow");
    py::register_exception<InsufficientBaudRate>(m, "InsufficientBaudRate");
    py::register_exception<ErrorBufferOverflow>(m, "ErrorBufferOverflow");
    
    // Command Processor Errors
    py::register_exception<CommandResent>(m, "CommandResent");
    py::register_exception<CommandQueueFull>(m, "CommandQueueFull");
    py::register_exception<ResponseTimeout>(m, "ResponseTimeout");
    py::register_exception<ReceivedUnexpectedMessage>(m, "ReceivedUnexpectedMessage");
    
    // Sensor Errors
    py::register_exception<MeasurementQueueFull>(m, "MeasurementQueueFull");
    py::register_exception<PrimaryBufferFull>(m, "PrimaryBufferFull");
    py::register_exception<MessageSubscriberCapacityReached>(m, "MessageSubscriberCapacityReached");
    py::register_exception<ReceivedInvalidResponse>(m, "ReceivedInvalidResponse");
    py::register_exception<InvalidAccessPrimaryBuffer>(m, "InvalidAccessPrimaryBuffer");
    py::register_exception<BufferFull>(m, "BufferFull");
    py::register_exception<AlreadyConnected>(m, "AlreadyConnected");
    
    // Serial Errors
    py::register_exception<InvalidPortName>(m, "InvalidPortName");
    py::register_exception<AccessDenied>(m, "AccessDenied");
    py::register_exception<SerialPortClosed>(m, "SerialPortClosed");
    py::register_exception<UnsupportedBaudRate>(m, "UnsupportedBaudRate");
    py::register_exception<SerialReadFailed>(m, "SerialReadFailed");
    py::register_exception<SerialWriteFailed>(m, "SerialWriteFailed");
    py::register_exception<UnexpectedSerialError>(m, "UnexpectedSerialError");
    
    // Packet Sync Errors
    py::register_exception<ReceivedByteBufferFull>(m, "ReceivedByteBufferFull");
    py::register_exception<ParsingFailed>(m, "ParsingFailed");
    py::register_exception<PacketQueueFull>(m, "PacketQueueFull");
    py::register_exception<PacketQueueOverrun>(m, "PacketQueueOverrun");
    py::register_exception<PacketQueueNull>(m, "PacketQueueNull");
    
    // File Errors
    py::register_exception<FileDoesNotExist>(m, "FileDoesNotExist");
    py::register_exception<FileOpenFailed>(m, "FileOpenFailed");
    py::register_exception<FileReadFailed>(m, "FileReadFailed");
    py::register_exception<FileWriteFailed>(m, "FileWriteFailed");


    py::enum_<Error>(m, "Error")
        // Sensor VN Errors
        .value("HardFault", Error::HardFault)
        .value("SerialBufferOverflow", Error::SerialBufferOverflow)
        .value("InvalidChecksum", Error::InvalidChecksum)
        .value("InvalidCommand", Error::InvalidCommand)
        .value("NotEnoughParameters", Error::NotEnoughParameters)
        .value("TooManyParameters", Error::TooManyParameters)
        .value("InvalidParameter", Error::InvalidParameter)
        .value("InvalidRegister", Error::InvalidRegister)
        .value("UnauthorizedAccess", Error::UnauthorizedAccess)
        .value("WatchdogReset", Error::WatchdogReset)
        .value("OutputBufferOverflow", Error::OutputBufferOverflow)
        .value("InsufficientBaudRate", Error::InsufficientBaudRate)
        .value("ErrorBufferOverflow", Error::ErrorBufferOverflow)

        // CommandProcessorErrors
        .value("CommandResent", Error::CommandResent)
        .value("CommandQueueFull", Error::CommandQueueFull)
        .value("ResponseTimeout", Error::ResponseTimeout)
        .value("ReceivedUnexpectedMessage", Error::ReceivedUnexpectedMessage)

        // SensorErrors
        .value("MeasurementQueueFull", Error::MeasurementQueueFull)
        .value("PrimaryBufferFull", Error::PrimaryBufferFull)
        .value("MessageSubscriberCapacityReached", Error::MessageSubscriberCapacityReached)
        .value("ReceivedInvalidResponse", Error::ReceivedInvalidResponse)
        .value("InvalidAccessPrimaryBuffer", Error::InvalidAccessPrimaryBuffer)
        .value("BufferFull", Error::BufferFull)
        .value("AlreadyConnected", Error::AlreadyConnected)

        // SerialErrors
        .value("InvalidPortName", Error::InvalidPortName)
        .value("AccessDenied", Error::AccessDenied)
        .value("SerialPortClosed", Error::SerialPortClosed)
        .value("UnsupportedBaudRate", Error::UnsupportedBaudRate)
        .value("SerialReadFailed", Error::SerialReadFailed)
        .value("SerialWriteFailed", Error::SerialWriteFailed)
        .value("UnexpectedSerialError", Error::UnexpectedSerialError)

        // PacketSyncErrors
        .value("ReceivedByteBufferFull", Error::ReceivedByteBufferFull)
        .value("ParsingFailed", Error::ParsingFailed)
        .value("PacketQueueFull", Error::PacketQueueFull)
        .value("PacketQueueOverrun", Error::PacketQueueOverrun)
        .value("PacketQueueNull", Error::PacketQueueNull)

        // FileErrors
        .value("FileDoesNotExist", Error::FileDoesNotExist)
        .value("FileOpenFailed", Error::FileOpenFailed)
        .value("FileReadFailed", Error::FileReadFailed)
        .value("FileWriteFailed", Error::FileWriteFailed);

    py::class_<AsyncError>(m, "AsyncError")
        .def(py::init<>())
        .def_readwrite("error", &AsyncError::error)
        .def_readwrite("message", &AsyncError::message)
        .def_readwrite("timestamp", &AsyncError::timestamp)
        .def("__repr__", [](const AsyncError& as) -> std::string {
            std::ostringstream oss;
            oss << as;
            return oss.str();
            })
        ;
}

void throwError(Error error){
    switch (error)
    {
        // Sensor VN Errors
        case Error::HardFault:
            throw HardFault();
        case Error::SerialBufferOverflow:
            throw SerialBufferOverflow();
        case Error::InvalidChecksum:
            throw InvalidChecksum();
        case Error::InvalidCommand:
            throw InvalidCommand();
        case Error::NotEnoughParameters:
            throw NotEnoughParameters();
        case Error::TooManyParameters:
            throw TooManyParameters();
        case Error::InvalidParameter:
            throw InvalidParameter();
        case Error::InvalidRegister:
            throw InvalidRegister();
        case Error::UnauthorizedAccess:
            throw UnauthorizedAccess();
        case Error::WatchdogReset:
            throw WatchdogReset();
        case Error::OutputBufferOverflow:
            throw OutputBufferOverflow();
        case Error::InsufficientBaudRate:
            throw InsufficientBaudRate();
        case Error::ErrorBufferOverflow:
            throw ErrorBufferOverflow();

        // Command Processor Errors
        case Error::CommandResent:
            throw CommandResent();
        case Error::CommandQueueFull:
            throw CommandQueueFull();
        case Error::ResponseTimeout:
            throw ResponseTimeout();
        case Error::ReceivedUnexpectedMessage:
            throw ReceivedUnexpectedMessage();
        
        // Sensor Errors
        case Error::MeasurementQueueFull:
            throw MeasurementQueueFull();
        case Error::PrimaryBufferFull:
            throw PrimaryBufferFull();
        case Error::MessageSubscriberCapacityReached:
            throw MessageSubscriberCapacityReached();
        case Error::ReceivedInvalidResponse:
            throw ReceivedInvalidResponse();
        case Error::InvalidAccessPrimaryBuffer:
            throw InvalidAccessPrimaryBuffer();
        case Error::BufferFull:
            throw BufferFull();
        case Error::AlreadyConnected:
            throw AlreadyConnected();

        // Serial Errors
        case Error::InvalidPortName:
            throw InvalidPortName();
        case Error::AccessDenied:
            throw AccessDenied();
        case Error::SerialPortClosed:
            throw SerialPortClosed();
        case Error::UnsupportedBaudRate:
            throw UnsupportedBaudRate();
        case Error::SerialReadFailed:
            throw SerialReadFailed();
        case Error::SerialWriteFailed:
            throw SerialWriteFailed();
        case Error::UnexpectedSerialError:
            throw UnexpectedSerialError();
        
        // Packet Sync Errors
        case Error::ReceivedByteBufferFull:
            throw ReceivedByteBufferFull();
        case Error::ParsingFailed:
            throw ParsingFailed();
        case Error::PacketQueueFull:
            throw PacketQueueFull();
        case Error::PacketQueueOverrun:
            throw PacketQueueOverrun();
        case Error::PacketQueueNull:
            throw PacketQueueNull();

        // File Errors
        case Error::FileDoesNotExist:
            throw FileDoesNotExist();
        case Error::FileOpenFailed:
            throw FileOpenFailed();        
        case Error::FileReadFailed:
            throw FileReadFailed();
        case Error::FileWriteFailed:
            throw FileWriteFailed();

        default:
            throw std::runtime_error("Unknown error code.");
        
    }
}

}
// clang-format on
