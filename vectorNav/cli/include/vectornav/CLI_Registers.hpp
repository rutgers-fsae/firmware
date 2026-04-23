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

#ifndef VN_CLI_REGISTERS_HPP_
#define VN_CLI_REGISTERS_HPP_

#define NOMINMAX 1

#include "CLI_GenericCommand.hpp"

#include <msclr/marshal.h>

#pragma managed(push, off)
#include "vectornav/Interface/Registers.hpp"
#pragma managed(pop)

using namespace System;
using namespace msclr::interop;

namespace VNSDK
{
namespace Registers
{
public ref class ConfigRegister
{
    protected:
    VN::Register* _reg;
    
    public:
    ConfigRegister(VN::Register* reg) : _reg(reg) {};
    ~ConfigRegister() { delete _reg; };
    VN::Register* GetReference() { return _reg; };
    
    VNSDK::GenericCommand^ ToReadCommand()
    {
        return gcnew VNSDK::GenericCommand(&_reg->toReadCommand());
    };
    
    VNSDK::GenericCommand^ ToWriteCommand()
    {
        VN::ConfigurationRegister* configReg = dynamic_cast<VN::ConfigurationRegister*>(_reg);
        if (configReg != nullptr)
        {
            std::optional<VN::GenericCommand> writeCmd =  configReg->toWriteCommand();
            if (!writeCmd.has_value()) {
                return nullptr;;
            }
            return gcnew VNSDK::GenericCommand(new VN::GenericCommand(*writeCmd));
        }
        else
        {
            return nullptr;
        }
    };
};

public ref class MeasRegister
{
    protected:
    VN::Register* _reg;
    
    public:
    MeasRegister(VN::Register* reg) : _reg(reg) {};
    ~MeasRegister() { delete _reg; };
    VN::Register* GetReference() { return _reg; };
    
    VNSDK::GenericCommand^ ToReadCommand()
    {
        VN::GenericCommand* cmd_c = new VN::GenericCommand(_reg->toReadCommand());
        VNSDK::GenericCommand^ cmd = gcnew VNSDK::GenericCommand(cmd_c);
        return cmd;
    };
};


namespace Attitude
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 21 - Magnetic and Gravity Reference Vectors </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class MagGravRefVec : public ConfigRegister
{
    public:
    MagGravRefVec() : ConfigRegister(new VN::Registers::Attitude::MagGravRefVec())
    {
    };
    
    property Nullable<float> magRefN
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefN.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefN.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefN = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magRefE
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefE.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefE.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefE = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magRefD
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefD.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefD.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->magRefD = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gravRefN
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefN.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefN.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefN = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gravRefE
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefE.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefE.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefE = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gravRefD
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefD.has_value())
            {
                return ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefD.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::MagGravRefVec *)_reg)->gravRefD = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 35 - VPE Basic Control </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class VpeBasicControl : public ConfigRegister
{
    public:
    VpeBasicControl() : ConfigRegister(new VN::Registers::Attitude::VpeBasicControl())
    {
    };
    
    property Nullable<uint8_t> resv
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::Attitude::VpeBasicControl *)_reg)->resv.has_value())
            {
                return ((VN::Registers::Attitude::VpeBasicControl *)_reg)->resv.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeBasicControl *)_reg)->resv = value.Value;
            }
        };
        
    };
    
    enum class HeadingMode : uint8_t
    {
        Absolute = 0,
        Relative = 1,
        Indoor = 2,
    };
    property HeadingMode headingMode
    {
        HeadingMode get()
        {
            return static_cast<VpeBasicControl::HeadingMode>(((VN::Registers::Attitude::VpeBasicControl *)_reg)->headingMode.value());
        }
        
        void set(HeadingMode value)
        {
            ((VN::Registers::Attitude::VpeBasicControl *)_reg)->headingMode = static_cast<VN::Registers::Attitude::VpeBasicControl::HeadingMode>(value);
        }
        
    };
    
    enum class FilteringMode : uint8_t
    {
        Unfiltered = 0,
        AdaptivelyFiltered = 1,
    };
    property FilteringMode filteringMode
    {
        FilteringMode get()
        {
            return static_cast<VpeBasicControl::FilteringMode>(((VN::Registers::Attitude::VpeBasicControl *)_reg)->filteringMode.value());
        }
        
        void set(FilteringMode value)
        {
            ((VN::Registers::Attitude::VpeBasicControl *)_reg)->filteringMode = static_cast<VN::Registers::Attitude::VpeBasicControl::FilteringMode>(value);
        }
        
    };
    
    enum class TuningMode : uint8_t
    {
        Static = 0,
        Adaptive = 1,
    };
    property TuningMode tuningMode
    {
        TuningMode get()
        {
            return static_cast<VpeBasicControl::TuningMode>(((VN::Registers::Attitude::VpeBasicControl *)_reg)->tuningMode.value());
        }
        
        void set(TuningMode value)
        {
            ((VN::Registers::Attitude::VpeBasicControl *)_reg)->tuningMode = static_cast<VN::Registers::Attitude::VpeBasicControl::TuningMode>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 36 - VPE Magnetometer Basic Tuning </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class VpeMagBasicTuning : public ConfigRegister
{
    public:
    VpeMagBasicTuning() : ConfigRegister(new VN::Registers::Attitude::VpeMagBasicTuning())
    {
    };
    
    property Nullable<float> baseTuningX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningX.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> baseTuningY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningY.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> baseTuningZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->baseTuningZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningX.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningY.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveTuningZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringX.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringY.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeMagBasicTuning *)_reg)->adaptiveFilteringZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 38 - VPE Accelerometer Basic Tuning </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class VpeAccelBasicTuning : public ConfigRegister
{
    public:
    VpeAccelBasicTuning() : ConfigRegister(new VN::Registers::Attitude::VpeAccelBasicTuning())
    {
    };
    
    property Nullable<float> baseTuningX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningX.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> baseTuningY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningY.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> baseTuningZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->baseTuningZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningX.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningY.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveTuningZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveTuningZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringX.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringY.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> adaptiveFilteringZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringZ.has_value())
            {
                return ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Attitude::VpeAccelBasicTuning *)_reg)->adaptiveFilteringZ = value.Value;
            }
        };
        
    };
    
    
};

} // namespace Attitude

namespace GNSS
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 55 - GNSS Basic Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssBasicConfig : public ConfigRegister
{
    public:
    GnssBasicConfig() : ConfigRegister(new VN::Registers::GNSS::GnssBasicConfig())
    {
    };
    
    enum class ReceiverEnable : uint8_t
    {
        Internal = 0,
        VnWrite = 1,
        VnAdor = 2,
        GnssA = 100,
        VnWriteAndGnssA = 101,
        VnAdorAndGnssA = 102,
        VnWriteAndGnssAB = 201,
        VnAdorAndGnssAB = 202,
    };
    property ReceiverEnable receiverEnable
    {
        ReceiverEnable get()
        {
            return static_cast<GnssBasicConfig::ReceiverEnable>(((VN::Registers::GNSS::GnssBasicConfig *)_reg)->receiverEnable.value());
        }
        
        void set(ReceiverEnable value)
        {
            ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->receiverEnable = static_cast<VN::Registers::GNSS::GnssBasicConfig::ReceiverEnable>(value);
        }
        
    };
    
    enum class PpsSource : uint8_t
    {
        PpsPinRising = 0,
        PpsPinFalling = 1,
        SyncInRising = 2,
        SyncInFalling = 3,
        None = 6,
    };
    property PpsSource ppsSource
    {
        PpsSource get()
        {
            return static_cast<GnssBasicConfig::PpsSource>(((VN::Registers::GNSS::GnssBasicConfig *)_reg)->ppsSource.value());
        }
        
        void set(PpsSource value)
        {
            ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->ppsSource = static_cast<VN::Registers::GNSS::GnssBasicConfig::PpsSource>(value);
        }
        
    };
    
    enum class Rate : uint8_t
    {
        Rate1Hz = 1,
        Rate5Hz = 5,
    };
    property Rate rate
    {
        Rate get()
        {
            return static_cast<GnssBasicConfig::Rate>(((VN::Registers::GNSS::GnssBasicConfig *)_reg)->rate.value());
        }
        
        void set(Rate value)
        {
            ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->rate = static_cast<VN::Registers::GNSS::GnssBasicConfig::Rate>(value);
        }
        
    };
    
    property Nullable<uint8_t> resv4
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::GNSS::GnssBasicConfig *)_reg)->resv4.has_value())
            {
                return ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->resv4.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->resv4 = value.Value;
            }
        };
        
    };
    
    enum class AntPower : uint8_t
    {
        Off = 0,
        Internal = 1,
        External = 2,
    };
    property AntPower antPower
    {
        AntPower get()
        {
            return static_cast<GnssBasicConfig::AntPower>(((VN::Registers::GNSS::GnssBasicConfig *)_reg)->antPower.value());
        }
        
        void set(AntPower value)
        {
            ((VN::Registers::GNSS::GnssBasicConfig *)_reg)->antPower = static_cast<VN::Registers::GNSS::GnssBasicConfig::AntPower>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 57 - GNSS Internal A Antenna Offset </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssAOffset : public ConfigRegister
{
    public:
    GnssAOffset() : ConfigRegister(new VN::Registers::GNSS::GnssAOffset())
    {
    };
    
    property Nullable<float> positionX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::GnssAOffset *)_reg)->positionX.has_value())
            {
                return ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::GnssAOffset *)_reg)->positionY.has_value())
            {
                return ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::GnssAOffset *)_reg)->positionZ.has_value())
            {
                return ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssAOffset *)_reg)->positionZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 99 - GNSS System Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssSystemConfig : public ConfigRegister
{
    public:
    GnssSystemConfig() : ConfigRegister(new VN::Registers::GNSS::GnssSystemConfig())
    {
        systems = gcnew Systems(_reg);
        sbasMode = gcnew SbasMode(_reg);
        sbasSelect1 = gcnew SbasSelect1(_reg);
        sbasSelect2 = gcnew SbasSelect2(_reg);
        sbasSelect3 = gcnew SbasSelect3(_reg);
    };
    
    ref class Systems
    {
        VN::Register* _reg;
        
        public:
        Systems(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint16_t> _value      ///< BitField value for systems
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return uint16_t(systems.value()); }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems = uint16_t(value);
            }
        };
        
        // GPS
        property Nullable<uint16_t> gps
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().gps; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->gps = uint16_t(value);
            }
        };
        
        // SBAS
        property Nullable<uint16_t> sbas
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().sbas; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->sbas = uint16_t(value);
            }
        };
        
        // GLONASS
        property Nullable<uint16_t> glonass
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().glonass; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->glonass = uint16_t(value);
            }
        };
        
        // Beidou
        property Nullable<uint16_t> beidou
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().beidou; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->beidou = uint16_t(value);
            }
        };
        
        // Galileo
        property Nullable<uint16_t> galileo
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().galileo; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->galileo = uint16_t(value);
            }
        };
        
        // IMES
        property Nullable<uint16_t> imes
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().imes; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->imes = uint16_t(value);
            }
        };
        
        // QZSS_L1_CA
        property Nullable<uint16_t> qzssL1Ca
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().qzssL1Ca; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->qzssL1Ca = uint16_t(value);
            }
        };
        
        // QZSS_L1_SAIF
        property Nullable<uint16_t> qzssL1Saif
        {
            Nullable<uint16_t> get()
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (systems.has_value()) { return systems.value().qzssL1Saif; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto systems = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems;
                if (!systems.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->systems.emplace();
                }
                systems->qzssL1Saif = uint16_t(value);
            }
        };
        
    };
    
    Systems^ systems;
    
    property Nullable<uint8_t> minCno
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minCno.has_value())
            {
                return ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minCno.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minCno = value.Value;
            }
        };
        
    };
    
    property Nullable<uint8_t> minElev
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minElev.has_value())
            {
                return ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minElev.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->minElev = value.Value;
            }
        };
        
    };
    
    property Nullable<uint8_t> maxSats
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::GNSS::GnssSystemConfig *)_reg)->maxSats.has_value())
            {
                return ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->maxSats.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->maxSats = value.Value;
            }
        };
        
    };
    
    ref class SbasMode
    {
        VN::Register* _reg;
        
        public:
        SbasMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for sbasMode
        {
            Nullable<uint8_t> get()
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (sbasMode.has_value()) { return uint8_t(sbasMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (!sbasMode.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode.emplace();
                }
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode = uint8_t(value);
            }
        };
        
        // Ranging
        property Nullable<uint8_t> ranging
        {
            Nullable<uint8_t> get()
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (sbasMode.has_value()) { return sbasMode.value().ranging; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (!sbasMode.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode.emplace();
                }
                sbasMode->ranging = uint8_t(value);
            }
        };
        
        // DiffCorr
        property Nullable<uint8_t> diffCorr
        {
            Nullable<uint8_t> get()
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (sbasMode.has_value()) { return sbasMode.value().diffCorr; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (!sbasMode.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode.emplace();
                }
                sbasMode->diffCorr = uint8_t(value);
            }
        };
        
        // Integrity
        property Nullable<uint8_t> integrity
        {
            Nullable<uint8_t> get()
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (sbasMode.has_value()) { return sbasMode.value().integrity; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (!sbasMode.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode.emplace();
                }
                sbasMode->integrity = uint8_t(value);
            }
        };
        
        // TestMode
        property Nullable<uint8_t> testMode
        {
            Nullable<uint8_t> get()
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (sbasMode.has_value()) { return sbasMode.value().testMode; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto sbasMode = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode;
                if (!sbasMode.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasMode.emplace();
                }
                sbasMode->testMode = uint8_t(value);
            }
        };
        
    };
    
    SbasMode^ sbasMode;
    
    ref class SbasSelect1
    {
        VN::Register* _reg;
        
        public:
        SbasSelect1(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint16_t> _value      ///< BitField value for sbasSelect1
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return uint16_t(sbasSelect1.value()); }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1 = uint16_t(value);
            }
        };
        
        // Sbas120
        property Nullable<uint16_t> sbas120
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas120; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas120 = uint16_t(value);
            }
        };
        
        // Sbas121
        property Nullable<uint16_t> sbas121
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas121; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas121 = uint16_t(value);
            }
        };
        
        // Sbas122
        property Nullable<uint16_t> sbas122
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas122; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas122 = uint16_t(value);
            }
        };
        
        // Sbas123
        property Nullable<uint16_t> sbas123
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas123; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas123 = uint16_t(value);
            }
        };
        
        // Sbas124
        property Nullable<uint16_t> sbas124
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas124; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas124 = uint16_t(value);
            }
        };
        
        // Sbas125
        property Nullable<uint16_t> sbas125
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas125; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas125 = uint16_t(value);
            }
        };
        
        // Sbas126
        property Nullable<uint16_t> sbas126
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas126; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas126 = uint16_t(value);
            }
        };
        
        // Sbas127
        property Nullable<uint16_t> sbas127
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas127; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas127 = uint16_t(value);
            }
        };
        
        // Sbas128
        property Nullable<uint16_t> sbas128
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas128; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas128 = uint16_t(value);
            }
        };
        
        // Sbas129
        property Nullable<uint16_t> sbas129
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas129; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas129 = uint16_t(value);
            }
        };
        
        // Sbas130
        property Nullable<uint16_t> sbas130
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas130; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas130 = uint16_t(value);
            }
        };
        
        // Sbas131
        property Nullable<uint16_t> sbas131
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas131; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas131 = uint16_t(value);
            }
        };
        
        // Sbas132
        property Nullable<uint16_t> sbas132
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas132; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas132 = uint16_t(value);
            }
        };
        
        // Sbas133
        property Nullable<uint16_t> sbas133
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas133; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas133 = uint16_t(value);
            }
        };
        
        // Sbas134
        property Nullable<uint16_t> sbas134
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas134; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas134 = uint16_t(value);
            }
        };
        
        // Sbas135
        property Nullable<uint16_t> sbas135
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (sbasSelect1.has_value()) { return sbasSelect1.value().sbas135; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect1 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1;
                if (!sbasSelect1.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect1.emplace();
                }
                sbasSelect1->sbas135 = uint16_t(value);
            }
        };
        
    };
    
    SbasSelect1^ sbasSelect1;
    
    ref class SbasSelect2
    {
        VN::Register* _reg;
        
        public:
        SbasSelect2(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint16_t> _value      ///< BitField value for sbasSelect2
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return uint16_t(sbasSelect2.value()); }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2 = uint16_t(value);
            }
        };
        
        // Sbas136
        property Nullable<uint16_t> sbas136
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas136; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas136 = uint16_t(value);
            }
        };
        
        // Sbas137
        property Nullable<uint16_t> sbas137
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas137; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas137 = uint16_t(value);
            }
        };
        
        // Sbas138
        property Nullable<uint16_t> sbas138
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas138; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas138 = uint16_t(value);
            }
        };
        
        // Sbas139
        property Nullable<uint16_t> sbas139
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas139; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas139 = uint16_t(value);
            }
        };
        
        // Sbas140
        property Nullable<uint16_t> sbas140
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas140; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas140 = uint16_t(value);
            }
        };
        
        // Sbas141
        property Nullable<uint16_t> sbas141
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas141; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas141 = uint16_t(value);
            }
        };
        
        // Sbas142
        property Nullable<uint16_t> sbas142
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas142; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas142 = uint16_t(value);
            }
        };
        
        // Sbas143
        property Nullable<uint16_t> sbas143
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas143; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas143 = uint16_t(value);
            }
        };
        
        // Sbas144
        property Nullable<uint16_t> sbas144
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas144; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas144 = uint16_t(value);
            }
        };
        
        // Sbas145
        property Nullable<uint16_t> sbas145
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas145; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas145 = uint16_t(value);
            }
        };
        
        // Sbas146
        property Nullable<uint16_t> sbas146
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas146; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas146 = uint16_t(value);
            }
        };
        
        // Sbas147
        property Nullable<uint16_t> sbas147
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas147; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas147 = uint16_t(value);
            }
        };
        
        // Sbas148
        property Nullable<uint16_t> sbas148
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas148; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas148 = uint16_t(value);
            }
        };
        
        // Sbas149
        property Nullable<uint16_t> sbas149
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas149; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas149 = uint16_t(value);
            }
        };
        
        // Sbas150
        property Nullable<uint16_t> sbas150
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas150; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas150 = uint16_t(value);
            }
        };
        
        // Sbas151
        property Nullable<uint16_t> sbas151
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (sbasSelect2.has_value()) { return sbasSelect2.value().sbas151; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect2 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2;
                if (!sbasSelect2.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect2.emplace();
                }
                sbasSelect2->sbas151 = uint16_t(value);
            }
        };
        
    };
    
    SbasSelect2^ sbasSelect2;
    
    ref class SbasSelect3
    {
        VN::Register* _reg;
        
        public:
        SbasSelect3(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint16_t> _value      ///< BitField value for sbasSelect3
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return uint16_t(sbasSelect3.value()); }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3 = uint16_t(value);
            }
        };
        
        // Sbas152
        property Nullable<uint16_t> sbas152
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas152; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas152 = uint16_t(value);
            }
        };
        
        // Sbas153
        property Nullable<uint16_t> sbas153
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas153; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas153 = uint16_t(value);
            }
        };
        
        // Sbas154
        property Nullable<uint16_t> sbas154
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas154; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas154 = uint16_t(value);
            }
        };
        
        // Sbas155
        property Nullable<uint16_t> sbas155
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas155; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas155 = uint16_t(value);
            }
        };
        
        // Sbas156
        property Nullable<uint16_t> sbas156
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas156; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas156 = uint16_t(value);
            }
        };
        
        // Sbas157
        property Nullable<uint16_t> sbas157
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas157; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas157 = uint16_t(value);
            }
        };
        
        // Sbas158
        property Nullable<uint16_t> sbas158
        {
            Nullable<uint16_t> get()
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (sbasSelect3.has_value()) { return sbasSelect3.value().sbas158; }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto sbasSelect3 = ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3;
                if (!sbasSelect3.has_value())
                {
                    ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->sbasSelect3.emplace();
                }
                sbasSelect3->sbas158 = uint16_t(value);
            }
        };
        
    };
    
    SbasSelect3^ sbasSelect3;
    
    enum class ReceiverSelect : uint8_t
    {
        GnssAB = 0,
        GnssA = 1,
        GnssB = 2,
    };
    property ReceiverSelect receiverSelect
    {
        ReceiverSelect get()
        {
            return static_cast<GnssSystemConfig::ReceiverSelect>(((VN::Registers::GNSS::GnssSystemConfig *)_reg)->receiverSelect);
        }
        
        void set(ReceiverSelect value)
        {
            ((VN::Registers::GNSS::GnssSystemConfig *)_reg)->receiverSelect = static_cast<VN::Registers::GNSS::GnssSystemConfig::ReceiverSelect>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 100 - GNSS Sync Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssSyncConfig : public ConfigRegister
{
    public:
    GnssSyncConfig() : ConfigRegister(new VN::Registers::GNSS::GnssSyncConfig())
    {
    };
    
    enum class GnssSyncEnable : uint8_t
    {
        Off = 0,
        AlwaysOn = 1,
        OnWhenLocked = 2,
    };
    property GnssSyncEnable gnssSyncEnable
    {
        GnssSyncEnable get()
        {
            return static_cast<GnssSyncConfig::GnssSyncEnable>(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->gnssSyncEnable.value());
        }
        
        void set(GnssSyncEnable value)
        {
            ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->gnssSyncEnable = static_cast<VN::Registers::GNSS::GnssSyncConfig::GnssSyncEnable>(value);
        }
        
    };
    
    enum class Polarity : uint8_t
    {
        FallingEdge = 0,
        RisingEdge = 1,
    };
    property Polarity polarity
    {
        Polarity get()
        {
            return static_cast<GnssSyncConfig::Polarity>(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->polarity.value());
        }
        
        void set(Polarity value)
        {
            ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->polarity = static_cast<VN::Registers::GNSS::GnssSyncConfig::Polarity>(value);
        }
        
    };
    
    enum class SpecType : uint8_t
    {
        PeriodPulseWidth = 0,
        FreqDutyCycle = 1,
    };
    property SpecType specType
    {
        SpecType get()
        {
            return static_cast<GnssSyncConfig::SpecType>(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->specType.value());
        }
        
        void set(SpecType value)
        {
            ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->specType = static_cast<VN::Registers::GNSS::GnssSyncConfig::SpecType>(value);
        }
        
    };
    
    property Nullable<uint8_t> resv
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->resv.has_value())
            {
                return ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->resv.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->resv = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> period
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->period.has_value())
            {
                return ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->period.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->period = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> pulseWidth
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->pulseWidth.has_value())
            {
                return ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->pulseWidth.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->pulseWidth = value.Value;
            }
        };
        
    };
    
    property Nullable<int32_t> offset
    {
        Nullable<int32_t> get()
        {
            if(((VN::Registers::GNSS::GnssSyncConfig *)_reg)->offset.has_value())
            {
                return ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->offset.value();
            }
            else
            {
                return Nullable<int32_t>();
            }
        };
        
        void set(Nullable<int32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::GnssSyncConfig *)_reg)->offset = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 157 - External GNSS Antenna Offset </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class ExtGnssOffset : public ConfigRegister
{
    public:
    ExtGnssOffset() : ConfigRegister(new VN::Registers::GNSS::ExtGnssOffset())
    {
    };
    
    property Nullable<float> positionX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionX.has_value())
            {
                return ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionY.has_value())
            {
                return ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionZ.has_value())
            {
                return ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSS::ExtGnssOffset *)_reg)->positionZ = value.Value;
            }
        };
        
    };
    
    
};

} // namespace GNSS

