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

#include "vectornav/CLI_Sensor.hpp"

using namespace msclr::interop;

namespace VNSDK
{

void Sensor::Connect(String ^ portName, UInt32 baudRate) { Connect(portName, static_cast<BaudRate>(baudRate), false); }

void Sensor::Connect(String ^ portName, BaudRate baudRate) { Connect(portName, baudRate, false); }

void Sensor::Connect(String ^ portName, UInt32 baudRate, bool monitorAsyncErrors) { Connect(portName, static_cast<BaudRate>(baudRate), monitorAsyncErrors); }

void Sensor::Connect(String ^ portName, BaudRate baudRate, bool monitorAsyncErrors)
{
    marshal_context ^ context = gcnew marshal_context();
    VN::Error error = _sensor->connect(context->marshal_as<const char*>(portName), Registers::System::ToNativeInstance(baudRate), monitorAsyncErrors);
    if (error != VN::Error::None)
    {
        throw gcnew VnException(error);
    }
}

void Sensor::Connect(String^ fileName)
{
    marshal_context ^ context = gcnew marshal_context();
    VN::Error error = _sensor->connect(context->marshal_as<const char*>(fileName));
    if (error != VN::Error::None)
    {
        throw gcnew VnException(error);
    }
}

void Sensor::AutoConnect(String ^ portName) { AutoConnect(portName, false); }

void Sensor::AutoConnect(String ^ portName, bool monitorAsyncErrors)
{
    marshal_context ^ context = gcnew marshal_context();
    VN::Error error = _sensor->autoConnect(context->marshal_as<const char*>(portName), monitorAsyncErrors);
    if (error != VN::Error::None)
    {
        throw gcnew VnException(error);
    }
    delete context;
}


bool Sensor::VerifySensorConnectivity()
{
    return _sensor->verifySensorConnectivity();
}

String ^ Sensor::ConnectedPortName()
{
    auto connectedPortName = _sensor->connectedPortName();
    if (!connectedPortName.has_value()) { throw gcnew VnException(VN::Error::SerialPortClosed); }
    String ^ portName = msclr::interop::marshal_as<String ^>(connectedPortName.value().c_str());
    return portName;
}

// Sensor::BaudRate Sensor::ConnectedBaudRate()
// {
//     auto connectedBaudRate = _sensor->connectedBaudRate();
//     if (!connectedBaudRate.has_value()) { throw gcnew VnException(VN::Error::SerialPortClosed); }
//     return static_cast<Sensor::BaudRate>(connectedBaudRate.value());
// }

uint32_t Sensor::ConnectedBaudRate()
{
    auto connectedBaudRate = _sensor->connectedBaudRate();
    if (!connectedBaudRate.has_value()) { throw gcnew VnException(VN::Error::SerialPortClosed); }
    return static_cast<uint32_t>(connectedBaudRate.value());
}

void Sensor::ChangeBaudRate(BaudRate baudRate)
{
    VN::Error error = _sensor->changeBaudRate(Registers::System::ToNativeInstance(baudRate));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ChangeBaudRate(UInt32 baudRate)
{
    VN::Error error = _sensor->changeBaudRate(static_cast<VN::Registers::System::BaudRate::BaudRates>(baudRate));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ChangeBaudRate(BaudRate baudRate, Registers::System::BaudRate::SerialPort serialPort)
{
    VN::Error error = _sensor->changeBaudRate(Registers::System::ToNativeInstance(baudRate),
        static_cast<VN::Registers::System::BaudRate::SerialPort>(serialPort));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ChangeBaudRate(UInt32 baudRate, System::Byte serialPort)
{
    VN::Error error = _sensor->changeBaudRate(static_cast<VN::Registers::System::BaudRate::BaudRates>(baudRate),
        static_cast<VN::Registers::System::BaudRate::SerialPort>(serialPort));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ChangeHostBaudRate(BaudRate baudRate)
{
    VN::Error error = _sensor->changeHostBaudRate(Registers::System::ToNativeInstance(baudRate));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ChangeHostBaudRate(UInt32 baudRate)
{
    VN::Error error = _sensor->changeHostBaudRate(static_cast<VN::Registers::System::BaudRate::BaudRates>(baudRate));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::Disconnect()
{
    _sensor->disconnect();
}

// ----------------------
// Accessing Measurements
// ----------------------

bool Sensor::HasMeasurement()
{
    return _sensor->hasMeasurement();
}

Nullable<CompositeData> Sensor::GetNextMeasurement()
{
    auto measurement = _sensor->getNextMeasurement();
    if (measurement) { return CompositeData(*measurement); }
    return Nullable<CompositeData>();
}

Nullable<CompositeData> Sensor::GetMostRecentMeasurement()
{
    auto measurement = _sensor->getMostRecentMeasurement();
    if (measurement) { return CompositeData(*measurement); }
    return Nullable<CompositeData>();
}

// ----------------------
// Commands
// ----------------------

void Sensor::ReadRegister(VNSDK::Registers::ConfigRegister ^ reg)
{
    VN::Error error = _sensor->readRegister(reg->GetReference());
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::ReadRegister(VNSDK::Registers::MeasRegister ^ reg)
{
    VN::Error error = _sensor->readRegister(reg->GetReference());
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::WriteRegister(VNSDK::Registers::ConfigRegister ^ reg)
{
    VN::Error error = _sensor->writeRegister((VN::ConfigurationRegister*)reg->GetReference());
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::WriteSettings()
{
    VN::Error error = _sensor->writeSettings();
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::Reset()
{
    VN::Error error = _sensor->reset(_sensor->asyncErrorThrowingEnabled());
    if (error != VN::Error::None)
    {
        throw gcnew VnException(error);
    }
}

void Sensor::RestoreFactorySettings()
{
    VN::Error error = _sensor->restoreFactorySettings();
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::KnownMagneticDisturbance(KnownMagneticDisturbance::State state)
{
    VN::Error error = _sensor->knownMagneticDisturbance(ToNativeInstance(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::KnownMagneticDisturbance(uint8_t state)
{
    VN::Error error = _sensor->knownMagneticDisturbance(static_cast<VN::KnownMagneticDisturbance::State>(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::KnownAccelerationDisturbance(KnownAccelerationDisturbance::State state)
{
    VN::Error error = _sensor->knownAccelerationDisturbance(ToNativeInstance(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::KnownAccelerationDisturbance(uint8_t state)
{
    VN::Error error = _sensor->knownAccelerationDisturbance(static_cast<VN::KnownAccelerationDisturbance::State>(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::SetInitialHeading(float heading)
{
    VN::Error error = _sensor->setInitialHeading(heading);
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::SetInitialHeading(Ypr ypr)
{
    VN::Ypr nativeYpr{ypr.yaw, ypr.pitch, ypr.roll};
    VN::Error error = _sensor->setInitialHeading(nativeYpr);
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::SetInitialHeading(Quaternion quat)
{
    const VN::Quat nativeQuat{VN::Vec3f{quat.vector.x, quat.vector.y, quat.vector.z}, quat.scalar};
    VN::Error error = _sensor->setInitialHeading(nativeQuat);
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::AsyncOutputEnable(AsyncOutputEnable::State state)
{
    VN::Error error = _sensor->asyncOutputEnable(ToNativeInstance(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::AsyncOutputEnable(uint8_t state)
{
    VN::Error error = _sensor->asyncOutputEnable(static_cast<VN::AsyncOutputEnable::State>(state));
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::SetFilterBias()
{
    VN::Error error = _sensor->setFilterBias();
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

// Error sendCommand(Command* commandToSend, SendCommandBlockMode waitMode, const Microseconds waitLength = Config::Sensor::commandSendTimeoutLength,
//     const Microseconds timeoutThreshold = Config::CommandProcessor::commandRemovalTimeoutLength) noexcept;

void Sensor::SendCommand(GenericCommand^% commandToSend, Sensor::SendCommandBlockMode waitMode)
{
    VN::BridgeSensor::SendCommandBlockMode waitModeC = static_cast<VN::BridgeSensor::SendCommandBlockMode>(static_cast<int>(waitMode));
    VN::Error error = _sensor->sendCommand(commandToSend->GetReference(), waitModeC); 	
    if (error != VN::Error::None) {throw gcnew VnException(error); }
}

void Sensor::SerialSend(String ^ msgToSend)
{
    marshal_context ^ context = gcnew marshal_context();
    auto msgChar = context->marshal_as<const char*>(msgToSend);
    _sensor->serialSend(msgChar, strlen(msgChar));
    delete context;
}

// ----------------------
// Error Handling
// ----------------------

void Sensor::ThrowIfAsyncError() {
    auto asyncError = _sensor->getNextAsyncError();
    if (asyncError) 
    {
        throw gcnew VnException(asyncError->error);
    }
}

System::DateTime Sensor::Now() 
{
    return _clock.now();
}

// ----------------------
// Additional logging
// ----------------------

void Sensor::SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter)
{
    VN::Error error = _sensor->subscribeToMessage(queueToSubscribe->ToNativePointer(), binaryOutputMeasurementFilter->ToNativePointer());
    if (error != VN::Error::None)
	{
		VnException^ vnException = gcnew VnException(error);
		throw vnException;
	}
}

void Sensor::SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter, FaSubscriberFilterType filterType)
{
    VN::Error error = _sensor->subscribeToMessage(queueToSubscribe->ToNativePointer(), binaryOutputMeasurementFilter->ToNativePointer(), ToNativeInstance(filterType));
    if (error != VN::Error::None)
	{
		VnException^ vnException = gcnew VnException(error);
		throw vnException;
	}
}

void Sensor::SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, System::String^ asciiHeaderFilter)
{
    std::string nativeString = msclr::interop::marshal_as<std::string>(asciiHeaderFilter);
    VN::AsciiHeader header(nativeString.c_str());
    VN::Error error = _sensor->subscribeToMessage(queueToSubscribe->ToNativePointer(), header);
    if (error != VN::Error::None)
	{
		VnException^ vnException = gcnew VnException(error);
		throw vnException;
	}
}

void Sensor::SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, System::String^ asciiHeaderFilter, AsciiSubscriberFilterType filterType)
{
    std::string nativeString = msclr::interop::marshal_as<std::string>(asciiHeaderFilter);
    VN::AsciiHeader header(nativeString.c_str());
    VN::Error error = _sensor->subscribeToMessage(queueToSubscribe->ToNativePointer(), header, ToNativeInstance(filterType));
    if (error != VN::Error::None)
	{
		VnException^ vnException = gcnew VnException(error);
		throw vnException;
	}
}

void Sensor::SubscribeToMessage(ManagedQueuePointer^ queueToSubscribe, SyncByte sb)
{
    VN::Error error = _sensor->subscribeToMessage(queueToSubscribe->ToNativePointer(), ToNativeInstance(sb));
    if (error != VN::Error::None)
    {
        VnException^ vnException = gcnew VnException(error);
        throw vnException;
    }
}

void Sensor::UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, BinaryOutputMeasurements^ binaryOutputMeasurementFilter)
{
    _sensor->unsubscribeFromMessage(queueToUnsubscribe->ToNativePointer(), binaryOutputMeasurementFilter->ToNativePointer());
}

void Sensor::UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, System::String^ asciiHeaderFilter)
{
    std::string nativeString = msclr::interop::marshal_as<std::string>(asciiHeaderFilter);
    VN::AsciiHeader header(nativeString.c_str());
    _sensor->unsubscribeFromMessage(queueToUnsubscribe->ToNativePointer(), header);
}

void Sensor::UnsubscribeFromMessage(ManagedQueuePointer^ queueToUnsubscribe, SyncByte syncByte)
{
    _sensor->unsubscribeFromMessage(queueToUnsubscribe->ToNativePointer(), ToNativeInstance(syncByte));
}

void Sensor::RegisterReceivedByteBuffer(ByteBuffer^ buffer)
{
    _sensor->registerReceivedByteBuffer(buffer->GetNativePointer());
}

void Sensor::DeregisterReceivedByteBuffer()
{
    _sensor->deregisterReceivedByteBuffer();
}

#if (PLUGIN_DATAEXPORT)
void Sensor::RegisterDataExporter(DataExport::Exporter^ exporter)
{
    VN::Error error = _sensor->subscribeToMessage(exporter->GetQueuePointer(), "VN", VN::BridgeSensor::AsciiSubscriberFilterType::StartsWith);
    if (error != VN::Error::None) { throw gcnew VnException(error); }
    error = _sensor->subscribeToMessage(exporter->GetQueuePointer(), VN::BridgeSensor::BinaryOutputMeasurements{}, VN::BridgeSensor::FaSubscriberFilterType::AnyMatch);
    if (error != VN::Error::None) { throw gcnew VnException(error); }
}

void Sensor::DeregisterDataExporter(DataExport::Exporter^ exporter)
{
    _sensor->unsubscribeFromMessage(exporter->GetQueuePointer(), "VN");
    _sensor->unsubscribeFromMessage(exporter->GetQueuePointer(), VN::BridgeSensor::BinaryOutputMeasurements{});
}
#endif

} // namespace VNSDK
