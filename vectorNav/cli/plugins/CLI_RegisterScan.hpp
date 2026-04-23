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

#ifndef VN_CLI_REGISTERSCAN_HPP_
#define VN_CLI_REGISTERSCAN_HPP_

#include "vectornav/CLI_Sensor.hpp"

#pragma managed(push, off)
#include "vectornav/RegisterScan.hpp"
#pragma managed(pop)

using namespace System;

namespace VNSDK
{

public ref class RegisterScan
{
public:
    literal System::UInt16 NUM_REG = 256;

    value struct SaveConfigurationFilter
    {
    public:
        enum class Type
        {
            Include,
            Exclude
        };
        Type type;
        List<System::Byte>^ list;

        SaveConfigurationFilter(Type typ, List<System::Byte>^ data)
        {
            type = typ;
            list = gcnew List<System::Byte>();
            list->AddRange(data);
        }

        VN::RegisterScan::SaveConfigurationFilter GetNativeInstance()
        {
            VN::RegisterScan::SaveConfigurationFilter filter;
            filter.type = static_cast<VN::RegisterScan::SaveConfigurationFilter::Type>(static_cast<int>(type));
            for (int i = 0; i< list->Count; i++) { filter.list.push_back(list[i]); }
            return filter;
        }
    };

    ref class XmlConfigReader
    {
    private:
        VN::RegisterScan::XmlConfigReader* _nativeConfigReader;

    public:
        XmlConfigReader(System::String^ filePath)
        {
            std::string nativeStr = msclr::interop::marshal_as<std::string>(filePath);
            VN::Filesystem::FilePath nativePath(nativeStr);
            _nativeConfigReader = new VN::RegisterScan::XmlConfigReader(nativePath);
        }

        ~XmlConfigReader() { delete _nativeConfigReader; }

        !XmlConfigReader()
        {
            if (_nativeConfigReader != nullptr)
            {
                delete _nativeConfigReader;
                _nativeConfigReader = nullptr;
            }
        }

        VN::RegisterScan::XmlConfigReader& GetNativeInstance() { return *_nativeConfigReader; };
    };

    ref class AsciiConfigReader
    {
    private:
        VN::RegisterScan::AsciiConfigReader* _nativeConfigReader;

    public:
        AsciiConfigReader(System::String^ filePath)
        {
            std::string nativeStr = msclr::interop::marshal_as<std::string>(filePath);
            VN::Filesystem::FilePath nativePath(nativeStr);
            _nativeConfigReader = new VN::RegisterScan::AsciiConfigReader(nativePath);
        }

        ~AsciiConfigReader() { delete _nativeConfigReader; }

        !AsciiConfigReader()
        {
            if (_nativeConfigReader != nullptr)
            {
                delete _nativeConfigReader;
                _nativeConfigReader = nullptr;
            }
        }

        VN::RegisterScan::AsciiConfigReader& GetNativeInstance() { return *_nativeConfigReader; };
    };

    ref class XmlConfigWriter
    {
    private:
        VN::RegisterScan::XmlConfigWriter* _nativeConfigWriter;

    public:
        XmlConfigWriter(System::String^ filePath, Sensor^ sensor)
        {
            std::string nativeStr = msclr::interop::marshal_as<std::string>(filePath);
            VN::Filesystem::FilePath nativePath(nativeStr);
            _nativeConfigWriter = new VN::RegisterScan::XmlConfigWriter(sensor->GetNativeInstance(), nativePath);
        }

        ~XmlConfigWriter() { delete _nativeConfigWriter; }

        !XmlConfigWriter()
        {
            if (_nativeConfigWriter != nullptr)
            {
                delete _nativeConfigWriter;
                _nativeConfigWriter = nullptr;
            }
        }

        VN::RegisterScan::XmlConfigWriter& GetNativeInstance() { return *_nativeConfigWriter; };
    };

    ref class AsciiConfigWriter
    {
    private:
        VN::RegisterScan::AsciiConfigWriter* _nativeConfigWriter;

    public:
        AsciiConfigWriter(System::String^ filePath)
        {
            std::string nativeStr = msclr::interop::marshal_as<std::string>(filePath);
            VN::Filesystem::FilePath nativePath(nativeStr);
            _nativeConfigWriter = new VN::RegisterScan::AsciiConfigWriter(nativePath);
        }

        ~AsciiConfigWriter() { delete _nativeConfigWriter; }

        !AsciiConfigWriter()
        {
            if (_nativeConfigWriter != nullptr)
            {
                delete _nativeConfigWriter;
                _nativeConfigWriter = nullptr;
            }
        }

            VN::RegisterScan::AsciiConfigWriter &GetNativeInstance()
            {
                return *_nativeConfigWriter;
            };
    };

    static List<System::Byte> ^
        GetDefaultConfigRegisters() {
            VN::Vector<uint8_t, NUM_REG> nativeVec = VN::RegisterScan::getDefaultConfigRegisters();
            List<System::Byte> ^ managedArr = gcnew List<System::Byte>();
            for (size_t i = 0; i < nativeVec.size(); ++i) { managedArr->Add(nativeVec[i]); }
            return managedArr;
        }

    static void SetConfigurationRegisters(Sensor ^ sensor, XmlConfigReader ^ configReader)
    {
        VN::Error error = VN::RegisterScan::setConfigurationRegisters(sensor->GetNativeInstance(), configReader->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SetConfigurationRegisters(Sensor^ sensor, AsciiConfigReader^ configReader)
    {
        VN::Error error = VN::RegisterScan::setConfigurationRegisters(sensor->GetNativeInstance(), configReader->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void LoadConfiguration(Sensor^ sensor, XmlConfigReader^ configReader)
    {
        VN::Error error = VN::RegisterScan::loadConfiguration(sensor->GetNativeInstance(), configReader->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void LoadConfiguration(Sensor^ sensor, AsciiConfigReader^ configReader)
    {
        VN::Error error = VN::RegisterScan::loadConfiguration(sensor->GetNativeInstance(), configReader->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveConfiguration(Sensor^ sensor, XmlConfigWriter^ configWriter)
    {
        VN::RegisterScan::SaveConfigurationFilter filter = 
            VN::RegisterScan::SaveConfigurationFilter{VN::RegisterScan::SaveConfigurationFilter::Type::Include, VN::RegisterScan::getDefaultConfigRegisters()};
        VN::Error error = VN::RegisterScan::saveConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance(), filter);
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveConfiguration(Sensor^ sensor, XmlConfigWriter^ configWriter, SaveConfigurationFilter^ filter)
    {
        VN::Error error = VN::RegisterScan::saveConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance(), filter->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveConfiguration(Sensor^ sensor, AsciiConfigWriter^ configWriter)
    {
        VN::RegisterScan::SaveConfigurationFilter filter = 
            VN::RegisterScan::SaveConfigurationFilter{VN::RegisterScan::SaveConfigurationFilter::Type::Include, VN::RegisterScan::getDefaultConfigRegisters()};
        VN::Error error = VN::RegisterScan::saveConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance(), filter);
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveConfiguration(Sensor^ sensor, AsciiConfigWriter^ configWriter, SaveConfigurationFilter^ filter)
    {
        VN::Error error = VN::RegisterScan::saveConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance(), filter->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveNonDefaultConfiguration(Sensor^ sensor, XmlConfigWriter^ configWriter)
    {
        VN::Error error = VN::RegisterScan::saveNonDefaultConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }

    static void SaveNonDefaultConfiguration(Sensor^ sensor, AsciiConfigWriter^ configWriter)
    {
        VN::Error error = VN::RegisterScan::saveNonDefaultConfiguration(sensor->GetNativeInstance(), configWriter->GetNativeInstance());
        if (error != VN::Error::None)
        {
            throw gcnew VnException(error);
        }       
    }
};
} // namespace VNSDK

#endif  // VN_CLI_REGISTERSCAN_HPP_