namespace GNSSCompass
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 93 - GNSS Compass Antenna Baseline </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssCompassBaseline : public ConfigRegister
{
    public:
    GnssCompassBaseline() : ConfigRegister(new VN::Registers::GNSSCompass::GnssCompassBaseline())
    {
    };
    
    property Nullable<float> positionX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionX.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionY.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> positionZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionZ.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->positionZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> uncertaintyX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyX.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> uncertaintyY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyY.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> uncertaintyZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyZ.has_value())
            {
                return ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::GNSSCompass::GnssCompassBaseline *)_reg)->uncertaintyZ = value.Value;
            }
        };
        
    };
    
    
};

} // namespace GNSSCompass

namespace HardSoftIronEstimator
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 44 - Real-Time HSI Control </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class RealTimeHsiControl : public ConfigRegister
{
    public:
    RealTimeHsiControl() : ConfigRegister(new VN::Registers::HardSoftIronEstimator::RealTimeHsiControl())
    {
    };
    
    enum class Mode : uint8_t
    {
        Off = 0,
        Run = 1,
        Reset = 2,
    };
    property Mode mode
    {
        Mode get()
        {
            return static_cast<RealTimeHsiControl::Mode>(((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->mode.value());
        }
        
        void set(Mode value)
        {
            ((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->mode = static_cast<VN::Registers::HardSoftIronEstimator::RealTimeHsiControl::Mode>(value);
        }
        
    };
    
    enum class ApplyCompensation : uint8_t
    {
        Disable = 1,
        Enable = 3,
    };
    property ApplyCompensation applyCompensation
    {
        ApplyCompensation get()
        {
            return static_cast<RealTimeHsiControl::ApplyCompensation>(((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->applyCompensation.value());
        }
        
        void set(ApplyCompensation value)
        {
            ((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->applyCompensation = static_cast<VN::Registers::HardSoftIronEstimator::RealTimeHsiControl::ApplyCompensation>(value);
        }
        
    };
    
    property Nullable<uint8_t> convergeRate
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->convergeRate.has_value())
            {
                return ((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->convergeRate.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::HardSoftIronEstimator::RealTimeHsiControl *)_reg)->convergeRate = value.Value;
            }
        };
        
    };
    
    
};

} // namespace HardSoftIronEstimator

namespace Heave
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 116 - Heave Basic Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class HeaveBasicConfig : public ConfigRegister
{
    public:
    HeaveBasicConfig() : ConfigRegister(new VN::Registers::Heave::HeaveBasicConfig())
    {
    };
    
    property Nullable<float> initialWavePeriod
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWavePeriod.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWavePeriod.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWavePeriod = value.Value;
            }
        };
        
    };
    
    property Nullable<float> initialWaveAmplitude
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWaveAmplitude.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWaveAmplitude.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->initialWaveAmplitude = value.Value;
            }
        };
        
    };
    
    property Nullable<float> maxWavePeriod
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->maxWavePeriod.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->maxWavePeriod.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->maxWavePeriod = value.Value;
            }
        };
        
    };
    
    property Nullable<float> minWaveAmplitude
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->minWaveAmplitude.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->minWaveAmplitude.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->minWaveAmplitude = value.Value;
            }
        };
        
    };
    
    property Nullable<float> delayedHeaveCutoffFreq
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->delayedHeaveCutoffFreq.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->delayedHeaveCutoffFreq.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->delayedHeaveCutoffFreq = value.Value;
            }
        };
        
    };
    
    property Nullable<float> heaveCutoffFreq
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveCutoffFreq.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveCutoffFreq.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveCutoffFreq = value.Value;
            }
        };
        
    };
    
    property Nullable<float> heaveRateCutoffFreq
    {
        Nullable<float> get()
        {
            if(((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveRateCutoffFreq.has_value())
            {
                return ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveRateCutoffFreq.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::Heave::HeaveBasicConfig *)_reg)->heaveRateCutoffFreq = value.Value;
            }
        };
        
    };
    
    
};

} // namespace Heave

