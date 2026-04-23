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

#ifndef VN_CLI_COMMANDS_HPP_
#define VN_CLI_COMMANDS_HPP_

#include "CLI_GenericCommand.hpp"

#include <msclr/marshal.h>

#pragma managed(push, off)
#include "vectornav/Interface/Commands.hpp"
#pragma managed(pop)

using namespace System;
using namespace msclr::interop;

namespace VNSDK
{

	public ref class WriteSettings : public GenericCommand
	{
	public:
		WriteSettings() : GenericCommand(new VN::WriteSettings()) {};
	};

	public ref class RestoreFactorySettings : public GenericCommand
	{
	public:
		RestoreFactorySettings() : GenericCommand(new VN::RestoreFactorySettings()) {};
	};

	public ref class Reset : public GenericCommand
	{
	public:
		Reset() : GenericCommand(new VN::Reset()) {};
	};

	public ref class FirmwareUpdate : public GenericCommand
	{
	public:
		FirmwareUpdate() : GenericCommand(new VN::FirmwareUpdate()) {};
	};

	public ref class KnownMagneticDisturbance : public GenericCommand
	{
	public:
		enum class State : uint8_t { NotPresent = 0, Present = 1 } state;

		KnownMagneticDisturbance(State state) : GenericCommand(new VN::KnownMagneticDisturbance(static_cast<VN::KnownMagneticDisturbance::State>(state))) {};
	};

	inline VN::KnownMagneticDisturbance::State ToNativeInstance(KnownMagneticDisturbance::State state) { return static_cast<VN::KnownMagneticDisturbance::State>(static_cast<int>(state)); }

	public ref class KnownAccelerationDisturbance : public GenericCommand
	{
	public:
		enum class State : uint8_t { NotPresent = 0, Present = 1 } state;

		KnownAccelerationDisturbance(State state) : GenericCommand(new VN::KnownAccelerationDisturbance(static_cast<VN::KnownAccelerationDisturbance::State>(state))) {};
	};

	inline VN::KnownAccelerationDisturbance::State ToNativeInstance(KnownAccelerationDisturbance::State state) { return static_cast<VN::KnownAccelerationDisturbance::State>(static_cast<int>(state)); }

	public ref class SetInitialHeading : public GenericCommand
	{
	public:
		SetInitialHeading(float heading) : GenericCommand(new VN::SetInitialHeading(heading)) {};
	};

	public ref class AsyncOutputEnable : public GenericCommand
	{
	public:
		enum class State : uint8_t { Disable = 0, Enable = 1 } state;

		AsyncOutputEnable(State state) : GenericCommand(new VN::AsyncOutputEnable(static_cast<VN::AsyncOutputEnable::State>(state))) {};
	};

	inline VN::AsyncOutputEnable::State ToNativeInstance(AsyncOutputEnable::State state) { return static_cast<VN::AsyncOutputEnable::State>(static_cast<int>(state)); }

	public ref class SetFilterBias : public GenericCommand
	{
	public:
		SetFilterBias() : GenericCommand(new VN::SetFilterBias()) {};
	};

	public ref class PollBinaryOutputMessage : public GenericCommand
	{
	public:
		PollBinaryOutputMessage(uint8_t binMsgNum) : GenericCommand(new VN::PollBinaryOutputMessage(binMsgNum)) {};
	};
}

#endif  // VN_CLI_COMMANDS_HPP_
