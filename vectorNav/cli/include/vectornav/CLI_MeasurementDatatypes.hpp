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

#ifndef VN_CLI_MEASUREMENTDATATYPES_HPP_
#define VN_CLI_MEASUREMENTDATATYPES_HPP_

#include "vectornav/CLI_Matrix.hpp"

#pragma managed(push, off)
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#pragma managed(pop)

namespace VNSDK {
public
value struct Time {
    uint64_t nanoseconds() { return time; }
    uint64_t microseconds() { return time/(uint64_t)1e3; }
    uint64_t milliseconds() { return time/(uint64_t)1e6; }
    double seconds() { return (double)time / 1e9; }
    Time(const uint64_t other) : time(other){};
    Time(const double other) : time(other*1e9){};
private:
    uint64_t time;
};

public
value struct TimeStatus {
    property bool towValid {
        bool get() { return _value & 0x01; }
        void set(bool value) { _value = (_value & ~0x01) | (value << 0); }
    }
    
    property bool dateValid {
        bool get() { return _value & 0x02; }
        void set(bool value) { _value = (_value & ~0x02) | (value << 1); }
    }
    
    property bool utcValid {
        bool get() { return _value & 0x04; }
        void set(bool value) { _value = (_value & ~0x04) | (value << 2); }
    }
    
    property uint8_t value {
        uint8_t get() { return _value; }
        void set(uint8_t value) { _value = value; }
    }
    
    TimeStatus(const uint8_t& other): _value(other) {}
private:
    uint8_t _value;
};

public
value struct InsStatus {
    property uint8_t mode {
        uint8_t get() { return _value & 0x03; }
        void set(uint8_t value) { _value = (_value & ~0x03) | (value & 0x03); }
    };
    
    property bool gnssFix {
        bool get() { return _value & 0x04; }
        void set(bool value) { _value = (_value & ~0x04) | (value << 2); }
    };
    
    property bool imuErr {
        bool get() { return _value & 0x10; }
        void set(bool value) { _value = (_value & ~0x10) | (value << 4); }
    };
    
    property bool magPressErr {
        bool get() { return _value & 0x20; }
        void set(bool value) { _value = (_value & ~0x20) | (value << 5); }
    };
    
    property bool gnssErr {
        bool get() { return _value & 0x40; }
        void set(bool value) { _value = (_value & ~0x40) | (value << 6); }
    };
    
    property uint8_t gnssCompassFix {
        uint8_t get() { return (_value & 0x300) >> 8; }
        void set(uint8_t value) { _value = (_value & ~0x300) | ((value << 8) & 0x300); }
    };
    
    property uint16_t value {
        uint16_t get() { return _value; }
        void set(uint16_t value) { _value = value; }
    };
    
    InsStatus(const uint16_t& other): _value(other) {}
private:
    uint16_t _value;
};

public
value struct ImuStatus {
    property uint8_t gyroStatus {
        uint8_t get() { return (_value & 0x03); }
        void set(uint8_t value) { _value = (_value & ~0x03) | (value & 0x03); }
    };
    
    property uint8_t accelStatus {
        uint8_t get() { return (_value & 0x0C) >> 2; }
        void set(uint8_t value) { _value = (_value & ~0x0C) | ((value << 2) & 0x0C); }
    };
    
    property uint8_t magStatus {
        uint8_t get() { return (_value & 0x30) >> 4; }
        void set(uint8_t value) { _value = (_value & ~0x30) | ((value << 4) & 0x30); }
    };
    
    property uint8_t presTempStatus {
        uint8_t get() { return (_value & 0xC0) >> 6; }
        void set(uint8_t value) { _value = (_value & ~0xC0) | ((value << 6) & 0xC0); }
    };
    
    property uint16_t value {
        uint16_t get() { return _value; }
        void set(uint16_t value) { _value = value; }
    };
    
    ImuStatus(const uint16_t& other): _value(other) {}
private:
    uint16_t _value;
};

public
value struct GnssStatus {
    property bool enabled {
        bool get() { return (_value & 0x01); }
        void set(bool value) { _value = (_value & ~0x01) | (value << 0); }
    };
    
    property bool operational {
        bool get() { return (_value & 0x02); }
        void set(bool value) { _value = (_value & ~0x02) | (value << 1); }
    };
    
    property bool fix {
        bool get() { return (_value & 0x04); }
        void set(bool value) { _value = (_value & ~0x04) | (value << 2); }
    };
    
    property bool antennaSignalError {
        bool get() { return (_value & 0x08); }
        void set(bool value) { _value = (_value & ~0x08) | (value << 3); }
    };
    
    property bool usedForNav {
        bool get() { return (_value & 0x10); }
        void set(bool value) { _value = (_value & ~0x10) | (value << 4); }
    };
    
    property uint8_t dataSource {
        uint8_t get() { return (_value & 0x300) >> 8; }
        void set(uint8_t value) { _value = (_value & ~0x300) | ((value << 8) & 0x300); }
    };
    
    property bool usedForNavCurr {
        bool get() { return _value & 0x400; }
        void set(bool value) { _value = (_value & ~0x400) | (value << 11); }
    };
    
    property bool ppsUsedForTime {
        bool get() { return (_value & 0x800); }
        void set(bool value) { _value = (_value & ~0x800) | (value << 12); }
    };
    
    property uint16_t value {
        uint16_t get() { return _value; }
        void set(uint16_t value) { _value = value; }
    };
    
    GnssStatus(const uint16_t& other): _value(other) {}
private:
    uint16_t _value;
};

public
value struct AhrsStatus {
    property uint8_t attitudeQuality {
        uint8_t get() { return (_value & 0x03); }
        void set(uint8_t value) { _value = (_value & ~0x03) | (value & 0x03); }
    };
    
    property bool gyroSaturation {
        bool get() { return (_value & 0x04); }
        void set(bool value) { _value = (_value & ~0x04) | (value << 2); }
    };
    
    property bool gyroSaturationRecovery {
        bool get() { return (_value & 0x08); }
        void set(bool value) { _value = (_value & ~0x08) | (value << 3); }
    };
    
    property uint8_t magDistrubance {
        uint8_t get() { return (_value & 0x30) >> 4; }
        void set(uint8_t value) { _value = (_value & ~0x30) | ((value << 4) & 0x30); }
    };
    
    property bool magSaturation {
        bool get() { return (_value & 0x40); }
        void set(bool value) { _value = (_value & ~0x40) | (value << 6); }
    };
    
    property uint8_t accDisturbance {
        uint8_t get() { return (_value & 0x180) >> 7; }
        void set(uint8_t value) { _value = (_value & ~0x180) | ((value << 7) & 0x180); }
    };
    
    property bool accSaturation {
        bool get() { return (_value & 0x200); }
        void set(bool value) { _value = (_value & ~0x200) | (value << 9); }
    };

    property bool knownMagDisturbance {
        bool get() { return (_value & 0x400); }
        void set(bool value) { _value = (_value & ~0x400) | (value << 11); }
    };

    property bool knownAccDisturbance {
        bool get() { return (_value & 0x800); }
        void set(bool value) { _value = (_value & ~0x800) | (value << 12); }
    };
    
    property uint16_t value {
        uint16_t get() { return _value; }
        void set(uint16_t value) { _value = value; }
    };
    
    AhrsStatus(const uint16_t& other): _value(other) {}
private:
    uint16_t _value;
};

public
value struct Ypr {
    float yaw, pitch, roll;
    Ypr(float y, float p, float r) : yaw(y), pitch(p), roll(r) {}
    Ypr(const VN::Ypr& other) : yaw(other.yaw), pitch(other.pitch), roll(other.roll) {}
    
    VN::Ypr GetNative()
    {
        return VN::Ypr(yaw, pitch, roll);
    }
};

public
value struct DeltaTheta {
    DeltaTheta(const VN::DeltaTheta& other) : deltaTime(other.deltaTime), deltaTheta(other.deltaTheta) {}
    float deltaTime;
    Vec3f deltaTheta;

    VN::DeltaTheta GetNative()
    {
        return VN::DeltaTheta( deltaTime, deltaTheta.GetNative());
    }
};

public
value struct Quaternion
{
    Quaternion(const VN::Quat& other) : vector(other.vector), scalar(other.scalar) {}
    Vec3f vector;
    float scalar;

    VN::Quat GetNative()
    {
        return VN::Quat(vector.GetNative(), scalar);
    }
};

public
value struct Lla {
    double lat, lon, alt;
    Lla(const VN::Lla& other) : lat(other.lat), lon(other.lon), alt(other.alt) { }
    Lla(double lat, double lon, float alt) : lat(lat), lon(lon), alt(alt) {}

    VN::Lla GetNative()
    {
        return VN::Lla(lat, lon, alt);
    }
};

public
value struct TimeUtc {
    int8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t fracSec;

    TimeUtc(const VN::TimeUtc& other)
        : year(other.year), month(other.month), day(other.day), hour(other.hour), minute(other.minute), second(other.second), fracSec(other.fracSec){};
};

public
value struct GnssTimeInfo {
    uint8_t gnssTimeStatus;
    int8_t leapSeconds;

    GnssTimeInfo(const VN::GnssTimeInfo& other) : gnssTimeStatus(other.gnssTimeStatus), leapSeconds(other.leapSeconds){};
};

public
value struct GnssDop {
    float gDop, pDop, tDop, vDop, hDop, nDop, eDop;

    GnssDop(const VN::GnssDop& other)
        : gDop(other.gDop), pDop(other.pDop), tDop(other.tDop), vDop(other.vDop), hDop(other.hDop), nDop(other.nDop), eDop(other.eDop){};
};

public
value struct GnssSatInfo {
    uint8_t numSats;
    uint8_t resv;
    array<uint8_t> ^ sys;
    array<uint8_t> ^ svId;
    array<uint8_t> ^ flags;
    array<uint8_t> ^ cno;
    array<uint8_t> ^ qi;
    array<int8_t> ^ el;
    array<int16_t> ^ az;

    GnssSatInfo(const VN::GnssSatInfo& other)
        : numSats(other.numSats),
          resv(other.resv),
          sys(gcnew array<uint8_t>(numSats)),
          svId(gcnew array<uint8_t>(numSats)),
          flags(gcnew array<uint8_t>(numSats)),
          cno(gcnew array<uint8_t>(numSats)),
          qi(gcnew array<uint8_t>(numSats)),
          el(gcnew array<int8_t>(numSats)),
          az(gcnew array<int16_t>(numSats)) {
        for (uint8_t i = 0; i < numSats; ++i) {
            sys[i] = other.sys[i];
            svId[i] = other.svId[i];
            flags[i] = other.flags[i];
            cno[i] = other.cno[i];
            qi[i] = other.qi[i];
            el[i] = other.el[i];
            az[i] = other.az[i];
        }
    }
};

public
value struct GnssRawMeas {
    double tow;
    uint16_t week;
    uint8_t numMeas;
    uint8_t resv;
    array<uint8_t> ^ sys;
    array<uint8_t> ^ svId;
    array<uint8_t> ^ band;
    array<uint8_t> ^ chan;
    array<int8_t> ^ freqNum;
    array<uint8_t> ^ cno;
    array<uint16_t> ^ flags;
    array<double> ^ pr;
    array<double> ^ cp;
    array<float> ^ dp;

    GnssRawMeas(const VN::GnssRawMeas& other)
        : tow(other.tow),
          week(other.week),
          numMeas(other.numMeas),
          resv(other.resv),
          sys(gcnew array<uint8_t>(other.numMeas)),
          svId(gcnew array<uint8_t>(other.numMeas)),
          band(gcnew array<uint8_t>(other.numMeas)),
          chan(gcnew array<uint8_t>(other.numMeas)),
          freqNum(gcnew array<int8_t>(other.numMeas)),
          cno(gcnew array<uint8_t>(other.numMeas)),
          flags(gcnew array<uint16_t>(other.numMeas)),
          pr(gcnew array<double>(other.numMeas)),
          cp(gcnew array<double>(other.numMeas)),
          dp(gcnew array<float>(other.numMeas)) {
        for (size_t i = 0; i < other.numMeas; ++i) {
            sys[i] = other.sys[i];
            svId[i] = other.svId[i];
            band[i] = other.band[i];
            chan[i] = other.chan[i];
            freqNum[i] = other.freqNum[i];
            cno[i] = other.cno[i];
            flags[i] = other.flags[i];
            pr[i] = other.pr[i];
            cp[i] = other.cp[i];
            dp[i] = other.dp[i];
        }
    }
};

}  // namespace VNSDK

#endif  // VN_CLI_MEASUREMENTDATATYPES_HPP_