namespace IMU
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 23 - Magnetometer Calibration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class MagCal : public ConfigRegister
{
    public:
    MagCal() : ConfigRegister(new VN::Registers::IMU::MagCal())
    {
    };
    
    property Nullable<float> magGain00
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain00.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain00.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain00 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain01
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain01.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain01.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain01 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain02
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain02.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain02.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain02 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain10
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain10.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain10.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain10 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain11
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain11.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain11.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain11 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain12
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain12.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain12.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain12 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain20
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain20.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain20.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain20 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain21
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain21.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain21.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain21 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magGain22
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magGain22.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magGain22.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magGain22 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magBiasX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magBiasX.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magBiasX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magBiasX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magBiasY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magBiasY.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magBiasY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magBiasY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> magBiasZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::MagCal *)_reg)->magBiasZ.has_value())
            {
                return ((VN::Registers::IMU::MagCal *)_reg)->magBiasZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::MagCal *)_reg)->magBiasZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 25 - Accel Calibration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class AccelCal : public ConfigRegister
{
    public:
    AccelCal() : ConfigRegister(new VN::Registers::IMU::AccelCal())
    {
    };
    
    property Nullable<float> accelGain00
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain00.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain00.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain00 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain01
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain01.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain01.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain01 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain02
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain02.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain02.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain02 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain10
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain10.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain10.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain10 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain11
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain11.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain11.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain11 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain12
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain12.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain12.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain12 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain20
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain20.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain20.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain20 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain21
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain21.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain21.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain21 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelGain22
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelGain22.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelGain22.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelGain22 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelBiasX.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelBiasY.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::AccelCal *)_reg)->accelBiasZ.has_value())
            {
                return ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::AccelCal *)_reg)->accelBiasZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 26 - Reference Frame Rotation </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class RefFrameRot : public ConfigRegister
{
    public:
    RefFrameRot() : ConfigRegister(new VN::Registers::IMU::RefFrameRot())
    {
    };
    
    property Nullable<float> rfr00
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr00.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr00.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr00 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr01
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr01.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr01.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr01 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr02
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr02.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr02.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr02 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr10
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr10.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr10.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr10 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr11
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr11.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr11.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr11 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr12
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr12.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr12.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr12 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr20
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr20.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr20.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr20 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr21
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr21.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr21.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr21 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> rfr22
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::RefFrameRot *)_reg)->rfr22.has_value())
            {
                return ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr22.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::RefFrameRot *)_reg)->rfr22 = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 82 - Delta Theta and Delta Velocity Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class DeltaThetaVelConfig : public ConfigRegister
{
    public:
    DeltaThetaVelConfig() : ConfigRegister(new VN::Registers::IMU::DeltaThetaVelConfig())
    {
    };
    
    enum class IntegrationFrame : uint8_t
    {
        Body = 0,
        NED = 1,
    };
    property IntegrationFrame integrationFrame
    {
        IntegrationFrame get()
        {
            return static_cast<DeltaThetaVelConfig::IntegrationFrame>(((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->integrationFrame.value());
        }
        
        void set(IntegrationFrame value)
        {
            ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->integrationFrame = static_cast<VN::Registers::IMU::DeltaThetaVelConfig::IntegrationFrame>(value);
        }
        
    };
    
    enum class GyroCompensation : uint8_t
    {
        None = 0,
        Bias = 1,
    };
    property GyroCompensation gyroCompensation
    {
        GyroCompensation get()
        {
            return static_cast<DeltaThetaVelConfig::GyroCompensation>(((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->gyroCompensation.value());
        }
        
        void set(GyroCompensation value)
        {
            ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->gyroCompensation = static_cast<VN::Registers::IMU::DeltaThetaVelConfig::GyroCompensation>(value);
        }
        
    };
    
    enum class AccelCompensation : uint8_t
    {
        None = 0,
        Gravity = 1,
        Bias = 2,
        BiasAndGravity = 3,
    };
    property AccelCompensation accelCompensation
    {
        AccelCompensation get()
        {
            return static_cast<DeltaThetaVelConfig::AccelCompensation>(((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->accelCompensation.value());
        }
        
        void set(AccelCompensation value)
        {
            ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->accelCompensation = static_cast<VN::Registers::IMU::DeltaThetaVelConfig::AccelCompensation>(value);
        }
        
    };
    
    enum class EarthRateCompensation : uint8_t
    {
        None = 0,
        GyroRate = 1,
        CoriolisAccel = 2,
        RateAndCoriolis = 3,
    };
    property EarthRateCompensation earthRateCompensation
    {
        EarthRateCompensation get()
        {
            return static_cast<DeltaThetaVelConfig::EarthRateCompensation>(((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->earthRateCompensation.value());
        }
        
        void set(EarthRateCompensation value)
        {
            ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->earthRateCompensation = static_cast<VN::Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation>(value);
        }
        
    };
    
    property Nullable<uint16_t> resv
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->resv.has_value())
            {
                return ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->resv.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::DeltaThetaVelConfig *)_reg)->resv = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 84 - Gyro Calibration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GyroCal : public ConfigRegister
{
    public:
    GyroCal() : ConfigRegister(new VN::Registers::IMU::GyroCal())
    {
    };
    
    property Nullable<float> gyroGain00
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain00.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain00.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain00 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain01
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain01.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain01.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain01 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain02
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain02.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain02.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain02 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain10
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain10.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain10.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain10 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain11
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain11.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain11.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain11 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain12
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain12.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain12.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain12 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain20
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain20.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain20.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain20 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain21
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain21.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain21.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain21 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroGain22
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroGain22.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain22.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroGain22 = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroBiasX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasX.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroBiasY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasY.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroBiasZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasZ.has_value())
            {
                return ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::GyroCal *)_reg)->gyroBiasZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 85 - IMU Filtering Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class ImuFilterControl : public ConfigRegister
{
    public:
    ImuFilterControl() : ConfigRegister(new VN::Registers::IMU::ImuFilterControl())
    {
        magFilterMode = gcnew MagFilterMode(_reg);
        accelFilterMode = gcnew AccelFilterMode(_reg);
        gyroFilterMode = gcnew GyroFilterMode(_reg);
        tempFilterMode = gcnew TempFilterMode(_reg);
        presFilterMode = gcnew PresFilterMode(_reg);
    };
    
    property Nullable<uint16_t> magWindowSize
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::ImuFilterControl *)_reg)->magWindowSize.has_value())
            {
                return ((VN::Registers::IMU::ImuFilterControl *)_reg)->magWindowSize.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->magWindowSize = value.Value;
            }
        };
        
    };
    
    property Nullable<uint16_t> accelWindowSize
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::ImuFilterControl *)_reg)->accelWindowSize.has_value())
            {
                return ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelWindowSize.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelWindowSize = value.Value;
            }
        };
        
    };
    
    property Nullable<uint16_t> gyroWindowSize
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroWindowSize.has_value())
            {
                return ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroWindowSize.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroWindowSize = value.Value;
            }
        };
        
    };
    
    property Nullable<uint16_t> tempWindowSize
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::ImuFilterControl *)_reg)->tempWindowSize.has_value())
            {
                return ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempWindowSize.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempWindowSize = value.Value;
            }
        };
        
    };
    
    property Nullable<uint16_t> presWindowSize
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::IMU::ImuFilterControl *)_reg)->presWindowSize.has_value())
            {
                return ((VN::Registers::IMU::ImuFilterControl *)_reg)->presWindowSize.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->presWindowSize = value.Value;
            }
        };
        
    };
    
    ref class MagFilterMode
    {
        VN::Register* _reg;
        
        public:
        MagFilterMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for magFilterMode
        {
            Nullable<uint8_t> get()
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (magFilterMode.has_value()) { return uint8_t(magFilterMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (!magFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode.emplace();
                }
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode = uint8_t(value);
            }
        };
        
        // Uncomp
        property Nullable<uint8_t> uncomp
        {
            Nullable<uint8_t> get()
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (magFilterMode.has_value()) { return magFilterMode.value().uncomp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (!magFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode.emplace();
                }
                magFilterMode->uncomp = uint8_t(value);
            }
        };
        
        // Comp
        property Nullable<uint8_t> comp
        {
            Nullable<uint8_t> get()
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (magFilterMode.has_value()) { return magFilterMode.value().comp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto magFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode;
                if (!magFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->magFilterMode.emplace();
                }
                magFilterMode->comp = uint8_t(value);
            }
        };
        
    };
    
    MagFilterMode^ magFilterMode;
    
    ref class AccelFilterMode
    {
        VN::Register* _reg;
        
        public:
        AccelFilterMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for accelFilterMode
        {
            Nullable<uint8_t> get()
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (accelFilterMode.has_value()) { return uint8_t(accelFilterMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (!accelFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode.emplace();
                }
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode = uint8_t(value);
            }
        };
        
        // Uncomp
        property Nullable<uint8_t> uncomp
        {
            Nullable<uint8_t> get()
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (accelFilterMode.has_value()) { return accelFilterMode.value().uncomp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (!accelFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode.emplace();
                }
                accelFilterMode->uncomp = uint8_t(value);
            }
        };
        
        // Comp
        property Nullable<uint8_t> comp
        {
            Nullable<uint8_t> get()
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (accelFilterMode.has_value()) { return accelFilterMode.value().comp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto accelFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode;
                if (!accelFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->accelFilterMode.emplace();
                }
                accelFilterMode->comp = uint8_t(value);
            }
        };
        
    };
    
    AccelFilterMode^ accelFilterMode;
    
    ref class GyroFilterMode
    {
        VN::Register* _reg;
        
        public:
        GyroFilterMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for gyroFilterMode
        {
            Nullable<uint8_t> get()
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (gyroFilterMode.has_value()) { return uint8_t(gyroFilterMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (!gyroFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode.emplace();
                }
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode = uint8_t(value);
            }
        };
        
        // Uncomp
        property Nullable<uint8_t> uncomp
        {
            Nullable<uint8_t> get()
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (gyroFilterMode.has_value()) { return gyroFilterMode.value().uncomp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (!gyroFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode.emplace();
                }
                gyroFilterMode->uncomp = uint8_t(value);
            }
        };
        
        // Comp
        property Nullable<uint8_t> comp
        {
            Nullable<uint8_t> get()
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (gyroFilterMode.has_value()) { return gyroFilterMode.value().comp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gyroFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode;
                if (!gyroFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->gyroFilterMode.emplace();
                }
                gyroFilterMode->comp = uint8_t(value);
            }
        };
        
    };
    
    GyroFilterMode^ gyroFilterMode;
    
    ref class TempFilterMode
    {
        VN::Register* _reg;
        
        public:
        TempFilterMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for tempFilterMode
        {
            Nullable<uint8_t> get()
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (tempFilterMode.has_value()) { return uint8_t(tempFilterMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (!tempFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode.emplace();
                }
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode = uint8_t(value);
            }
        };
        
        // Uncomp
        property Nullable<uint8_t> uncomp
        {
            Nullable<uint8_t> get()
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (tempFilterMode.has_value()) { return tempFilterMode.value().uncomp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (!tempFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode.emplace();
                }
                tempFilterMode->uncomp = uint8_t(value);
            }
        };
        
        // Comp
        property Nullable<uint8_t> comp
        {
            Nullable<uint8_t> get()
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (tempFilterMode.has_value()) { return tempFilterMode.value().comp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto tempFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode;
                if (!tempFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->tempFilterMode.emplace();
                }
                tempFilterMode->comp = uint8_t(value);
            }
        };
        
    };
    
    TempFilterMode^ tempFilterMode;
    
    ref class PresFilterMode
    {
        VN::Register* _reg;
        
        public:
        PresFilterMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for presFilterMode
        {
            Nullable<uint8_t> get()
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (presFilterMode.has_value()) { return uint8_t(presFilterMode.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (!presFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode.emplace();
                }
                ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode = uint8_t(value);
            }
        };
        
        // Uncomp
        property Nullable<uint8_t> uncomp
        {
            Nullable<uint8_t> get()
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (presFilterMode.has_value()) { return presFilterMode.value().uncomp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (!presFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode.emplace();
                }
                presFilterMode->uncomp = uint8_t(value);
            }
        };
        
        // Comp
        property Nullable<uint8_t> comp
        {
            Nullable<uint8_t> get()
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (presFilterMode.has_value()) { return presFilterMode.value().comp; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto presFilterMode = ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode;
                if (!presFilterMode.has_value())
                {
                    ((VN::Registers::IMU::ImuFilterControl *)_reg)->presFilterMode.emplace();
                }
                presFilterMode->comp = uint8_t(value);
            }
        };
        
    };
    
    PresFilterMode^ presFilterMode;
    
    
};

} // namespace IMU

namespace INS
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 67 - INS Basic Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsBasicConfig : public ConfigRegister
{
    public:
    InsBasicConfig() : ConfigRegister(new VN::Registers::INS::InsBasicConfig())
    {
    };
    
    enum class Scenario : uint8_t
    {
        Ahrs = 0,
        GnssInsWithPressure = 1,
        GnssInsNoPressure = 2,
        DualGnssNoPressure = 3,
        DualGnssWithPressure = 5,
    };
    property Scenario scenario
    {
        Scenario get()
        {
            return static_cast<InsBasicConfig::Scenario>(((VN::Registers::INS::InsBasicConfig *)_reg)->scenario.value());
        }
        
        void set(Scenario value)
        {
            ((VN::Registers::INS::InsBasicConfig *)_reg)->scenario = static_cast<VN::Registers::INS::InsBasicConfig::Scenario>(value);
        }
        
    };
    
    enum class AhrsAiding : uint8_t
    {
        Disable = 0,
        Enable = 1,
    };
    property AhrsAiding ahrsAiding
    {
        AhrsAiding get()
        {
            return static_cast<InsBasicConfig::AhrsAiding>(((VN::Registers::INS::InsBasicConfig *)_reg)->ahrsAiding.value());
        }
        
        void set(AhrsAiding value)
        {
            ((VN::Registers::INS::InsBasicConfig *)_reg)->ahrsAiding = static_cast<VN::Registers::INS::InsBasicConfig::AhrsAiding>(value);
        }
        
    };
    
    enum class EstBaseline : uint8_t
    {
        Disable = 0,
        Enable = 1,
    };
    property EstBaseline estBaseline
    {
        EstBaseline get()
        {
            return static_cast<InsBasicConfig::EstBaseline>(((VN::Registers::INS::InsBasicConfig *)_reg)->estBaseline.value());
        }
        
        void set(EstBaseline value)
        {
            ((VN::Registers::INS::InsBasicConfig *)_reg)->estBaseline = static_cast<VN::Registers::INS::InsBasicConfig::EstBaseline>(value);
        }
        
    };
    
    property Nullable<uint8_t> resv
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::INS::InsBasicConfig *)_reg)->resv.has_value())
            {
                return ((VN::Registers::INS::InsBasicConfig *)_reg)->resv.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsBasicConfig *)_reg)->resv = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 74 - Filter Startup Bias </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class FilterStartupBias : public ConfigRegister
{
    public:
    FilterStartupBias() : ConfigRegister(new VN::Registers::INS::FilterStartupBias())
    {
    };
    
    property Nullable<float> gyroBiasX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasX.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroBiasY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasY.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> gyroBiasZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasZ.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->gyroBiasZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasX.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasY.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> accelBiasZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasZ.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->accelBiasZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> presBias
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::FilterStartupBias *)_reg)->presBias.has_value())
            {
                return ((VN::Registers::INS::FilterStartupBias *)_reg)->presBias.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::FilterStartupBias *)_reg)->presBias = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 105 - INS Reference Point Offset </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsRefOffset : public ConfigRegister
{
    public:
    InsRefOffset() : ConfigRegister(new VN::Registers::INS::InsRefOffset())
    {
    };
    
    property Nullable<float> refOffsetX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetX.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> refOffsetY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetY.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> refOffsetZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetZ.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refOffsetZ = value.Value;
            }
        };
        
    };
    
    property Nullable<float> refUncertX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refUncertX.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> refUncertY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refUncertY.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> refUncertZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::INS::InsRefOffset *)_reg)->refUncertZ.has_value())
            {
                return ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsRefOffset *)_reg)->refUncertZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 144 - INS GNSS Select </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsGnssSelect : public ConfigRegister
{
    public:
    InsGnssSelect() : ConfigRegister(new VN::Registers::INS::InsGnssSelect())
    {
    };
    
    enum class ActiveReceiverSelect : uint8_t
    {
        None = 0,
        PrimaryReceiver = 1,
        SecondaryReceiver = 2,
        TertiaryReceiver = 3,
        FallbackOnFailure = 5,
    };
    property ActiveReceiverSelect activeReceiverSelect
    {
        ActiveReceiverSelect get()
        {
            return static_cast<InsGnssSelect::ActiveReceiverSelect>(((VN::Registers::INS::InsGnssSelect *)_reg)->activeReceiverSelect.value());
        }
        
        void set(ActiveReceiverSelect value)
        {
            ((VN::Registers::INS::InsGnssSelect *)_reg)->activeReceiverSelect = static_cast<VN::Registers::INS::InsGnssSelect::ActiveReceiverSelect>(value);
        }
        
    };
    
    property Nullable<uint8_t> usedForNavTime
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::INS::InsGnssSelect *)_reg)->usedForNavTime.has_value())
            {
                return ((VN::Registers::INS::InsGnssSelect *)_reg)->usedForNavTime.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsGnssSelect *)_reg)->usedForNavTime = value.Value;
            }
        };
        
    };
    
    property Nullable<uint8_t> hysteresisTime
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::INS::InsGnssSelect *)_reg)->hysteresisTime.has_value())
            {
                return ((VN::Registers::INS::InsGnssSelect *)_reg)->hysteresisTime.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsGnssSelect *)_reg)->hysteresisTime = value.Value;
            }
        };
        
    };
    
    enum class UseGnssCompass : uint8_t
    {
        Off = 0,
        On = 1,
    };
    property UseGnssCompass useGnssCompass
    {
        UseGnssCompass get()
        {
            return static_cast<InsGnssSelect::UseGnssCompass>(((VN::Registers::INS::InsGnssSelect *)_reg)->useGnssCompass.value());
        }
        
        void set(UseGnssCompass value)
        {
            ((VN::Registers::INS::InsGnssSelect *)_reg)->useGnssCompass = static_cast<VN::Registers::INS::InsGnssSelect::UseGnssCompass>(value);
        }
        
    };
    
    property Nullable<uint8_t> resv1
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::INS::InsGnssSelect *)_reg)->resv1.has_value())
            {
                return ((VN::Registers::INS::InsGnssSelect *)_reg)->resv1.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsGnssSelect *)_reg)->resv1 = value.Value;
            }
        };
        
    };
    
    property Nullable<uint8_t> resv2
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::INS::InsGnssSelect *)_reg)->resv2.has_value())
            {
                return ((VN::Registers::INS::InsGnssSelect *)_reg)->resv2.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::INS::InsGnssSelect *)_reg)->resv2 = value.Value;
            }
        };
        
    };
    
    
};

} // namespace INS

