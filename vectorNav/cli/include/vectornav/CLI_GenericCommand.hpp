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

#ifndef VN_CLI_GENERICCOMMAND_HPP_
#define VN_CLI_GENERICCOMMAND_HPP_

#include <msclr/marshal.h>

#pragma managed(push, off)
#include "vectornav/Interface/GenericCommand.hpp"
#pragma managed(pop)

using namespace System;
using namespace msclr::interop;

namespace VNSDK
{
	public ref class GenericCommand
	{
	protected:
		VN::GenericCommand* _cmd;

	public:
		GenericCommand(VN::GenericCommand* cmd) : _cmd(cmd) {};
		~GenericCommand() { delete _cmd; };
		VN::GenericCommand* GetReference() { return _cmd; };

		bool IsAwaitingResponse()  // The command queue has NOT yet popped this command
		{
			return _cmd->isAwaitingResponse();
		};

		bool HasValidResponse()    // This command has received a response that matches.
		{
			return _cmd->hasValidResponse();
		};

		Nullable<int> GetError()
		{
			std::optional<VN::Error> error = _cmd->getError();
			if (error.has_value()) { return Nullable<int>(static_cast<int>(error.value())); }
			else { return Nullable<int>(); }
		};

		String^ GetResponse()
		{
			marshal_context^ context = gcnew marshal_context();
			String^ value = context->marshal_as<String^>(_cmd->getResponse().c_str());
			delete context;
			return value;

		};

	};
}

#endif  // VN_CLI_GENERICCOMMAND_HPP_