namespace System
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 0 - User Tag </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class UserTag : public ConfigRegister
{
    public:
    UserTag() : ConfigRegister(new VN::Registers::System::UserTag())
    {
    };
    
    property String^ tag
    {
        String^ get()
        {
            marshal_context^ context = gcnew marshal_context();
            String^ value = context->marshal_as<String^>(((VN::Registers::System::UserTag *)_reg)->tag.value().c_str());
            delete context;
            return value;
        }
        
        void set(String^ value)
        {
            marshal_context^ context = gcnew marshal_context();
            ((VN::Registers::System::UserTag *)_reg)->tag = context->marshal_as<const char*>(value);
            delete context;
        }
        
    }
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 5 - Baud Rate </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class BaudRate : public ConfigRegister
{
    public:
    BaudRate() : ConfigRegister(new VN::Registers::System::BaudRate())
    {
    };
    
    enum class BaudRates : uint32_t
    {
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200,
        Baud128000 = 128000,
        Baud230400 = 230400,
        Baud460800 = 460800,
        Baud921600 = 921600,
    };
    property BaudRates baudRate
    {
        BaudRates get()
        {
            return static_cast<BaudRate::BaudRates>(((VN::Registers::System::BaudRate *)_reg)->baudRate.value());
        }
        
        void set(BaudRates value)
        {
            ((VN::Registers::System::BaudRate *)_reg)->baudRate = static_cast<VN::Registers::System::BaudRate::BaudRates>(value);
        }
        
    };
    
    enum class SerialPort : uint8_t
    {
        ActiveSerial = 0,                       ///< Set value on active serial port.
        Serial1 = 1,
        Serial2 = 2,
        Poll = static_cast<uint8_t>('?')
    };
    property SerialPort serialPort
    {
        SerialPort get()
        {
            return static_cast<BaudRate::SerialPort>(((VN::Registers::System::BaudRate *)_reg)->serialPort);
        }
        
        void set(SerialPort value)
        {
            ((VN::Registers::System::BaudRate *)_reg)->serialPort = static_cast<VN::Registers::System::BaudRate::SerialPort>(value);
        }
        
    };
    
    
};

inline VN::Registers::System::BaudRate::BaudRates ToNativeInstance(BaudRate::BaudRates baudrate) { return static_cast<VN::Registers::System::BaudRate::BaudRates>(static_cast<int>(baudrate)); }

/**--------------------------------------------------------------------------------------------------
<summary> Register 6 - Async Data Output Type </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class AsyncOutputType : public ConfigRegister
{
    public:
    AsyncOutputType() : ConfigRegister(new VN::Registers::System::AsyncOutputType())
    {
    };
    
    enum class Ador : uint32_t
    {
        OFF = 0,
        YPR = 1,
        QTN = 2,
        QMR = 8,
        MAG = 10,
        ACC = 11,
        GYR = 12,
        MAR = 13,
        YMR = 14,
        YBA = 16,
        YIA = 17,
        IMU = 19,
        GPS = 20,
        GPE = 21,
        INS = 22,
        INE = 23,
        ISL = 28,
        ISE = 29,
        DTV = 30,
        G2S = 32,
        G2E = 33,
        HVE = 34,
    };
    property Ador ador
    {
        Ador get()
        {
            return static_cast<AsyncOutputType::Ador>(((VN::Registers::System::AsyncOutputType *)_reg)->ador.value());
        }
        
        void set(Ador value)
        {
            ((VN::Registers::System::AsyncOutputType *)_reg)->ador = static_cast<VN::Registers::System::AsyncOutputType::Ador>(value);
        }
        
    };
    
    enum class SerialPort : uint8_t
    {
        ActiveSerial = 0,                       ///< Set value on active serial port.
        Serial1 = 1,
        Serial2 = 2,
        Poll = static_cast<uint8_t>('?')
    };
    property SerialPort serialPort
    {
        SerialPort get()
        {
            return static_cast<AsyncOutputType::SerialPort>(((VN::Registers::System::AsyncOutputType *)_reg)->serialPort);
        }
        
        void set(SerialPort value)
        {
            ((VN::Registers::System::AsyncOutputType *)_reg)->serialPort = static_cast<VN::Registers::System::AsyncOutputType::SerialPort>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 7 - Async Data Output Freq </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class AsyncOutputFreq : public ConfigRegister
{
    public:
    AsyncOutputFreq() : ConfigRegister(new VN::Registers::System::AsyncOutputFreq())
    {
    };
    
    enum class Adof : uint32_t
    {
        Rate0Hz = 0,
        Rate1Hz = 1,
        Rate2Hz = 2,
        Rate4Hz = 4,
        Rate5Hz = 5,
        Rate10Hz = 10,
        Rate20Hz = 20,
        Rate25Hz = 25,
        Rate40Hz = 40,
        Rate50Hz = 50,
        Rate100Hz = 100,
        Rate200Hz = 200,
    };
    property Adof adof
    {
        Adof get()
        {
            return static_cast<AsyncOutputFreq::Adof>(((VN::Registers::System::AsyncOutputFreq *)_reg)->adof.value());
        }
        
        void set(Adof value)
        {
            ((VN::Registers::System::AsyncOutputFreq *)_reg)->adof = static_cast<VN::Registers::System::AsyncOutputFreq::Adof>(value);
        }
        
    };
    
    enum class SerialPort : uint8_t
    {
        ActiveSerial = 0,                       ///< Set value on active serial port.
        Serial1 = 1,
        Serial2 = 2,
        Poll = static_cast<uint8_t>('?')
    };
    property SerialPort serialPort
    {
        SerialPort get()
        {
            return static_cast<AsyncOutputFreq::SerialPort>(((VN::Registers::System::AsyncOutputFreq *)_reg)->serialPort);
        }
        
        void set(SerialPort value)
        {
            ((VN::Registers::System::AsyncOutputFreq *)_reg)->serialPort = static_cast<VN::Registers::System::AsyncOutputFreq::SerialPort>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 30 - Communication Protocol Control </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class ProtocolControl : public ConfigRegister
{
    public:
    ProtocolControl() : ConfigRegister(new VN::Registers::System::ProtocolControl())
    {
    };
    
    enum class AsciiAppendCount : uint8_t
    {
        None = 0,
        SyncInCount = 1,
        SyncInTime = 2,
        SyncOutCount = 3,
        GpsPps = 4,
        GpsTow = 5,
    };
    property AsciiAppendCount asciiAppendCount
    {
        AsciiAppendCount get()
        {
            return static_cast<ProtocolControl::AsciiAppendCount>(((VN::Registers::System::ProtocolControl *)_reg)->asciiAppendCount.value());
        }
        
        void set(AsciiAppendCount value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->asciiAppendCount = static_cast<VN::Registers::System::ProtocolControl::AsciiAppendCount>(value);
        }
        
    };
    
    enum class AsciiAppendStatus : uint8_t
    {
        None = 0,
        Ahrs = 1,
        Ins = 2,
        Imu = 3,
        Gnss1 = 4,
        Gnss2 = 5,
        Gnss3 = 6,
    };
    property AsciiAppendStatus asciiAppendStatus
    {
        AsciiAppendStatus get()
        {
            return static_cast<ProtocolControl::AsciiAppendStatus>(((VN::Registers::System::ProtocolControl *)_reg)->asciiAppendStatus.value());
        }
        
        void set(AsciiAppendStatus value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->asciiAppendStatus = static_cast<VN::Registers::System::ProtocolControl::AsciiAppendStatus>(value);
        }
        
    };
    
    enum class SpiAppendCount : uint8_t
    {
        None = 0,
        SyncInCount = 1,
        SyncInTime = 2,
        SyncOutCount = 3,
        GpsPps = 4,
        GpsTow = 5,
    };
    property SpiAppendCount spiAppendCount
    {
        SpiAppendCount get()
        {
            return static_cast<ProtocolControl::SpiAppendCount>(((VN::Registers::System::ProtocolControl *)_reg)->spiAppendCount.value());
        }
        
        void set(SpiAppendCount value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->spiAppendCount = static_cast<VN::Registers::System::ProtocolControl::SpiAppendCount>(value);
        }
        
    };
    
    enum class SpiAppendStatus : uint8_t
    {
        None = 0,
        Ahrs = 1,
        Ins = 2,
        Imu = 3,
        Gnss1 = 4,
        Gnss2 = 5,
        Gnss3 = 6,
    };
    property SpiAppendStatus spiAppendStatus
    {
        SpiAppendStatus get()
        {
            return static_cast<ProtocolControl::SpiAppendStatus>(((VN::Registers::System::ProtocolControl *)_reg)->spiAppendStatus.value());
        }
        
        void set(SpiAppendStatus value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->spiAppendStatus = static_cast<VN::Registers::System::ProtocolControl::SpiAppendStatus>(value);
        }
        
    };
    
    enum class AsciiChecksum : uint8_t
    {
        Checksum8bit = 1,
        Crc16bit = 3,
    };
    property AsciiChecksum asciiChecksum
    {
        AsciiChecksum get()
        {
            return static_cast<ProtocolControl::AsciiChecksum>(((VN::Registers::System::ProtocolControl *)_reg)->asciiChecksum.value());
        }
        
        void set(AsciiChecksum value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->asciiChecksum = static_cast<VN::Registers::System::ProtocolControl::AsciiChecksum>(value);
        }
        
    };
    
    enum class SpiChecksum : uint8_t
    {
        Off = 0,
        Checksum8bit = 1,
        Crc16bit = 3,
    };
    property SpiChecksum spiChecksum
    {
        SpiChecksum get()
        {
            return static_cast<ProtocolControl::SpiChecksum>(((VN::Registers::System::ProtocolControl *)_reg)->spiChecksum.value());
        }
        
        void set(SpiChecksum value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->spiChecksum = static_cast<VN::Registers::System::ProtocolControl::SpiChecksum>(value);
        }
        
    };
    
    enum class ErrorMode : uint8_t
    {
        Ignore = 0,
        SendError = 1,
        AdorOff = 2,
    };
    property ErrorMode errorMode
    {
        ErrorMode get()
        {
            return static_cast<ProtocolControl::ErrorMode>(((VN::Registers::System::ProtocolControl *)_reg)->errorMode.value());
        }
        
        void set(ErrorMode value)
        {
            ((VN::Registers::System::ProtocolControl *)_reg)->errorMode = static_cast<VN::Registers::System::ProtocolControl::ErrorMode>(value);
        }
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 32 - Synchronization Control </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class SyncControl : public ConfigRegister
{
    public:
    SyncControl() : ConfigRegister(new VN::Registers::System::SyncControl())
    {
    };
    
    enum class SyncInMode : uint8_t
    {
        Disable = 0,
        Count = 3,
        ImuSample = 4,
        AsyncAll = 5,
        Async0 = 6,
    };
    property SyncInMode syncInMode
    {
        SyncInMode get()
        {
            return static_cast<SyncControl::SyncInMode>(((VN::Registers::System::SyncControl *)_reg)->syncInMode.value());
        }
        
        void set(SyncInMode value)
        {
            ((VN::Registers::System::SyncControl *)_reg)->syncInMode = static_cast<VN::Registers::System::SyncControl::SyncInMode>(value);
        }
        
    };
    
    enum class SyncInEdge : uint8_t
    {
        RisingEdge = 0,
        FallingEdge = 1,
    };
    property SyncInEdge syncInEdge
    {
        SyncInEdge get()
        {
            return static_cast<SyncControl::SyncInEdge>(((VN::Registers::System::SyncControl *)_reg)->syncInEdge.value());
        }
        
        void set(SyncInEdge value)
        {
            ((VN::Registers::System::SyncControl *)_reg)->syncInEdge = static_cast<VN::Registers::System::SyncControl::SyncInEdge>(value);
        }
        
    };
    
    property Nullable<uint16_t> syncInSkipFactor
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::System::SyncControl *)_reg)->syncInSkipFactor.has_value())
            {
                return ((VN::Registers::System::SyncControl *)_reg)->syncInSkipFactor.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::System::SyncControl *)_reg)->syncInSkipFactor = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> resv1
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::System::SyncControl *)_reg)->resv1.has_value())
            {
                return ((VN::Registers::System::SyncControl *)_reg)->resv1.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::System::SyncControl *)_reg)->resv1 = value.Value;
            }
        };
        
    };
    
    enum class SyncOutMode : uint8_t
    {
        None = 0,
        ImuStart = 1,
        ImuReady = 2,
        NavReady = 3,
        GpsPps = 6,
    };
    property SyncOutMode syncOutMode
    {
        SyncOutMode get()
        {
            return static_cast<SyncControl::SyncOutMode>(((VN::Registers::System::SyncControl *)_reg)->syncOutMode.value());
        }
        
        void set(SyncOutMode value)
        {
            ((VN::Registers::System::SyncControl *)_reg)->syncOutMode = static_cast<VN::Registers::System::SyncControl::SyncOutMode>(value);
        }
        
    };
    
    enum class SyncOutPolarity : uint8_t
    {
        NegativePulse = 0,
        PositivePulse = 1,
    };
    property SyncOutPolarity syncOutPolarity
    {
        SyncOutPolarity get()
        {
            return static_cast<SyncControl::SyncOutPolarity>(((VN::Registers::System::SyncControl *)_reg)->syncOutPolarity.value());
        }
        
        void set(SyncOutPolarity value)
        {
            ((VN::Registers::System::SyncControl *)_reg)->syncOutPolarity = static_cast<VN::Registers::System::SyncControl::SyncOutPolarity>(value);
        }
        
    };
    
    property Nullable<uint16_t> syncOutSkipFactor
    {
        Nullable<uint16_t> get()
        {
            if(((VN::Registers::System::SyncControl *)_reg)->syncOutSkipFactor.has_value())
            {
                return ((VN::Registers::System::SyncControl *)_reg)->syncOutSkipFactor.value();
            }
            else
            {
                return Nullable<uint16_t>();
            }
        };
        
        void set(Nullable<uint16_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::System::SyncControl *)_reg)->syncOutSkipFactor = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> syncOutPulseWidth
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::System::SyncControl *)_reg)->syncOutPulseWidth.has_value())
            {
                return ((VN::Registers::System::SyncControl *)_reg)->syncOutPulseWidth.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::System::SyncControl *)_reg)->syncOutPulseWidth = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> resv2
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::System::SyncControl *)_reg)->resv2.has_value())
            {
                return ((VN::Registers::System::SyncControl *)_reg)->resv2.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::System::SyncControl *)_reg)->resv2 = value.Value;
            }
        };
        
    };
    
    
};

public ref class BinaryOutput : public ConfigRegister
{
    
    public:
    BinaryOutput(VN::Register* reg) : ConfigRegister(reg)
    {
        asyncMode = gcnew AsyncMode(reg);
        common = gcnew CommonGroup(reg);
        time = gcnew TimeGroup(reg);
        imu = gcnew ImuGroup(reg);
        gnss = gcnew GnssGroup(reg);
        attitude = gcnew AttitudeGroup(reg);
        ins = gcnew InsGroup(reg);
        gnss2 = gcnew Gnss2Group(reg);
        gnss3 = gcnew Gnss3Group(reg);
        
    };
    
    ref class AsyncMode
    {
        VN::Register* _reg;
        
        public:
        AsyncMode(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint16_t> _value      ///< BitField value for asyncMode
        {
            Nullable<uint16_t> get()
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (asyncMode.has_value()) { return uint16_t(asyncMode.value()); }
                else { return Nullable<uint16_t>(); }
            }
            
            void set(Nullable<uint16_t> value)
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (!asyncMode.has_value())
                {
                    ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode = VN::Registers::System::BinaryOutput::AsyncMode();
                }
                ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode = uint16_t(value);
            }
        };
        
        // Serial1
        property Nullable<bool> serial1
        {
            Nullable<bool> get()
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (asyncMode.has_value()) { return asyncMode.value().serial1; }
                else { return Nullable<bool>(); }
            }
            
            void set(Nullable<bool> value)
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (!asyncMode.has_value())
                {
                    ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode.emplace();
                }
                ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode->serial1 = bool(value);
            }
        };
        
        // Serial2
        property Nullable<bool> serial2
        {
            Nullable<bool> get()
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (asyncMode.has_value()) { return asyncMode.value().serial2; }
                else { return Nullable<bool>(); }
            }
            
            void set(Nullable<bool> value)
            {
                auto asyncMode = ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode;
                if (!asyncMode.has_value())
                {
                    ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode.emplace();
                }
                ((VN::Registers::System::BinaryOutput *)_reg)->asyncMode->serial2 = bool(value);
            }
        };
        
    };
    
    AsyncMode^ asyncMode;
    
    property Nullable<uint16_t> rateDivisor
    {
        Nullable<uint16_t> get()
        {
            auto rateDivisor = ((VN::Registers::System::BinaryOutput *)_reg)->rateDivisor;
            if (rateDivisor.has_value()) { return uint16_t(rateDivisor.value()); }
            else { return Nullable<uint16_t>(); }
        }
        
        void set(Nullable<uint16_t> value)
        {
            ((VN::Registers::System::BinaryOutput *)_reg)->rateDivisor = uint16_t(value);
        }
        
    };
    
    VN::BinaryHeader toBinaryHeader()
    {
        return ((VN::Registers::System::BinaryOutput *)_reg)->toBinaryHeader();
    }
    
    ref class CommonGroup
    {
        VN::Register* _reg;
        
        public:
        CommonGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->common);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common = value;
            };
            
        }
        
        // TimeStartup
        property bool timeStartup
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.timeStartup;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.timeStartup = value;
            };
            
        }
        
        // TimeGps
        property bool timeGps
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.timeGps;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.timeGps = value;
            };
            
        }
        
        // TimeSyncIn
        property bool timeSyncIn
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.timeSyncIn;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.timeSyncIn = value;
            };
            
        }
        
        // Ypr
        property bool ypr
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.ypr;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.ypr = value;
            };
            
        }
        
        // Quaternion
        property bool quaternion
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.quaternion;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.quaternion = value;
            };
            
        }
        
        // AngularRate
        property bool angularRate
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.angularRate;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.angularRate = value;
            };
            
        }
        
        // PosLla
        property bool posLla
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.posLla;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.posLla = value;
            };
            
        }
        
        // VelNed
        property bool velNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.velNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.velNed = value;
            };
            
        }
        
        // Accel
        property bool accel
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.accel;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.accel = value;
            };
            
        }
        
        // Imu
        property bool imu
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.imu;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.imu = value;
            };
            
        }
        
        // MagPres
        property bool magPres
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.magPres;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.magPres = value;
            };
            
        }
        
        // Deltas
        property bool deltas
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.deltas;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.deltas = value;
            };
            
        }
        
        // InsStatus
        property bool insStatus
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.insStatus;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.insStatus = value;
            };
            
        }
        
        // SyncInCnt
        property bool syncInCnt
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.syncInCnt;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.syncInCnt = value;
            };
            
        }
        
        // TimeGpsPps
        property bool timeGpsPps
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->common.timeGpsPps;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->common.timeGpsPps = value;
            };
            
        }
        
    };
    
    CommonGroup^ common;
    
    ref class TimeGroup
    {
        VN::Register* _reg;
        
        public:
        TimeGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->time);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time = value;
            };
            
        }
        
        // TimeStartup
        property bool timeStartup
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeStartup;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeStartup = value;
            };
            
        }
        
        // TimeGps
        property bool timeGps
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGps;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGps = value;
            };
            
        }
        
        // TimeGpsTow
        property bool timeGpsTow
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsTow;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsTow = value;
            };
            
        }
        
        // TimeGpsWeek
        property bool timeGpsWeek
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsWeek;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsWeek = value;
            };
            
        }
        
        // TimeSyncIn
        property bool timeSyncIn
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeSyncIn;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeSyncIn = value;
            };
            
        }
        
        // TimeGpsPps
        property bool timeGpsPps
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsPps;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeGpsPps = value;
            };
            
        }
        
        // TimeUtc
        property bool timeUtc
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeUtc;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeUtc = value;
            };
            
        }
        
        // SyncInCnt
        property bool syncInCnt
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.syncInCnt;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.syncInCnt = value;
            };
            
        }
        
        // SyncOutCnt
        property bool syncOutCnt
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.syncOutCnt;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.syncOutCnt = value;
            };
            
        }
        
        // TimeStatus
        property bool timeStatus
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->time.timeStatus;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->time.timeStatus = value;
            };
            
        }
        
    };
    
    TimeGroup^ time;
    
    ref class ImuGroup
    {
        VN::Register* _reg;
        
        public:
        ImuGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->imu);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu = value;
            };
            
        }
        
        // ImuStatus
        property bool imuStatus
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.imuStatus;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.imuStatus = value;
            };
            
        }
        
        // UncompMag
        property bool uncompMag
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompMag;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompMag = value;
            };
            
        }
        
        // UncompAccel
        property bool uncompAccel
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompAccel;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompAccel = value;
            };
            
        }
        
        // UncompGyro
        property bool uncompGyro
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompGyro;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.uncompGyro = value;
            };
            
        }
        
        // Temperature
        property bool temperature
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.temperature;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.temperature = value;
            };
            
        }
        
        // Pressure
        property bool pressure
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.pressure;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.pressure = value;
            };
            
        }
        
        // DeltaTheta
        property bool deltaTheta
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.deltaTheta;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.deltaTheta = value;
            };
            
        }
        
        // DeltaVel
        property bool deltaVel
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.deltaVel;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.deltaVel = value;
            };
            
        }
        
        // Mag
        property bool mag
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.mag;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.mag = value;
            };
            
        }
        
        // Accel
        property bool accel
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.accel;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.accel = value;
            };
            
        }
        
        // AngularRate
        property bool angularRate
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.angularRate;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.angularRate = value;
            };
            
        }
        
        // SensSat
        property bool sensSat
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->imu.sensSat;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->imu.sensSat = value;
            };
            
        }
        
    };
    
    ImuGroup^ imu;
    
    ref class GnssGroup
    {
        VN::Register* _reg;
        
        public:
        GnssGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->gnss);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss = value;
            };
            
        }
        
        // Gnss1TimeUtc
        property bool gnss1TimeUtc
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeUtc;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeUtc = value;
            };
            
        }
        
        // Gps1Tow
        property bool gps1Tow
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gps1Tow;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gps1Tow = value;
            };
            
        }
        
        // Gps1Week
        property bool gps1Week
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gps1Week;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gps1Week = value;
            };
            
        }
        
        // Gnss1NumSats
        property bool gnss1NumSats
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1NumSats;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1NumSats = value;
            };
            
        }
        
        // Gnss1Fix
        property bool gnss1Fix
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Fix;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Fix = value;
            };
            
        }
        
        // Gnss1PosLla
        property bool gnss1PosLla
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosLla;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosLla = value;
            };
            
        }
        
        // Gnss1PosEcef
        property bool gnss1PosEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosEcef = value;
            };
            
        }
        
        // Gnss1VelNed
        property bool gnss1VelNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelNed = value;
            };
            
        }
        
        // Gnss1VelEcef
        property bool gnss1VelEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelEcef = value;
            };
            
        }
        
        // Gnss1PosUncertainty
        property bool gnss1PosUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1PosUncertainty = value;
            };
            
        }
        
        // Gnss1VelUncertainty
        property bool gnss1VelUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1VelUncertainty = value;
            };
            
        }
        
        // Gnss1TimeUncertainty
        property bool gnss1TimeUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeUncertainty = value;
            };
            
        }
        
        // Gnss1TimeInfo
        property bool gnss1TimeInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1TimeInfo = value;
            };
            
        }
        
        // Gnss1Dop
        property bool gnss1Dop
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Dop;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Dop = value;
            };
            
        }
        
        // Gnss1SatInfo
        property bool gnss1SatInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1SatInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1SatInfo = value;
            };
            
        }
        
        // Gnss1RawMeas
        property bool gnss1RawMeas
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1RawMeas;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1RawMeas = value;
            };
            
        }
        
        // Gnss1Status
        property bool gnss1Status
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Status;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1Status = value;
            };
            
        }
        
        // Gnss1AltMSL
        property bool gnss1AltMsl
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1AltMsl;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss.gnss1AltMsl = value;
            };
            
        }
        
    };
    
    GnssGroup^ gnss;
    
    ref class AttitudeGroup
    {
        VN::Register* _reg;
        
        public:
        AttitudeGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->attitude);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude = value;
            };
            
        }
        
        // Ypr
        property bool ypr
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.ypr;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.ypr = value;
            };
            
        }
        
        // Quaternion
        property bool quaternion
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.quaternion;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.quaternion = value;
            };
            
        }
        
        // Dcm
        property bool dcm
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.dcm;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.dcm = value;
            };
            
        }
        
        // MagNed
        property bool magNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.magNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.magNed = value;
            };
            
        }
        
        // AccelNed
        property bool accelNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.accelNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.accelNed = value;
            };
            
        }
        
        // LinBodyAcc
        property bool linBodyAcc
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.linBodyAcc;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.linBodyAcc = value;
            };
            
        }
        
        // LinAccelNed
        property bool linAccelNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.linAccelNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.linAccelNed = value;
            };
            
        }
        
        // YprU
        property bool yprU
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.yprU;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.yprU = value;
            };
            
        }
        
        // Heave
        property bool heave
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.heave;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.heave = value;
            };
            
        }
        
        // AttU
        property bool attU
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->attitude.attU;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->attitude.attU = value;
            };
            
        }
        
    };
    
    AttitudeGroup^ attitude;
    
    ref class InsGroup
    {
        VN::Register* _reg;
        
        public:
        InsGroup(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->ins);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins = value;
            };
            
        }
        
        // InsStatus
        property bool insStatus
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.insStatus;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.insStatus = value;
            };
            
        }
        
        // PosLla
        property bool posLla
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.posLla;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.posLla = value;
            };
            
        }
        
        // PosEcef
        property bool posEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.posEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.posEcef = value;
            };
            
        }
        
        // VelBody
        property bool velBody
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.velBody;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.velBody = value;
            };
            
        }
        
        // VelNed
        property bool velNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.velNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.velNed = value;
            };
            
        }
        
        // VelEcef
        property bool velEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.velEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.velEcef = value;
            };
            
        }
        
        // MagEcef
        property bool magEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.magEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.magEcef = value;
            };
            
        }
        
        // AccelEcef
        property bool accelEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.accelEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.accelEcef = value;
            };
            
        }
        
        // LinAccelEcef
        property bool linAccelEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.linAccelEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.linAccelEcef = value;
            };
            
        }
        
        // PosU
        property bool posU
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.posU;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.posU = value;
            };
            
        }
        
        // VelU
        property bool velU
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->ins.velU;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->ins.velU = value;
            };
            
        }
        
    };
    
    InsGroup^ ins;
    
    ref class Gnss2Group
    {
        VN::Register* _reg;
        
        public:
        Gnss2Group(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->gnss2);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2 = value;
            };
            
        }
        
        // Gnss2TimeUtc
        property bool gnss2TimeUtc
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeUtc;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeUtc = value;
            };
            
        }
        
        // Gps2Tow
        property bool gps2Tow
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gps2Tow;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gps2Tow = value;
            };
            
        }
        
        // Gps2Week
        property bool gps2Week
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gps2Week;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gps2Week = value;
            };
            
        }
        
        // Gnss2NumSats
        property bool gnss2NumSats
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2NumSats;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2NumSats = value;
            };
            
        }
        
        // Gnss2Fix
        property bool gnss2Fix
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Fix;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Fix = value;
            };
            
        }
        
        // Gnss2PosLla
        property bool gnss2PosLla
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosLla;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosLla = value;
            };
            
        }
        
        // Gnss2PosEcef
        property bool gnss2PosEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosEcef = value;
            };
            
        }
        
        // Gnss2VelNed
        property bool gnss2VelNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelNed = value;
            };
            
        }
        
        // Gnss2VelEcef
        property bool gnss2VelEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelEcef = value;
            };
            
        }
        
        // Gnss2PosUncertainty
        property bool gnss2PosUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2PosUncertainty = value;
            };
            
        }
        
        // Gnss2VelUncertainty
        property bool gnss2VelUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2VelUncertainty = value;
            };
            
        }
        
        // Gnss2TimeUncertainty
        property bool gnss2TimeUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeUncertainty = value;
            };
            
        }
        
        // Gnss2TimeInfo
        property bool gnss2TimeInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2TimeInfo = value;
            };
            
        }
        
        // Gnss2Dop
        property bool gnss2Dop
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Dop;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Dop = value;
            };
            
        }
        
        // Gnss2SatInfo
        property bool gnss2SatInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2SatInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2SatInfo = value;
            };
            
        }
        
        // Gnss2RawMeas
        property bool gnss2RawMeas
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2RawMeas;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2RawMeas = value;
            };
            
        }
        
        // Gnss2Status
        property bool gnss2Status
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Status;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2Status = value;
            };
            
        }
        
        // Gnss2AltMSL
        property bool gnss2AltMsl
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2AltMsl;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss2.gnss2AltMsl = value;
            };
            
        }
        
    };
    
    Gnss2Group^ gnss2;
    
    ref class Gnss3Group
    {
        VN::Register* _reg;
        
        public:
        Gnss3Group(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property uint32_t _value
        {
            uint32_t get()
            {
                return uint32_t(((VN::Registers::System::BinaryOutput *)_reg)->gnss3);
            };
            
            void set(uint32_t value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3 = value;
            };
            
        }
        
        // Gnss3TimeUtc
        property bool gnss3TimeUtc
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeUtc;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeUtc = value;
            };
            
        }
        
        // Gps3Tow
        property bool gps3Tow
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gps3Tow;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gps3Tow = value;
            };
            
        }
        
        // Gps3Week
        property bool gps3Week
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gps3Week;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gps3Week = value;
            };
            
        }
        
        // Gnss3NumSats
        property bool gnss3NumSats
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3NumSats;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3NumSats = value;
            };
            
        }
        
        // Gnss3Fix
        property bool gnss3Fix
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Fix;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Fix = value;
            };
            
        }
        
        // Gnss3PosLla
        property bool gnss3PosLla
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosLla;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosLla = value;
            };
            
        }
        
        // Gnss3PosEcef
        property bool gnss3PosEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosEcef = value;
            };
            
        }
        
        // Gnss3VelNed
        property bool gnss3VelNed
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelNed;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelNed = value;
            };
            
        }
        
        // Gnss3VelEcef
        property bool gnss3VelEcef
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelEcef;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelEcef = value;
            };
            
        }
        
        // Gnss3PosUncertainty
        property bool gnss3PosUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3PosUncertainty = value;
            };
            
        }
        
        // Gnss3VelUncertainty
        property bool gnss3VelUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3VelUncertainty = value;
            };
            
        }
        
        // Gnss3TimeUncertainty
        property bool gnss3TimeUncertainty
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeUncertainty;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeUncertainty = value;
            };
            
        }
        
        // Gnss3TimeInfo
        property bool gnss3TimeInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3TimeInfo = value;
            };
            
        }
        
        // Gnss3Dop
        property bool gnss3Dop
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Dop;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Dop = value;
            };
            
        }
        
        // Gnss3SatInfo
        property bool gnss3SatInfo
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3SatInfo;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3SatInfo = value;
            };
            
        }
        
        // Gnss3RawMeas
        property bool gnss3RawMeas
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3RawMeas;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3RawMeas = value;
            };
            
        }
        
        // Gnss3Status
        property bool gnss3Status
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Status;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3Status = value;
            };
            
        }
        
        // Gnss3AltMSL
        property bool gnss3AltMsl
        {
            bool get()
            {
                return ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3AltMsl;
            };
            
            void set(bool value)
            {
                ((VN::Registers::System::BinaryOutput *)_reg)->gnss3.gnss3AltMsl = value;
            };
            
        }
        
    };
    
    Gnss3Group^ gnss3;
    
    
};

public ref class BinaryOutput1 : public BinaryOutput
{
    public:
    BinaryOutput1() : BinaryOutput(new VN::Registers::System::BinaryOutput1()) {};
};

public ref class BinaryOutput2 : public BinaryOutput
{
    public:
    BinaryOutput2() : BinaryOutput(new VN::Registers::System::BinaryOutput2()) {};
};

public ref class BinaryOutput3 : public BinaryOutput
{
    public:
    BinaryOutput3() : BinaryOutput(new VN::Registers::System::BinaryOutput3()) {};
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 101 - NMEA Output 1 </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class NmeaOutput1 : public ConfigRegister
{
    public:
    NmeaOutput1() : ConfigRegister(new VN::Registers::System::NmeaOutput1())
    {
        msgSelection = gcnew MsgSelection(_reg);
    };
    
    enum class Port : uint8_t
    {
        None = 0,
        Serial1 = 1,
        Serial2 = 2,
        Serial1And2 = 3,
    };
    property Port port
    {
        Port get()
        {
            return static_cast<NmeaOutput1::Port>(((VN::Registers::System::NmeaOutput1 *)_reg)->port.value());
        }
        
        void set(Port value)
        {
            ((VN::Registers::System::NmeaOutput1 *)_reg)->port = static_cast<VN::Registers::System::NmeaOutput1::Port>(value);
        }
        
    };
    
    enum class Rate : uint8_t
    {
        Rate0Hz = 0,
        Rate1Hz = 1,
        Rate5Hz = 5,
        Rate10Hz = 10,
        Rate20Hz = 20,
    };
    property Rate rate
    {
        Rate get()
        {
            return static_cast<NmeaOutput1::Rate>(((VN::Registers::System::NmeaOutput1 *)_reg)->rate.value());
        }
        
        void set(Rate value)
        {
            ((VN::Registers::System::NmeaOutput1 *)_reg)->rate = static_cast<VN::Registers::System::NmeaOutput1::Rate>(value);
        }
        
    };
    
    enum class Mode : uint8_t
    {
        V41GPID = 0,
        V23GPID = 1,
        V41SYSID = 2,
    };
    property Mode mode
    {
        Mode get()
        {
            return static_cast<NmeaOutput1::Mode>(((VN::Registers::System::NmeaOutput1 *)_reg)->mode.value());
        }
        
        void set(Mode value)
        {
            ((VN::Registers::System::NmeaOutput1 *)_reg)->mode = static_cast<VN::Registers::System::NmeaOutput1::Mode>(value);
        }
        
    };
    
    enum class GnssSelect : uint8_t
    {
        GnssA = 0,
        ActiveReceiver = 1,
    };
    property GnssSelect gnssSelect
    {
        GnssSelect get()
        {
            return static_cast<NmeaOutput1::GnssSelect>(((VN::Registers::System::NmeaOutput1 *)_reg)->gnssSelect.value());
        }
        
        void set(GnssSelect value)
        {
            ((VN::Registers::System::NmeaOutput1 *)_reg)->gnssSelect = static_cast<VN::Registers::System::NmeaOutput1::GnssSelect>(value);
        }
        
    };
    
    ref class MsgSelection
    {
        VN::Register* _reg;
        
        public:
        MsgSelection(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint32_t> _value      ///< BitField value for msgSelection
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return uint32_t(msgSelection.value()); }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection = uint32_t(value);
            }
        };
        
        // RMC_GNSS
        property Nullable<uint32_t> rmcGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().rmcGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->rmcGnss = uint32_t(value);
            }
        };
        
        // RMC_INS
        property Nullable<uint32_t> rmcIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().rmcIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->rmcIns = uint32_t(value);
            }
        };
        
        // GGA_GNSS
        property Nullable<uint32_t> ggaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().ggaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->ggaGnss = uint32_t(value);
            }
        };
        
        // GGA_INS
        property Nullable<uint32_t> ggaIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().ggaIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->ggaIns = uint32_t(value);
            }
        };
        
        // GLL_GNSS
        property Nullable<uint32_t> gllGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gllGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gllGnss = uint32_t(value);
            }
        };
        
        // GLL_INS
        property Nullable<uint32_t> gllIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gllIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gllIns = uint32_t(value);
            }
        };
        
        // GSA_GNSS
        property Nullable<uint32_t> gsaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gsaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gsaGnss = uint32_t(value);
            }
        };
        
        // GSV_GNSS
        property Nullable<uint32_t> gsvGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gsvGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gsvGnss = uint32_t(value);
            }
        };
        
        // HDG_INS
        property Nullable<uint32_t> hdgIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().hdgIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->hdgIns = uint32_t(value);
            }
        };
        
        // HDT_INS
        property Nullable<uint32_t> hdtIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().hdtIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->hdtIns = uint32_t(value);
            }
        };
        
        // THS_INS
        property Nullable<uint32_t> thsIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().thsIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->thsIns = uint32_t(value);
            }
        };
        
        // VTG_GNSS
        property Nullable<uint32_t> vtgGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().vtgGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->vtgGnss = uint32_t(value);
            }
        };
        
        // VTG_INS
        property Nullable<uint32_t> vtgIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().vtgIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->vtgIns = uint32_t(value);
            }
        };
        
        // ZDA_GNSS
        property Nullable<uint32_t> zdaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().zdaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->zdaGnss = uint32_t(value);
            }
        };
        
        // ZDA_INS
        property Nullable<uint32_t> zdaIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().zdaIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->zdaIns = uint32_t(value);
            }
        };
        
        // PASHR_INS
        property Nullable<uint32_t> pashrIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().pashrIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->pashrIns = uint32_t(value);
            }
        };
        
        // TSS1_INS
        property Nullable<uint32_t> tss1Ins
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().tss1Ins; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->tss1Ins = uint32_t(value);
            }
        };
        
        // INDYN
        property Nullable<uint32_t> indyn
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().indyn; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput1 *)_reg)->msgSelection.emplace();
                }
                msgSelection->indyn = uint32_t(value);
            }
        };
        
    };
    
    MsgSelection^ msgSelection;
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 102 - NMEA Output 2 </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class NmeaOutput2 : public ConfigRegister
{
    public:
    NmeaOutput2() : ConfigRegister(new VN::Registers::System::NmeaOutput2())
    {
        msgSelection = gcnew MsgSelection(_reg);
    };
    
    enum class Port : uint8_t
    {
        None = 0,
        Serial1 = 1,
        Serial2 = 2,
        Serial1And2 = 3,
    };
    property Port port
    {
        Port get()
        {
            return static_cast<NmeaOutput2::Port>(((VN::Registers::System::NmeaOutput2 *)_reg)->port.value());
        }
        
        void set(Port value)
        {
            ((VN::Registers::System::NmeaOutput2 *)_reg)->port = static_cast<VN::Registers::System::NmeaOutput2::Port>(value);
        }
        
    };
    
    enum class Rate : uint8_t
    {
        Rate0Hz = 0,
        Rate1Hz = 1,
        Rate5Hz = 5,
        Rate10Hz = 10,
        Rate20Hz = 20,
    };
    property Rate rate
    {
        Rate get()
        {
            return static_cast<NmeaOutput2::Rate>(((VN::Registers::System::NmeaOutput2 *)_reg)->rate.value());
        }
        
        void set(Rate value)
        {
            ((VN::Registers::System::NmeaOutput2 *)_reg)->rate = static_cast<VN::Registers::System::NmeaOutput2::Rate>(value);
        }
        
    };
    
    enum class Mode : uint8_t
    {
        V41GPID = 0,
        V23GPID = 1,
        V41SYSID = 2,
    };
    property Mode mode
    {
        Mode get()
        {
            return static_cast<NmeaOutput2::Mode>(((VN::Registers::System::NmeaOutput2 *)_reg)->mode.value());
        }
        
        void set(Mode value)
        {
            ((VN::Registers::System::NmeaOutput2 *)_reg)->mode = static_cast<VN::Registers::System::NmeaOutput2::Mode>(value);
        }
        
    };
    
    enum class GnssSelect : uint8_t
    {
        GnssA = 0,
        ActiveReceiver = 1,
    };
    property GnssSelect gnssSelect
    {
        GnssSelect get()
        {
            return static_cast<NmeaOutput2::GnssSelect>(((VN::Registers::System::NmeaOutput2 *)_reg)->gnssSelect.value());
        }
        
        void set(GnssSelect value)
        {
            ((VN::Registers::System::NmeaOutput2 *)_reg)->gnssSelect = static_cast<VN::Registers::System::NmeaOutput2::GnssSelect>(value);
        }
        
    };
    
    ref class MsgSelection
    {
        VN::Register* _reg;
        
        public:
        MsgSelection(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint32_t> _value      ///< BitField value for msgSelection
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return uint32_t(msgSelection.value()); }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection = uint32_t(value);
            }
        };
        
        // RMC_GNSS
        property Nullable<uint32_t> rmcGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().rmcGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->rmcGnss = uint32_t(value);
            }
        };
        
        // RMC_INS
        property Nullable<uint32_t> rmcIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().rmcIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->rmcIns = uint32_t(value);
            }
        };
        
        // GGA_GNSS
        property Nullable<uint32_t> ggaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().ggaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->ggaGnss = uint32_t(value);
            }
        };
        
        // GGA_INS
        property Nullable<uint32_t> ggaIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().ggaIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->ggaIns = uint32_t(value);
            }
        };
        
        // GLL_GNSS
        property Nullable<uint32_t> gllGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gllGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gllGnss = uint32_t(value);
            }
        };
        
        // GLL_INS
        property Nullable<uint32_t> gllIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gllIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gllIns = uint32_t(value);
            }
        };
        
        // GSA_GNSS
        property Nullable<uint32_t> gsaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gsaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gsaGnss = uint32_t(value);
            }
        };
        
        // GSV_GNSS
        property Nullable<uint32_t> gsvGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().gsvGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->gsvGnss = uint32_t(value);
            }
        };
        
        // HDG_INS
        property Nullable<uint32_t> hdgIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().hdgIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->hdgIns = uint32_t(value);
            }
        };
        
        // HDT_INS
        property Nullable<uint32_t> hdtIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().hdtIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->hdtIns = uint32_t(value);
            }
        };
        
        // THS_INS
        property Nullable<uint32_t> thsIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().thsIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->thsIns = uint32_t(value);
            }
        };
        
        // VTG_GNSS
        property Nullable<uint32_t> vtgGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().vtgGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->vtgGnss = uint32_t(value);
            }
        };
        
        // VTG_INS
        property Nullable<uint32_t> vtgIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().vtgIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->vtgIns = uint32_t(value);
            }
        };
        
        // ZDA_GNSS
        property Nullable<uint32_t> zdaGnss
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().zdaGnss; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->zdaGnss = uint32_t(value);
            }
        };
        
        // ZDA_INS
        property Nullable<uint32_t> zdaIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().zdaIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->zdaIns = uint32_t(value);
            }
        };
        
        // PASHR_INS
        property Nullable<uint32_t> pashrIns
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().pashrIns; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->pashrIns = uint32_t(value);
            }
        };
        
        // TSS1_INS
        property Nullable<uint32_t> tss1Ins
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().tss1Ins; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->tss1Ins = uint32_t(value);
            }
        };
        
        // INDYN
        property Nullable<uint32_t> indyn
        {
            Nullable<uint32_t> get()
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (msgSelection.has_value()) { return msgSelection.value().indyn; }
                else { return Nullable<uint32_t>(); }
            }
            
            void set(Nullable<uint32_t> value)
            {
                auto msgSelection = ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection;
                if (!msgSelection.has_value())
                {
                    ((VN::Registers::System::NmeaOutput2 *)_reg)->msgSelection.emplace();
                }
                msgSelection->indyn = uint32_t(value);
            }
        };
        
    };
    
    MsgSelection^ msgSelection;
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 206 - Legacy Compatibility Settings </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class LegacyCompatibilitySettings : public ConfigRegister
{
    public:
    LegacyCompatibilitySettings() : ConfigRegister(new VN::Registers::System::LegacyCompatibilitySettings())
    {
        gnssLegacy = gcnew GnssLegacy(_reg);
    };
    
    enum class InsLegacy : uint8_t
    {
        Current = 0,
        Legacy = 1,
    };
    property InsLegacy insLegacy
    {
        InsLegacy get()
        {
            return static_cast<LegacyCompatibilitySettings::InsLegacy>(((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->insLegacy.value());
        }
        
        void set(InsLegacy value)
        {
            ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->insLegacy = static_cast<VN::Registers::System::LegacyCompatibilitySettings::InsLegacy>(value);
        }
        
    };
    
    ref class GnssLegacy
    {
        VN::Register* _reg;
        
        public:
        GnssLegacy(VN::Register* reg)
        {
            _reg = reg;
        }
        
        property Nullable<uint8_t> _value       ///< BitField value for gnssLegacy
        {
            Nullable<uint8_t> get()
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (gnssLegacy.has_value()) { return uint8_t(gnssLegacy.value()); }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (!gnssLegacy.has_value())
                {
                    ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy.emplace();
                }
                ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy = uint8_t(value);
            }
        };
        
        // LegacyGnssFix
        property Nullable<uint8_t> legacyGnssFix
        {
            Nullable<uint8_t> get()
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (gnssLegacy.has_value()) { return gnssLegacy.value().legacyGnssFix; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (!gnssLegacy.has_value())
                {
                    ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy.emplace();
                }
                gnssLegacy->legacyGnssFix = uint8_t(value);
            }
        };
        
        // RequireReg55Reset
        property Nullable<uint8_t> requireReg55Reset
        {
            Nullable<uint8_t> get()
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (gnssLegacy.has_value()) { return gnssLegacy.value().requireReg55Reset; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (!gnssLegacy.has_value())
                {
                    ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy.emplace();
                }
                gnssLegacy->requireReg55Reset = uint8_t(value);
            }
        };
        
        // alwaysPpsPulse
        property Nullable<uint8_t> alwaysPpsPulse
        {
            Nullable<uint8_t> get()
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (gnssLegacy.has_value()) { return gnssLegacy.value().alwaysPpsPulse; }
                else { return Nullable<uint8_t>(); }
            }
            
            void set(Nullable<uint8_t> value)
            {
                auto gnssLegacy = ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy;
                if (!gnssLegacy.has_value())
                {
                    ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->gnssLegacy.emplace();
                }
                gnssLegacy->alwaysPpsPulse = uint8_t(value);
            }
        };
        
    };
    
    GnssLegacy^ gnssLegacy;
    
    enum class ImuLegacy : uint8_t
    {
        Current = 0,
        Legacy = 1,
    };
    property ImuLegacy imuLegacy
    {
        ImuLegacy get()
        {
            return static_cast<LegacyCompatibilitySettings::ImuLegacy>(((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->imuLegacy.value());
        }
        
        void set(ImuLegacy value)
        {
            ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->imuLegacy = static_cast<VN::Registers::System::LegacyCompatibilitySettings::ImuLegacy>(value);
        }
        
    };
    
    enum class HwLegacy : uint8_t
    {
        Current = 0,
        Legacy = 1,
    };
    property HwLegacy hwLegacy
    {
        HwLegacy get()
        {
            return static_cast<LegacyCompatibilitySettings::HwLegacy>(((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->hwLegacy.value());
        }
        
        void set(HwLegacy value)
        {
            ((VN::Registers::System::LegacyCompatibilitySettings *)_reg)->hwLegacy = static_cast<VN::Registers::System::LegacyCompatibilitySettings::HwLegacy>(value);
        }
        
    };
    
    
};

} // namespace System

namespace VelocityAiding
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 50 - Velocity Aiding Measurement </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class VelAidingMeas : public ConfigRegister
{
    public:
    VelAidingMeas() : ConfigRegister(new VN::Registers::VelocityAiding::VelAidingMeas())
    {
    };
    
    property Nullable<float> velocityX
    {
        Nullable<float> get()
        {
            if(((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityX.has_value())
            {
                return ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityX.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityX = value.Value;
            }
        };
        
    };
    
    property Nullable<float> velocityY
    {
        Nullable<float> get()
        {
            if(((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityY.has_value())
            {
                return ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityY.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityY = value.Value;
            }
        };
        
    };
    
    property Nullable<float> velocityZ
    {
        Nullable<float> get()
        {
            if(((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityZ.has_value())
            {
                return ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityZ.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::VelocityAiding::VelAidingMeas *)_reg)->velocityZ = value.Value;
            }
        };
        
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 51 - Velocity Aiding Control </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class VelAidingControl : public ConfigRegister
{
    public:
    VelAidingControl() : ConfigRegister(new VN::Registers::VelocityAiding::VelAidingControl())
    {
    };
    
    enum class VelAidEnable : uint8_t
    {
        Disable = 0,
        Enable = 1,
    };
    property VelAidEnable velAidEnable
    {
        VelAidEnable get()
        {
            return static_cast<VelAidingControl::VelAidEnable>(((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->velAidEnable.value());
        }
        
        void set(VelAidEnable value)
        {
            ((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->velAidEnable = static_cast<VN::Registers::VelocityAiding::VelAidingControl::VelAidEnable>(value);
        }
        
    };
    
    property Nullable<float> velUncertTuning
    {
        Nullable<float> get()
        {
            if(((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->velUncertTuning.has_value())
            {
                return ((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->velUncertTuning.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->velUncertTuning = value.Value;
            }
        };
        
    };
    
    property Nullable<float> resv
    {
        Nullable<float> get()
        {
            if(((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->resv.has_value())
            {
                return ((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->resv.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::VelocityAiding::VelAidingControl *)_reg)->resv = value.Value;
            }
        };
        
    };
    
    
};

} // namespace VelocityAiding

namespace WorldMagGravityModel
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 83 - Reference Model Configuration </summary>
<remarks>
</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class RefModelConfig : public ConfigRegister
{
    public:
    RefModelConfig() : ConfigRegister(new VN::Registers::WorldMagGravityModel::RefModelConfig())
    {
    };
    
    enum class EnableMagModel : uint8_t
    {
        Disabled = 0,
        Enabled = 1,
    };
    property EnableMagModel enableMagModel
    {
        EnableMagModel get()
        {
            return static_cast<RefModelConfig::EnableMagModel>(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->enableMagModel.value());
        }
        
        void set(EnableMagModel value)
        {
            ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->enableMagModel = static_cast<VN::Registers::WorldMagGravityModel::RefModelConfig::EnableMagModel>(value);
        }
        
    };
    
    enum class EnableGravityModel : uint8_t
    {
        Disabled = 0,
        Enabled = 1,
    };
    property EnableGravityModel enableGravityModel
    {
        EnableGravityModel get()
        {
            return static_cast<RefModelConfig::EnableGravityModel>(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->enableGravityModel.value());
        }
        
        void set(EnableGravityModel value)
        {
            ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->enableGravityModel = static_cast<VN::Registers::WorldMagGravityModel::RefModelConfig::EnableGravityModel>(value);
        }
        
    };
    
    property Nullable<uint8_t> resv1
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv1.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv1.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv1 = value.Value;
            }
        };
        
    };
    
    property Nullable<uint8_t> resv2
    {
        Nullable<uint8_t> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv2.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv2.value();
            }
            else
            {
                return Nullable<uint8_t>();
            }
        };
        
        void set(Nullable<uint8_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->resv2 = value.Value;
            }
        };
        
    };
    
    property Nullable<uint32_t> recalcThreshold
    {
        Nullable<uint32_t> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->recalcThreshold.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->recalcThreshold.value();
            }
            else
            {
                return Nullable<uint32_t>();
            }
        };
        
        void set(Nullable<uint32_t> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->recalcThreshold = value.Value;
            }
        };
        
    };
    
    property Nullable<float> year
    {
        Nullable<float> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->year.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->year.value();
            }
            else
            {
                return Nullable<float>();
            }
        };
        
        void set(Nullable<float> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->year = value.Value;
            }
        };
        
    };
    
    property Nullable<double> latitude
    {
        Nullable<double> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->latitude.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->latitude.value();
            }
            else
            {
                return Nullable<double>();
            }
        };
        
        void set(Nullable<double> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->latitude = value.Value;
            }
        };
        
    };
    
    property Nullable<double> longitude
    {
        Nullable<double> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->longitude.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->longitude.value();
            }
            else
            {
                return Nullable<double>();
            }
        };
        
        void set(Nullable<double> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->longitude = value.Value;
            }
        };
        
    };
    
    property Nullable<double> altitude
    {
        Nullable<double> get()
        {
            if(((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->altitude.has_value())
            {
                return ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->altitude.value();
            }
            else
            {
                return Nullable<double>();
            }
        };
        
        void set(Nullable<double> value)
        {
            if(value.HasValue)
            {
                ((VN::Registers::WorldMagGravityModel::RefModelConfig *)_reg)->altitude = value.Value;
            }
        };
        
    };
    
    
};

} // namespace WorldMagGravityModel

namespace Attitude
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 8 - Yaw Pitch Roll </summary>
<remarks>

Attitude solution as yaw, pitch, and roll in degrees. The yaw, pitch, and roll is given as a 3,2,1 
Euler angle rotation sequence describing the orientation of the sensor with respect to the inertial 
North East Down (NED) frame. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class YawPitchRoll : public MeasRegister
{
    public:
    YawPitchRoll() : MeasRegister(new VN::Registers::Attitude::YawPitchRoll())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::Attitude::YawPitchRoll *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YawPitchRoll *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::Attitude::YawPitchRoll *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YawPitchRoll *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::Attitude::YawPitchRoll *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YawPitchRoll *)_reg)->roll = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 9 - Quaternion </summary>
<remarks>

Attitude solution as a quaternion. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Quaternion : public MeasRegister
{
    public:
    Quaternion() : MeasRegister(new VN::Registers::Attitude::Quaternion())
    {
    };
    
    property float quatX
    {
        float get()
        {
            return ((VN::Registers::Attitude::Quaternion *)_reg)->quatX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::Quaternion *)_reg)->quatX = value;
        }
    };
    
    property float quatY
    {
        float get()
        {
            return ((VN::Registers::Attitude::Quaternion *)_reg)->quatY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::Quaternion *)_reg)->quatY = value;
        }
    };
    
    property float quatZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::Quaternion *)_reg)->quatZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::Quaternion *)_reg)->quatZ = value;
        }
    };
    
    property float quatS
    {
        float get()
        {
            return ((VN::Registers::Attitude::Quaternion *)_reg)->quatS;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::Quaternion *)_reg)->quatS = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 15 - Quaternion & Compensated IMU </summary>
<remarks>

Quaternion attitude solution, and compensated (Magnetic, Acceleration, Angular Rate) values. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class QuatMagAccelRate : public MeasRegister
{
    public:
    QuatMagAccelRate() : MeasRegister(new VN::Registers::Attitude::QuatMagAccelRate())
    {
    };
    
    property float quatX
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatX = value;
        }
    };
    
    property float quatY
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatY = value;
        }
    };
    
    property float quatZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatZ = value;
        }
    };
    
    property float quatS
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatS;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->quatS = value;
        }
    };
    
    property float magX
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magX = value;
        }
    };
    
    property float magY
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magY = value;
        }
    };
    
    property float magZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->magZ = value;
        }
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->accelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::QuatMagAccelRate *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 27 - Yaw-Pitch-Roll & Compensated IMU </summary>
<remarks>

Yaw, Pitch, Roll, Accel, and Angular Rates 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class YprMagAccelAngularRates : public MeasRegister
{
    public:
    YprMagAccelAngularRates() : MeasRegister(new VN::Registers::Attitude::YprMagAccelAngularRates())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->roll = value;
        }
    };
    
    property float magX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magX = value;
        }
    };
    
    property float magY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magY = value;
        }
    };
    
    property float magZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->magZ = value;
        }
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->accelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprMagAccelAngularRates *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 239 - Yaw-Pitch-Roll, Linear Acceleration & Gyro </summary>
<remarks>

Yaw, Pitch, Roll, Linear Body Accel, and Angular Rates. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class YprLinearBodyAccelAngularRates : public MeasRegister
{
    public:
    YprLinearBodyAccelAngularRates() : MeasRegister(new VN::Registers::Attitude::YprLinearBodyAccelAngularRates())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->roll = value;
        }
    };
    
    property float linAccelX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelX = value;
        }
    };
    
    property float linAccelY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelY = value;
        }
    };
    
    property float linAccelZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->linAccelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearBodyAccelAngularRates *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 240 - Yaw-Pitch-Roll, Inertial Linear Acceleration & Gyro </summary>
<remarks>

Yaw, Pitch, Roll, Linear Inertial Accel, and Angular Rates. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class YprLinearInertialAccelAngularRates : public MeasRegister
{
    public:
    YprLinearInertialAccelAngularRates() : MeasRegister(new VN::Registers::Attitude::YprLinearInertialAccelAngularRates())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->roll = value;
        }
    };
    
    property float linAccelN
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelN;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelN = value;
        }
    };
    
    property float linAccelE
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelE;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelE = value;
        }
    };
    
    property float linAccelD
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelD;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->linAccelD = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::Attitude::YprLinearInertialAccelAngularRates *)_reg)->gyroZ = value;
        }
    };
    
    
};

} // namespace Attitude

namespace GNSS
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 58 - GNSS Solution - LLA </summary>
<remarks>

Primary GNSS receiver measurement with lat/lon/alt position and velocity in NED frame. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssSolLla : public ConfigRegister
{
    public:
    GnssSolLla() : ConfigRegister(new VN::Registers::GNSS::GnssSolLla())
    {
    };
    
    property double gps1Tow
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gps1Tow;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gps1Tow = value;
        }
    };
    
    property uint16_t gps1Week
    {
        uint16_t get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gps1Week;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gps1Week = value;
        }
    };
    
    enum class Gnss1Fix : uint8_t
    {
        NoFix = 0,
        TimeFix = 1,
        Fix2D = 2,
        Fix3D = 3,
        SBAS = 4,
        RtkFloat = 7,
        RtkFix = 8,
    };
    property Gnss1Fix gnss1Fix
    {
        Gnss1Fix get()
        {
            return static_cast<GnssSolLla::Gnss1Fix>(((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Fix);
        }
        void set(Gnss1Fix value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Fix = static_cast<VN::Registers::GNSS::GnssSolLla::Gnss1Fix>(value);
        }
        
    };
    
    property uint8_t gnss1NumSats
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1NumSats;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1NumSats = value;
        }
    };
    
    property double gnss1Lat
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Lat;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Lat = value;
        }
    };
    
    property double gnss1Lon
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Lon;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Lon = value;
        }
    };
    
    property double gnss1Alt
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Alt;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1Alt = value;
        }
    };
    
    property float gnss1VelN
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelN;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelN = value;
        }
    };
    
    property float gnss1VelE
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelE;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelE = value;
        }
    };
    
    property float gnss1VelD
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelD;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelD = value;
        }
    };
    
    property float gnss1PosUncertaintyN
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyN;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyN = value;
        }
    };
    
    property float gnss1PosUncertaintyE
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyE;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyE = value;
        }
    };
    
    property float gnss1PosUncertaintyD
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyD;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1PosUncertaintyD = value;
        }
    };
    
    property float gnss1VelUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1VelUncertainty = value;
        }
    };
    
    property float gnss1TimeUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1TimeUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolLla *)_reg)->gnss1TimeUncertainty = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 59 - GNSS Solution - ECEF </summary>
<remarks>

Primary GNSS receiver measurement in ECEF frame. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssSolEcef : public ConfigRegister
{
    public:
    GnssSolEcef() : ConfigRegister(new VN::Registers::GNSS::GnssSolEcef())
    {
    };
    
    property double gps1Tow
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gps1Tow;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gps1Tow = value;
        }
    };
    
    property uint16_t gps1Week
    {
        uint16_t get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gps1Week;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gps1Week = value;
        }
    };
    
    enum class Gnss1Fix : uint8_t
    {
        NoFix = 0,
        TimeFix = 1,
        Fix2D = 2,
        Fix3D = 3,
        SBAS = 4,
        RtkFloat = 7,
        RtkFix = 8,
    };
    property Gnss1Fix gnss1Fix
    {
        Gnss1Fix get()
        {
            return static_cast<GnssSolEcef::Gnss1Fix>(((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1Fix);
        }
        void set(Gnss1Fix value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1Fix = static_cast<VN::Registers::GNSS::GnssSolEcef::Gnss1Fix>(value);
        }
        
    };
    
    property uint8_t gnss1NumSats
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1NumSats;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1NumSats = value;
        }
    };
    
    property double gnss1PosX
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosX;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosX = value;
        }
    };
    
    property double gnss1PosY
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosY;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosY = value;
        }
    };
    
    property double gnss1PosZ
    {
        double get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosZ;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosZ = value;
        }
    };
    
    property float gnss1VelX
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelX = value;
        }
    };
    
    property float gnss1VelY
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelY = value;
        }
    };
    
    property float gnss1VelZ
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelZ = value;
        }
    };
    
    property float gnss1PosUncertaintyX
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyX = value;
        }
    };
    
    property float gnss1PosUncertaintyY
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyY = value;
        }
    };
    
    property float gnss1PosUncertaintyZ
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1PosUncertaintyZ = value;
        }
    };
    
    property float gnss1VelUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1VelUncertainty = value;
        }
    };
    
    property float gnss1TimeUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1TimeUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::GnssSolEcef *)_reg)->gnss1TimeUncertainty = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 103 - GNSS 2 Solution - LLA </summary>
<remarks>

Estimated GNSS 2 Solution with lat/lon/alt position. This register is deprecated and will be 
removed in future firmware versions. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Gnss2SolLla : public MeasRegister
{
    public:
    Gnss2SolLla() : MeasRegister(new VN::Registers::GNSS::Gnss2SolLla())
    {
    };
    
    property double gps2Tow
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gps2Tow;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gps2Tow = value;
        }
    };
    
    property uint16_t gps2Week
    {
        uint16_t get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gps2Week;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gps2Week = value;
        }
    };
    
    enum class Gnss2Fix : uint8_t
    {
        NoFix = 0,
        TimeFix = 1,
        Fix2D = 2,
        Fix3D = 3,
        SBAS = 4,
        RtkFloat = 7,
        RtkFix = 8,
    };
    property Gnss2Fix gnss2Fix
    {
        Gnss2Fix get()
        {
            return static_cast<Gnss2SolLla::Gnss2Fix>(((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Fix);
        }
        void set(Gnss2Fix value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Fix = static_cast<VN::Registers::GNSS::Gnss2SolLla::Gnss2Fix>(value);
        }
        
    };
    
    property uint8_t gnss2NumSats
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2NumSats;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2NumSats = value;
        }
    };
    
    property double gnss2Lat
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Lat;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Lat = value;
        }
    };
    
    property double gnss2Lon
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Lon;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Lon = value;
        }
    };
    
    property double gnss2Alt
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Alt;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2Alt = value;
        }
    };
    
    property float gnss2VelN
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelN;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelN = value;
        }
    };
    
    property float gnss2VelE
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelE;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelE = value;
        }
    };
    
    property float gnss2VelD
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelD;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelD = value;
        }
    };
    
    property float gnss2PosUncertaintyN
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyN;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyN = value;
        }
    };
    
    property float gnss2PosUncertaintyE
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyE;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyE = value;
        }
    };
    
    property float gnss2PosUncertaintyD
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyD;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2PosUncertaintyD = value;
        }
    };
    
    property float gnss2VelUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2VelUncertainty = value;
        }
    };
    
    property float gnss2TimeUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2TimeUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolLla *)_reg)->gnss2TimeUncertainty = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 104 - GNSS 2 Solution - ECEF </summary>
<remarks>

Estimated GNSS 2 Solution with ECEF position. This register is deprecated and will be removed in 
future firmware versions. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Gnss2SolEcef : public MeasRegister
{
    public:
    Gnss2SolEcef() : MeasRegister(new VN::Registers::GNSS::Gnss2SolEcef())
    {
    };
    
    property double gps2Tow
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gps2Tow;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gps2Tow = value;
        }
    };
    
    property uint16_t gps2Week
    {
        uint16_t get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gps2Week;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gps2Week = value;
        }
    };
    
    enum class Gnss2Fix : uint8_t
    {
        NoFix = 0,
        TimeFix = 1,
        Fix2D = 2,
        Fix3D = 3,
        SBAS = 4,
        RtkFloat = 7,
        RtkFix = 8,
    };
    property Gnss2Fix gnss2Fix
    {
        Gnss2Fix get()
        {
            return static_cast<Gnss2SolEcef::Gnss2Fix>(((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2Fix);
        }
        void set(Gnss2Fix value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2Fix = static_cast<VN::Registers::GNSS::Gnss2SolEcef::Gnss2Fix>(value);
        }
        
    };
    
    property uint8_t gnss2NumSats
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2NumSats;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2NumSats = value;
        }
    };
    
    property double gnss2PosX
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosX;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosX = value;
        }
    };
    
    property double gnss2PosY
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosY;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosY = value;
        }
    };
    
    property double gnss2PosZ
    {
        double get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosZ;
        }
        void set(double value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosZ = value;
        }
    };
    
    property float gnss2VelX
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelX = value;
        }
    };
    
    property float gnss2VelY
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelY = value;
        }
    };
    
    property float gnss2VelZ
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelZ = value;
        }
    };
    
    property float gnss2PosUncertaintyX
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyX = value;
        }
    };
    
    property float gnss2PosUncertaintyY
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyY = value;
        }
    };
    
    property float gnss2PosUncertaintyZ
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2PosUncertaintyZ = value;
        }
    };
    
    property float gnss2VelUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2VelUncertainty = value;
        }
    };
    
    property float gnss2TimeUncertainty
    {
        float get()
        {
            return ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2TimeUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::GNSS::Gnss2SolEcef *)_reg)->gnss2TimeUncertainty = value;
        }
    };
    
    
};

} // namespace GNSS

namespace GNSSCompass
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 86 - GNSS Compass Signal Health Status </summary>
<remarks>

Provides several indicators that serve as an overall health status of the GNSS compass subsystem. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssCompassSignalHealthStatus : public MeasRegister
{
    public:
    GnssCompassSignalHealthStatus() : MeasRegister(new VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus())
    {
    };
    
    property float numSatsPvtA
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsPvtA;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsPvtA = value;
        }
    };
    
    property float numSatsRtkA
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsRtkA;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsRtkA = value;
        }
    };
    
    property float highestCn0A
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->highestCn0A;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->highestCn0A = value;
        }
    };
    
    property float numSatsPvtB
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsPvtB;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsPvtB = value;
        }
    };
    
    property float numSatsRtkB
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsRtkB;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numSatsRtkB = value;
        }
    };
    
    property float highestCn0B
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->highestCn0B;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->highestCn0B = value;
        }
    };
    
    property float numComSatsPvt
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numComSatsPvt;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numComSatsPvt = value;
        }
    };
    
    property float numComSatsRtk
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numComSatsRtk;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassSignalHealthStatus *)_reg)->numComSatsRtk = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 97 - GNSS Compass Estimated Baseline </summary>
<remarks>

Provides the estimated GNSS compass baseline measurement. The estimated position offset and 
measurement uncertainty is for antenna B relative to antenna A in the body reference frame. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssCompassEstBaseline : public MeasRegister
{
    public:
    GnssCompassEstBaseline() : MeasRegister(new VN::Registers::GNSSCompass::GnssCompassEstBaseline())
    {
    };
    
    property uint8_t estBaselineComplete
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->estBaselineComplete;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->estBaselineComplete = value;
        }
    };
    
    property uint8_t resv
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->resv;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->resv = value;
        }
    };
    
    property uint16_t numMeas
    {
        uint16_t get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->numMeas;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->numMeas = value;
        }
    };
    
    property float positionX
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionX = value;
        }
    };
    
    property float positionY
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionY = value;
        }
    };
    
    property float positionZ
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->positionZ = value;
        }
    };
    
    property float uncertaintyX
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyX;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyX = value;
        }
    };
    
    property float uncertaintyY
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyY;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyY = value;
        }
    };
    
    property float uncertaintyZ
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyZ;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassEstBaseline *)_reg)->uncertaintyZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 98 - GNSS Compass Startup Status </summary>
<remarks>

Provides status information on the GNSS compass startup process. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class GnssCompassStartupStatus : public MeasRegister
{
    public:
    GnssCompassStartupStatus() : MeasRegister(new VN::Registers::GNSSCompass::GnssCompassStartupStatus())
    {
    };
    
    property uint8_t percentComplete
    {
        uint8_t get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassStartupStatus *)_reg)->percentComplete;
        }
        void set(uint8_t value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassStartupStatus *)_reg)->percentComplete = value;
        }
    };
    
    property float currentHeading
    {
        float get()
        {
            return ((VN::Registers::GNSSCompass::GnssCompassStartupStatus *)_reg)->currentHeading;
        }
        void set(float value)
        {
            ((VN::Registers::GNSSCompass::GnssCompassStartupStatus *)_reg)->currentHeading = value;
        }
    };
    
    
};

} // namespace GNSSCompass

namespace HardSoftIronEstimator
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 47 - Real-Time HSI Results </summary>
<remarks>

Magnetometer calibration values calculated by the real-time HSI calibration filter. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class EstMagCal : public MeasRegister
{
    public:
    EstMagCal() : MeasRegister(new VN::Registers::HardSoftIronEstimator::EstMagCal())
    {
    };
    
    property float magGain00
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain00;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain00 = value;
        }
    };
    
    property float magGain01
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain01;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain01 = value;
        }
    };
    
    property float magGain02
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain02;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain02 = value;
        }
    };
    
    property float magGain10
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain10;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain10 = value;
        }
    };
    
    property float magGain11
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain11;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain11 = value;
        }
    };
    
    property float magGain12
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain12;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain12 = value;
        }
    };
    
    property float magGain20
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain20;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain20 = value;
        }
    };
    
    property float magGain21
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain21;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain21 = value;
        }
    };
    
    property float magGain22
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain22;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magGain22 = value;
        }
    };
    
    property float magBiasX
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasX;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasX = value;
        }
    };
    
    property float magBiasY
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasY;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasY = value;
        }
    };
    
    property float magBiasZ
    {
        float get()
        {
            return ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasZ;
        }
        void set(float value)
        {
            ((VN::Registers::HardSoftIronEstimator::EstMagCal *)_reg)->magBiasZ = value;
        }
    };
    
    
};

} // namespace HardSoftIronEstimator

namespace Heave
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 115 - Heave and Heave Rate </summary>
<remarks>

Real-time heave and heave-rate estimates, plus a delayed-heave estimate. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class HeaveOutputs : public MeasRegister
{
    public:
    HeaveOutputs() : MeasRegister(new VN::Registers::Heave::HeaveOutputs())
    {
    };
    
    property float heave
    {
        float get()
        {
            return ((VN::Registers::Heave::HeaveOutputs *)_reg)->heave;
        }
        void set(float value)
        {
            ((VN::Registers::Heave::HeaveOutputs *)_reg)->heave = value;
        }
    };
    
    property float heaveRate
    {
        float get()
        {
            return ((VN::Registers::Heave::HeaveOutputs *)_reg)->heaveRate;
        }
        void set(float value)
        {
            ((VN::Registers::Heave::HeaveOutputs *)_reg)->heaveRate = value;
        }
    };
    
    property float delayedHeave
    {
        float get()
        {
            return ((VN::Registers::Heave::HeaveOutputs *)_reg)->delayedHeave;
        }
        void set(float value)
        {
            ((VN::Registers::Heave::HeaveOutputs *)_reg)->delayedHeave = value;
        }
    };
    
    
};

} // namespace Heave

namespace IMU
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 17 - Compensated Magnetometer </summary>
<remarks>

Compensated magnetometer measurements. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Mag : public MeasRegister
{
    public:
    Mag() : MeasRegister(new VN::Registers::IMU::Mag())
    {
    };
    
    property float magX
    {
        float get()
        {
            return ((VN::Registers::IMU::Mag *)_reg)->magX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Mag *)_reg)->magX = value;
        }
    };
    
    property float magY
    {
        float get()
        {
            return ((VN::Registers::IMU::Mag *)_reg)->magY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Mag *)_reg)->magY = value;
        }
    };
    
    property float magZ
    {
        float get()
        {
            return ((VN::Registers::IMU::Mag *)_reg)->magZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Mag *)_reg)->magZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 18 - Compensated Accelerometer </summary>
<remarks>

Compensated acceleration measurements 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Accel : public MeasRegister
{
    public:
    Accel() : MeasRegister(new VN::Registers::IMU::Accel())
    {
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::IMU::Accel *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Accel *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::IMU::Accel *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Accel *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::IMU::Accel *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Accel *)_reg)->accelZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 19 - Compensated Gyro </summary>
<remarks>

Compensated angular rate measurements. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Gyro : public MeasRegister
{
    public:
    Gyro() : MeasRegister(new VN::Registers::IMU::Gyro())
    {
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::IMU::Gyro *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Gyro *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::IMU::Gyro *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Gyro *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::IMU::Gyro *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::Gyro *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 20 - Compensated IMU </summary>
<remarks>

Compensated magnetic, acceleration, and angular rate measurements. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class MagAccelGyro : public MeasRegister
{
    public:
    MagAccelGyro() : MeasRegister(new VN::Registers::IMU::MagAccelGyro())
    {
    };
    
    property float magX
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->magX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->magX = value;
        }
    };
    
    property float magY
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->magY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->magY = value;
        }
    };
    
    property float magZ
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->magZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->magZ = value;
        }
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->accelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::MagAccelGyro *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 54 - IMU Measurements </summary>
<remarks>

Provides the calibrated IMU measurements including barometric pressure. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class ImuMeas : public MeasRegister
{
    public:
    ImuMeas() : MeasRegister(new VN::Registers::IMU::ImuMeas())
    {
    };
    
    property float uncompMagX
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagX = value;
        }
    };
    
    property float uncompMagY
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagY = value;
        }
    };
    
    property float uncompMagZ
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompMagZ = value;
        }
    };
    
    property float uncompAccX
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccX = value;
        }
    };
    
    property float uncompAccY
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccY = value;
        }
    };
    
    property float uncompAccZ
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompAccZ = value;
        }
    };
    
    property float uncompGyroX
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroX = value;
        }
    };
    
    property float uncompGyroY
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroY = value;
        }
    };
    
    property float uncompGyroZ
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->uncompGyroZ = value;
        }
    };
    
    property float temperature
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->temperature;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->temperature = value;
        }
    };
    
    property float pressure
    {
        float get()
        {
            return ((VN::Registers::IMU::ImuMeas *)_reg)->pressure;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::ImuMeas *)_reg)->pressure = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 80 - Delta Theta and Delta Velocity </summary>
<remarks>

This register contains the output values of the onboard coning and sculling algorithm. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class DeltaThetaVelocity : public MeasRegister
{
    public:
    DeltaThetaVelocity() : MeasRegister(new VN::Registers::IMU::DeltaThetaVelocity())
    {
    };
    
    property float deltaTime
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaTime;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaTime = value;
        }
    };
    
    property float deltaThetaX
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaX = value;
        }
    };
    
    property float deltaThetaY
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaY = value;
        }
    };
    
    property float deltaThetaZ
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaThetaZ = value;
        }
    };
    
    property float deltaVelX
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelX;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelX = value;
        }
    };
    
    property float deltaVelY
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelY;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelY = value;
        }
    };
    
    property float deltaVelZ
    {
        float get()
        {
            return ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelZ;
        }
        void set(float value)
        {
            ((VN::Registers::IMU::DeltaThetaVelocity *)_reg)->deltaVelZ = value;
        }
    };
    
    
};

} // namespace IMU

namespace INS
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 63 - INS Solution - LLA </summary>
<remarks>

Estimated INS solution with lat/lon/alt position. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsSolLla : public MeasRegister
{
    public:
    InsSolLla() : MeasRegister(new VN::Registers::INS::InsSolLla())
    {
    };
    
    property double timeGpsTow
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->timeGpsTow;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->timeGpsTow = value;
        }
    };
    
    property uint16_t timeGpsWeek
    {
        uint16_t get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->timeGpsWeek;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->timeGpsWeek = value;
        }
    };
    
    property VN::InsStatus insStatus
    {
        VN::InsStatus get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->insStatus;
        }
        void set(VN::InsStatus value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->insStatus = value;
        }
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->roll = value;
        }
    };
    
    property double posLat
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->posLat;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->posLat = value;
        }
    };
    
    property double posLon
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->posLon;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->posLon = value;
        }
    };
    
    property double posAlt
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->posAlt;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->posAlt = value;
        }
    };
    
    property float velN
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->velN;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->velN = value;
        }
    };
    
    property float velE
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->velE;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->velE = value;
        }
    };
    
    property float velD
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->velD;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->velD = value;
        }
    };
    
    property float attUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->attUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->attUncertainty = value;
        }
    };
    
    property float posUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->posUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->posUncertainty = value;
        }
    };
    
    property float velUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolLla *)_reg)->velUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolLla *)_reg)->velUncertainty = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 64 - INS Solution - ECEF </summary>
<remarks>

Estimated INS Solution with ECEF position 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsSolEcef : public MeasRegister
{
    public:
    InsSolEcef() : MeasRegister(new VN::Registers::INS::InsSolEcef())
    {
    };
    
    property double timeGpsTow
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->timeGpsTow;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->timeGpsTow = value;
        }
    };
    
    property uint16_t timeGpsWeek
    {
        uint16_t get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->timeGpsWeek;
        }
        void set(uint16_t value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->timeGpsWeek = value;
        }
    };
    
    property VN::InsStatus insStatus
    {
        VN::InsStatus get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->insStatus;
        }
        void set(VN::InsStatus value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->insStatus = value;
        }
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->roll = value;
        }
    };
    
    property double posEx
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->posEx;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->posEx = value;
        }
    };
    
    property double posEy
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->posEy;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->posEy = value;
        }
    };
    
    property double posEz
    {
        double get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->posEz;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->posEz = value;
        }
    };
    
    property float velEx
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->velEx;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->velEx = value;
        }
    };
    
    property float velEy
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->velEy;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->velEy = value;
        }
    };
    
    property float velEz
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->velEz;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->velEz = value;
        }
    };
    
    property float attUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->attUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->attUncertainty = value;
        }
    };
    
    property float posUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->posUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->posUncertainty = value;
        }
    };
    
    property float velUncertainty
    {
        float get()
        {
            return ((VN::Registers::INS::InsSolEcef *)_reg)->velUncertainty;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsSolEcef *)_reg)->velUncertainty = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 72 - INS State - LLA </summary>
<remarks>

Estimated INS state with lat/lon/alt position. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsStateLla : public MeasRegister
{
    public:
    InsStateLla() : MeasRegister(new VN::Registers::INS::InsStateLla())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->roll = value;
        }
    };
    
    property double posLat
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->posLat;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->posLat = value;
        }
    };
    
    property double posLon
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->posLon;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->posLon = value;
        }
    };
    
    property double posAlt
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->posAlt;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->posAlt = value;
        }
    };
    
    property float velN
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->velN;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->velN = value;
        }
    };
    
    property float velE
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->velE;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->velE = value;
        }
    };
    
    property float velD
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->velD;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->velD = value;
        }
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->accelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateLla *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateLla *)_reg)->gyroZ = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 73 - INS State - ECEF </summary>
<remarks>

Estimated INS state with ECEF position. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class InsStateEcef : public MeasRegister
{
    public:
    InsStateEcef() : MeasRegister(new VN::Registers::INS::InsStateEcef())
    {
    };
    
    property float yaw
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->yaw;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->yaw = value;
        }
    };
    
    property float pitch
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->pitch;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->pitch = value;
        }
    };
    
    property float roll
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->roll;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->roll = value;
        }
    };
    
    property double posEx
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->posEx;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->posEx = value;
        }
    };
    
    property double posEy
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->posEy;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->posEy = value;
        }
    };
    
    property double posEz
    {
        double get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->posEz;
        }
        void set(double value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->posEz = value;
        }
    };
    
    property float velEx
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->velEx;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->velEx = value;
        }
    };
    
    property float velEy
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->velEy;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->velEy = value;
        }
    };
    
    property float velEz
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->velEz;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->velEz = value;
        }
    };
    
    property float accelX
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->accelX;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->accelX = value;
        }
    };
    
    property float accelY
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->accelY;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->accelY = value;
        }
    };
    
    property float accelZ
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->accelZ;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->accelZ = value;
        }
    };
    
    property float gyroX
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->gyroX;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->gyroX = value;
        }
    };
    
    property float gyroY
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->gyroY;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->gyroY = value;
        }
    };
    
    property float gyroZ
    {
        float get()
        {
            return ((VN::Registers::INS::InsStateEcef *)_reg)->gyroZ;
        }
        void set(float value)
        {
            ((VN::Registers::INS::InsStateEcef *)_reg)->gyroZ = value;
        }
    };
    
    
};

} // namespace INS

namespace System
{
/**--------------------------------------------------------------------------------------------------
<summary> Register 1 - Model </summary>
<remarks>

Product model string. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Model : public MeasRegister
{
    public:
    Model() : MeasRegister(new VN::Registers::System::Model())
    {
    };
    
    property String^ model
    {
        String^ get()
        {
            marshal_context^ context = gcnew marshal_context();
            String^ value = context->marshal_as<String^>(((VN::Registers::System::Model *)_reg)->model.c_str());
            delete context;
            return value;
        }
        
        void set(String^ value)
        {
            marshal_context^ context = gcnew marshal_context();
            ((VN::Registers::System::Model *)_reg)->model = context->marshal_as<const char*>(value);
            delete context;
        }
        
    }
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 2 - Hardware Version </summary>
<remarks>

Hardware version number. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class HwVer : public MeasRegister
{
    public:
    HwVer() : MeasRegister(new VN::Registers::System::HwVer())
    {
    };
    
    property uint32_t hwVer
    {
        uint32_t get()
        {
            return ((VN::Registers::System::HwVer *)_reg)->hwVer;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::HwVer *)_reg)->hwVer = value;
        }
    };
    
    property uint32_t hwMinVer
    {
        uint32_t get()
        {
            return ((VN::Registers::System::HwVer *)_reg)->hwMinVer;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::HwVer *)_reg)->hwMinVer = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 3 - Serial Number </summary>
<remarks>

Device serial number. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class Serial : public MeasRegister
{
    public:
    Serial() : MeasRegister(new VN::Registers::System::Serial())
    {
    };
    
    property uint32_t serialNum
    {
        uint32_t get()
        {
            return ((VN::Registers::System::Serial *)_reg)->serialNum;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::Serial *)_reg)->serialNum = value;
        }
    };
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 4 - Firmware Version </summary>
<remarks>

Firmware version number. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class FwVer : public MeasRegister
{
    public:
    FwVer() : MeasRegister(new VN::Registers::System::FwVer())
    {
    };
    
    property String^ fwVer
    {
        String^ get()
        {
            marshal_context^ context = gcnew marshal_context();
            String^ value = context->marshal_as<String^>(((VN::Registers::System::FwVer *)_reg)->fwVer.c_str());
            delete context;
            return value;
        }
        
        void set(String^ value)
        {
            marshal_context^ context = gcnew marshal_context();
            ((VN::Registers::System::FwVer *)_reg)->fwVer = context->marshal_as<const char*>(value);
            delete context;
        }
        
    }
    
    
};

/**--------------------------------------------------------------------------------------------------
<summary> Register 33 - Synchronization Status </summary>
<remarks>

Contains counters based on the SyncIn and SyncOut events. 

</remarks>
*-----------------------------------------------------------------------------------------------**/
public ref class SyncStatus : public MeasRegister
{
    public:
    SyncStatus() : MeasRegister(new VN::Registers::System::SyncStatus())
    {
    };
    
    property uint32_t syncInCount
    {
        uint32_t get()
        {
            return ((VN::Registers::System::SyncStatus *)_reg)->syncInCount;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::SyncStatus *)_reg)->syncInCount = value;
        }
    };
    
    property uint32_t syncInTime
    {
        uint32_t get()
        {
            return ((VN::Registers::System::SyncStatus *)_reg)->syncInTime;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::SyncStatus *)_reg)->syncInTime = value;
        }
    };
    
    property uint32_t syncOutCount
    {
        uint32_t get()
        {
            return ((VN::Registers::System::SyncStatus *)_reg)->syncOutCount;
        }
        void set(uint32_t value)
        {
            ((VN::Registers::System::SyncStatus *)_reg)->syncOutCount = value;
        }
    };
    
    
};

} // namespace System

namespace ById {
using reg0 = VNSDK::Registers::System::UserTag;
using reg5 = VNSDK::Registers::System::BaudRate;
using reg6 = VNSDK::Registers::System::AsyncOutputType;
using reg7 = VNSDK::Registers::System::AsyncOutputFreq;
using reg21 = VNSDK::Registers::Attitude::MagGravRefVec;
using reg23 = VNSDK::Registers::IMU::MagCal;
using reg25 = VNSDK::Registers::IMU::AccelCal;
using reg26 = VNSDK::Registers::IMU::RefFrameRot;
using reg30 = VNSDK::Registers::System::ProtocolControl;
using reg32 = VNSDK::Registers::System::SyncControl;
using reg35 = VNSDK::Registers::Attitude::VpeBasicControl;
using reg36 = VNSDK::Registers::Attitude::VpeMagBasicTuning;
using reg38 = VNSDK::Registers::Attitude::VpeAccelBasicTuning;
using reg44 = VNSDK::Registers::HardSoftIronEstimator::RealTimeHsiControl;
using reg50 = VNSDK::Registers::VelocityAiding::VelAidingMeas;
using reg51 = VNSDK::Registers::VelocityAiding::VelAidingControl;
using reg55 = VNSDK::Registers::GNSS::GnssBasicConfig;
using reg57 = VNSDK::Registers::GNSS::GnssAOffset;
using reg67 = VNSDK::Registers::INS::InsBasicConfig;
using reg74 = VNSDK::Registers::INS::FilterStartupBias;
using reg75 = VNSDK::Registers::System::BinaryOutput1;
using reg76 = VNSDK::Registers::System::BinaryOutput2;
using reg77 = VNSDK::Registers::System::BinaryOutput3;
using reg82 = VNSDK::Registers::IMU::DeltaThetaVelConfig;
using reg83 = VNSDK::Registers::WorldMagGravityModel::RefModelConfig;
using reg84 = VNSDK::Registers::IMU::GyroCal;
using reg85 = VNSDK::Registers::IMU::ImuFilterControl;
using reg93 = VNSDK::Registers::GNSSCompass::GnssCompassBaseline;
using reg99 = VNSDK::Registers::GNSS::GnssSystemConfig;
using reg100 = VNSDK::Registers::GNSS::GnssSyncConfig;
using reg101 = VNSDK::Registers::System::NmeaOutput1;
using reg102 = VNSDK::Registers::System::NmeaOutput2;
using reg105 = VNSDK::Registers::INS::InsRefOffset;
using reg116 = VNSDK::Registers::Heave::HeaveBasicConfig;
using reg144 = VNSDK::Registers::INS::InsGnssSelect;
using reg157 = VNSDK::Registers::GNSS::ExtGnssOffset;
using reg206 = VNSDK::Registers::System::LegacyCompatibilitySettings;
using reg1 = VNSDK::Registers::System::Model;
using reg2 = VNSDK::Registers::System::HwVer;
using reg3 = VNSDK::Registers::System::Serial;
using reg4 = VNSDK::Registers::System::FwVer;
using reg8 = VNSDK::Registers::Attitude::YawPitchRoll;
using reg9 = VNSDK::Registers::Attitude::Quaternion;
using reg15 = VNSDK::Registers::Attitude::QuatMagAccelRate;
using reg17 = VNSDK::Registers::IMU::Mag;
using reg18 = VNSDK::Registers::IMU::Accel;
using reg19 = VNSDK::Registers::IMU::Gyro;
using reg20 = VNSDK::Registers::IMU::MagAccelGyro;
using reg27 = VNSDK::Registers::Attitude::YprMagAccelAngularRates;
using reg33 = VNSDK::Registers::System::SyncStatus;
using reg47 = VNSDK::Registers::HardSoftIronEstimator::EstMagCal;
using reg54 = VNSDK::Registers::IMU::ImuMeas;
using reg58 = VNSDK::Registers::GNSS::GnssSolLla;
using reg59 = VNSDK::Registers::GNSS::GnssSolEcef;
using reg63 = VNSDK::Registers::INS::InsSolLla;
using reg64 = VNSDK::Registers::INS::InsSolEcef;
using reg72 = VNSDK::Registers::INS::InsStateLla;
using reg73 = VNSDK::Registers::INS::InsStateEcef;
using reg80 = VNSDK::Registers::IMU::DeltaThetaVelocity;
using reg86 = VNSDK::Registers::GNSSCompass::GnssCompassSignalHealthStatus;
using reg97 = VNSDK::Registers::GNSSCompass::GnssCompassEstBaseline;
using reg98 = VNSDK::Registers::GNSSCompass::GnssCompassStartupStatus;
using reg103 = VNSDK::Registers::GNSS::Gnss2SolLla;
using reg104 = VNSDK::Registers::GNSS::Gnss2SolEcef;
using reg115 = VNSDK::Registers::Heave::HeaveOutputs;
using reg239 = VNSDK::Registers::Attitude::YprLinearBodyAccelAngularRates;
using reg240 = VNSDK::Registers::Attitude::YprLinearInertialAccelAngularRates;
} // namespace ById
} // namespace Registers
} // namespace VNSDK


#endif //VN_CLI_REGISTERS_HPP_


