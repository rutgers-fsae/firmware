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


#include "vectornav/Interface/Registers.hpp"

#include <cstdint>

#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{
namespace Registers
{
Vector<const char*, Config::PacketFinders::asciiMaxFieldCount> findIndexOfFieldSeparators(const AsciiMessage& input);
namespace Attitude
{
bool YawPitchRoll::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    return false;  // no errors occurred
}

bool Quaternion::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 4 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatX_tmp = StringUtils::fromString<float>(start, end);
    if (!quatX_tmp.has_value()) { return true; }
    quatX = quatX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatY_tmp = StringUtils::fromString<float>(start, end);
    if (!quatY_tmp.has_value()) { return true; }
    quatY = quatY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatZ_tmp = StringUtils::fromString<float>(start, end);
    if (!quatZ_tmp.has_value()) { return true; }
    quatZ = quatZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatS_tmp = StringUtils::fromString<float>(start, end);
    if (!quatS_tmp.has_value()) { return true; }
    quatS = quatS_tmp.value();

    return false;  // no errors occurred
}

bool QuatMagAccelRate::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 13 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatX_tmp = StringUtils::fromString<float>(start, end);
    if (!quatX_tmp.has_value()) { return true; }
    quatX = quatX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatY_tmp = StringUtils::fromString<float>(start, end);
    if (!quatY_tmp.has_value()) { return true; }
    quatY = quatY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatZ_tmp = StringUtils::fromString<float>(start, end);
    if (!quatZ_tmp.has_value()) { return true; }
    quatZ = quatZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto quatS_tmp = StringUtils::fromString<float>(start, end);
    if (!quatS_tmp.has_value()) { return true; }
    quatS = quatS_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magX_tmp = StringUtils::fromString<float>(start, end);
    if (!magX_tmp.has_value()) { return true; }
    magX = magX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magY_tmp = StringUtils::fromString<float>(start, end);
    if (!magY_tmp.has_value()) { return true; }
    magY = magY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magZ_tmp.has_value()) { return true; }
    magZ = magZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool MagGravRefVec::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 6 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magRefN_tmp = StringUtils::fromString<float>(start, end);
    if (!magRefN_tmp.has_value()) { return true; }
    magRefN = magRefN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magRefE_tmp = StringUtils::fromString<float>(start, end);
    if (!magRefE_tmp.has_value()) { return true; }
    magRefE = magRefE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magRefD_tmp = StringUtils::fromString<float>(start, end);
    if (!magRefD_tmp.has_value()) { return true; }
    magRefD = magRefD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gravRefN_tmp = StringUtils::fromString<float>(start, end);
    if (!gravRefN_tmp.has_value()) { return true; }
    gravRefN = gravRefN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gravRefE_tmp = StringUtils::fromString<float>(start, end);
    if (!gravRefE_tmp.has_value()) { return true; }
    gravRefE = gravRefE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gravRefD_tmp = StringUtils::fromString<float>(start, end);
    if (!gravRefD_tmp.has_value()) { return true; }
    gravRefD = gravRefD_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage MagGravRefVec::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (magRefN.has_value() && magRefE.has_value() && magRefD.has_value() && gravRefN.has_value() && gravRefE.has_value() && gravRefD.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f", magRefN.value(), magRefE.value(), magRefD.value(), gravRefN.value(),
                      gravRefE.value(), gravRefD.value());
    }

    return result;
}

bool YprMagAccelAngularRates::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 12 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magX_tmp = StringUtils::fromString<float>(start, end);
    if (!magX_tmp.has_value()) { return true; }
    magX = magX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magY_tmp = StringUtils::fromString<float>(start, end);
    if (!magY_tmp.has_value()) { return true; }
    magY = magY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magZ_tmp.has_value()) { return true; }
    magZ = magZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool VpeBasicControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 4 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto headingMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!headingMode_tmp.has_value()) { return true; }
    headingMode = static_cast<VpeBasicControl::HeadingMode>(headingMode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto filteringMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!filteringMode_tmp.has_value()) { return true; }
    filteringMode = static_cast<VpeBasicControl::FilteringMode>(filteringMode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto tuningMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!tuningMode_tmp.has_value()) { return true; }
    tuningMode = static_cast<VpeBasicControl::TuningMode>(tuningMode_tmp.value());

    return false;  // no errors occurred
}

AsciiMessage VpeBasicControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (resv.has_value() && headingMode.has_value() && filteringMode.has_value() && tuningMode.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u", resv.value(), static_cast<uint8_t>(headingMode.value()),
                      static_cast<uint8_t>(filteringMode.value()), static_cast<uint8_t>(tuningMode.value()));
    }

    return result;
}

bool VpeMagBasicTuning::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningX_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningX_tmp.has_value()) { return true; }
    baseTuningX = baseTuningX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningY_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningY_tmp.has_value()) { return true; }
    baseTuningY = baseTuningY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningZ_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningZ_tmp.has_value()) { return true; }
    baseTuningZ = baseTuningZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningX_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningX_tmp.has_value()) { return true; }
    adaptiveTuningX = adaptiveTuningX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningY_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningY_tmp.has_value()) { return true; }
    adaptiveTuningY = adaptiveTuningY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningZ_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningZ_tmp.has_value()) { return true; }
    adaptiveTuningZ = adaptiveTuningZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringX_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringX_tmp.has_value()) { return true; }
    adaptiveFilteringX = adaptiveFilteringX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringY_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringY_tmp.has_value()) { return true; }
    adaptiveFilteringY = adaptiveFilteringY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringZ_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringZ_tmp.has_value()) { return true; }
    adaptiveFilteringZ = adaptiveFilteringZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage VpeMagBasicTuning::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (baseTuningX.has_value() && baseTuningY.has_value() && baseTuningZ.has_value() && adaptiveTuningX.has_value() && adaptiveTuningY.has_value() &&
        adaptiveTuningZ.has_value() && adaptiveFilteringX.has_value() && adaptiveFilteringY.has_value() && adaptiveFilteringZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f", baseTuningX.value(), baseTuningY.value(), baseTuningZ.value(),
                      adaptiveTuningX.value(), adaptiveTuningY.value(), adaptiveTuningZ.value(), adaptiveFilteringX.value(), adaptiveFilteringY.value(),
                      adaptiveFilteringZ.value());
    }

    return result;
}

bool VpeAccelBasicTuning::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningX_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningX_tmp.has_value()) { return true; }
    baseTuningX = baseTuningX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningY_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningY_tmp.has_value()) { return true; }
    baseTuningY = baseTuningY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baseTuningZ_tmp = StringUtils::fromString<float>(start, end);
    if (!baseTuningZ_tmp.has_value()) { return true; }
    baseTuningZ = baseTuningZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningX_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningX_tmp.has_value()) { return true; }
    adaptiveTuningX = adaptiveTuningX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningY_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningY_tmp.has_value()) { return true; }
    adaptiveTuningY = adaptiveTuningY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveTuningZ_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveTuningZ_tmp.has_value()) { return true; }
    adaptiveTuningZ = adaptiveTuningZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringX_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringX_tmp.has_value()) { return true; }
    adaptiveFilteringX = adaptiveFilteringX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringY_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringY_tmp.has_value()) { return true; }
    adaptiveFilteringY = adaptiveFilteringY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adaptiveFilteringZ_tmp = StringUtils::fromString<float>(start, end);
    if (!adaptiveFilteringZ_tmp.has_value()) { return true; }
    adaptiveFilteringZ = adaptiveFilteringZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage VpeAccelBasicTuning::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (baseTuningX.has_value() && baseTuningY.has_value() && baseTuningZ.has_value() && adaptiveTuningX.has_value() && adaptiveTuningY.has_value() &&
        adaptiveTuningZ.has_value() && adaptiveFilteringX.has_value() && adaptiveFilteringY.has_value() && adaptiveFilteringZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f", baseTuningX.value(), baseTuningY.value(), baseTuningZ.value(),
                      adaptiveTuningX.value(), adaptiveTuningY.value(), adaptiveTuningZ.value(), adaptiveFilteringX.value(), adaptiveFilteringY.value(),
                      adaptiveFilteringZ.value());
    }

    return result;
}

bool YprLinearBodyAccelAngularRates::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelX_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelX_tmp.has_value()) { return true; }
    linAccelX = linAccelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelY_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelY_tmp.has_value()) { return true; }
    linAccelY = linAccelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelZ_tmp.has_value()) { return true; }
    linAccelZ = linAccelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool YprLinearInertialAccelAngularRates::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelN_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelN_tmp.has_value()) { return true; }
    linAccelN = linAccelN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelE_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelE_tmp.has_value()) { return true; }
    linAccelE = linAccelE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto linAccelD_tmp = StringUtils::fromString<float>(start, end);
    if (!linAccelD_tmp.has_value()) { return true; }
    linAccelD = linAccelD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

}  // namespace Attitude

namespace GNSS
{
bool GnssBasicConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 5 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto receiverEnable_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!receiverEnable_tmp.has_value()) { return true; }
    receiverEnable = static_cast<GnssBasicConfig::ReceiverEnable>(receiverEnable_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto ppsSource_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!ppsSource_tmp.has_value()) { return true; }
    ppsSource = static_cast<GnssBasicConfig::PpsSource>(ppsSource_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rate_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!rate_tmp.has_value()) { return true; }
    rate = static_cast<GnssBasicConfig::Rate>(rate_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv4_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv4_tmp.has_value()) { return true; }
    resv4 = resv4_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto antPower_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!antPower_tmp.has_value()) { return true; }
    antPower = static_cast<GnssBasicConfig::AntPower>(antPower_tmp.value());

    return false;  // no errors occurred
}

AsciiMessage GnssBasicConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (receiverEnable.has_value() && ppsSource.has_value() && rate.has_value() && resv4.has_value() && antPower.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u", static_cast<uint8_t>(receiverEnable.value()),
                      static_cast<uint8_t>(ppsSource.value()), static_cast<uint8_t>(rate.value()), resv4.value(), static_cast<uint8_t>(antPower.value()));
    }

    return result;
}

bool GnssAOffset::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionX_tmp = StringUtils::fromString<float>(start, end);
    if (!positionX_tmp.has_value()) { return true; }
    positionX = positionX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionY_tmp = StringUtils::fromString<float>(start, end);
    if (!positionY_tmp.has_value()) { return true; }
    positionY = positionY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionZ_tmp = StringUtils::fromString<float>(start, end);
    if (!positionZ_tmp.has_value()) { return true; }
    positionZ = positionZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GnssAOffset::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (positionX.has_value() && positionY.has_value() && positionZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f", positionX.value(), positionY.value(), positionZ.value());
    }

    return result;
}

bool GnssSolLla::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps1Tow_tmp = StringUtils::fromString<double>(start, end);
    if (!gps1Tow_tmp.has_value()) { return true; }
    gps1Tow = gps1Tow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps1Week_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!gps1Week_tmp.has_value()) { return true; }
    gps1Week = gps1Week_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1Fix_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss1Fix_tmp.has_value()) { return true; }
    gnss1Fix = static_cast<GnssSolLla::Gnss1Fix>(gnss1Fix_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1NumSats_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss1NumSats_tmp.has_value()) { return true; }
    gnss1NumSats = gnss1NumSats_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1Lat_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1Lat_tmp.has_value()) { return true; }
    gnss1Lat = gnss1Lat_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1Lon_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1Lon_tmp.has_value()) { return true; }
    gnss1Lon = gnss1Lon_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1Alt_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1Alt_tmp.has_value()) { return true; }
    gnss1Alt = gnss1Alt_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelN_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelN_tmp.has_value()) { return true; }
    gnss1VelN = gnss1VelN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelE_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelE_tmp.has_value()) { return true; }
    gnss1VelE = gnss1VelE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelD_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelD_tmp.has_value()) { return true; }
    gnss1VelD = gnss1VelD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyN_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyN_tmp.has_value()) { return true; }
    gnss1PosUncertaintyN = gnss1PosUncertaintyN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyE_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyE_tmp.has_value()) { return true; }
    gnss1PosUncertaintyE = gnss1PosUncertaintyE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyD_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyD_tmp.has_value()) { return true; }
    gnss1PosUncertaintyD = gnss1PosUncertaintyD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelUncertainty_tmp.has_value()) { return true; }
    gnss1VelUncertainty = gnss1VelUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1TimeUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1TimeUncertainty_tmp.has_value()) { return true; }
    gnss1TimeUncertainty = gnss1TimeUncertainty_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GnssSolLla::toString() const
{
    AsciiMessage result = "";

    std::snprintf(result.begin(), result.capacity(), "%.6f,%u,%u,%u,%.8f,%.8f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%g", gps1Tow, gps1Week,
                  static_cast<uint8_t>(gnss1Fix), gnss1NumSats, gnss1Lat, gnss1Lon, gnss1Alt, gnss1VelN, gnss1VelE, gnss1VelD, gnss1PosUncertaintyN,
                  gnss1PosUncertaintyE, gnss1PosUncertaintyD, gnss1VelUncertainty, gnss1TimeUncertainty);

    return result;
}

bool GnssSolEcef::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps1Tow_tmp = StringUtils::fromString<double>(start, end);
    if (!gps1Tow_tmp.has_value()) { return true; }
    gps1Tow = gps1Tow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps1Week_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!gps1Week_tmp.has_value()) { return true; }
    gps1Week = gps1Week_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1Fix_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss1Fix_tmp.has_value()) { return true; }
    gnss1Fix = static_cast<GnssSolEcef::Gnss1Fix>(gnss1Fix_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1NumSats_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss1NumSats_tmp.has_value()) { return true; }
    gnss1NumSats = gnss1NumSats_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosX_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1PosX_tmp.has_value()) { return true; }
    gnss1PosX = gnss1PosX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosY_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1PosY_tmp.has_value()) { return true; }
    gnss1PosY = gnss1PosY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosZ_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss1PosZ_tmp.has_value()) { return true; }
    gnss1PosZ = gnss1PosZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelX_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelX_tmp.has_value()) { return true; }
    gnss1VelX = gnss1VelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelY_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelY_tmp.has_value()) { return true; }
    gnss1VelY = gnss1VelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelZ_tmp.has_value()) { return true; }
    gnss1VelZ = gnss1VelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyX_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyX_tmp.has_value()) { return true; }
    gnss1PosUncertaintyX = gnss1PosUncertaintyX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyY_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyY_tmp.has_value()) { return true; }
    gnss1PosUncertaintyY = gnss1PosUncertaintyY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1PosUncertaintyZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1PosUncertaintyZ_tmp.has_value()) { return true; }
    gnss1PosUncertaintyZ = gnss1PosUncertaintyZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1VelUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1VelUncertainty_tmp.has_value()) { return true; }
    gnss1VelUncertainty = gnss1VelUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss1TimeUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss1TimeUncertainty_tmp.has_value()) { return true; }
    gnss1TimeUncertainty = gnss1TimeUncertainty_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GnssSolEcef::toString() const
{
    AsciiMessage result = "";

    std::snprintf(result.begin(), result.capacity(), "%.6f,%u,%u,%u,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%g", gps1Tow, gps1Week,
                  static_cast<uint8_t>(gnss1Fix), gnss1NumSats, gnss1PosX, gnss1PosY, gnss1PosZ, gnss1VelX, gnss1VelY, gnss1VelZ, gnss1PosUncertaintyX,
                  gnss1PosUncertaintyY, gnss1PosUncertaintyZ, gnss1VelUncertainty, gnss1TimeUncertainty);

    return result;
}

bool GnssSystemConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1)
    {
        // This register may not have an optional field
        if (tokens.size() != 8 + 1) { return true; }
    }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto systems_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!systems_tmp.has_value()) { return true; }
    systems = systems_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto minCno_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!minCno_tmp.has_value()) { return true; }
    minCno = minCno_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto minElev_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!minElev_tmp.has_value()) { return true; }
    minElev = minElev_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto maxSats_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!maxSats_tmp.has_value()) { return true; }
    maxSats = maxSats_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto sbasMode_tmp = StringUtils::fromStringHex<uint8_t>(start, end);
    if (!sbasMode_tmp.has_value()) { return true; }
    sbasMode = sbasMode_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto sbasSelect1_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!sbasSelect1_tmp.has_value()) { return true; }
    sbasSelect1 = sbasSelect1_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto sbasSelect2_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!sbasSelect2_tmp.has_value()) { return true; }
    sbasSelect2 = sbasSelect2_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto sbasSelect3_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!sbasSelect3_tmp.has_value()) { return true; }
    sbasSelect3 = sbasSelect3_tmp.value();

    if (tokens.size() == (9 + 1))
    {
        // This is an optional parameter.
        start = tokens[index++] + 1;
        end = tokens[index];
        const auto receiverSelect_tmp = StringUtils::fromString<uint8_t>(start, end);
        if (!receiverSelect_tmp.has_value()) { return true; }
        receiverSelect = static_cast<GnssSystemConfig::ReceiverSelect>(receiverSelect_tmp.value());
    }

    return false;  // no errors occurred
}

GenericCommand GnssSystemConfig::toReadCommand()
{
    AsciiMessage commandString;
    if (receiverSelect == GNSS::GnssSystemConfig::ReceiverSelect::GnssAB) { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d", _id); }
    else { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,%1d", _id, static_cast<uint8_t>(receiverSelect)); }
    GenericCommand readCommand(commandString, 6);
    return readCommand;
}

AsciiMessage GnssSystemConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (systems.has_value() && minCno.has_value() && minElev.has_value() && maxSats.has_value() && sbasMode.has_value() && sbasSelect1.has_value() &&
        sbasSelect2.has_value() && sbasSelect3.has_value())
    {
        if (receiverSelect == GNSS::GnssSystemConfig::ReceiverSelect::GnssAB)
        {
            std::snprintf(result.begin(), result.capacity(), "%04X,%d,%d,%d,%02X,%04X,%04X,%04X", uint16_t(systems.value()), minCno.value(), minElev.value(),
                          maxSats.value(), uint8_t(sbasMode.value()), uint16_t(sbasSelect1.value()), uint16_t(sbasSelect2.value()),
                          uint16_t(sbasSelect3.value()));
        }
        else
        {
            std::snprintf(result.begin(), result.capacity(), "%04X,%d,%d,%d,%02X,%04X,%04X,%04X,%d", uint16_t(systems.value()), minCno.value(), minElev.value(),
                          maxSats.value(), uint8_t(sbasMode.value()), uint16_t(sbasSelect1.value()), uint16_t(sbasSelect2.value()),
                          uint16_t(sbasSelect3.value()), static_cast<uint8_t>(receiverSelect));
        }
    }
    return result;
}

bool GnssSyncConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 7 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnssSyncEnable_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnssSyncEnable_tmp.has_value()) { return true; }
    gnssSyncEnable = static_cast<GnssSyncConfig::GnssSyncEnable>(gnssSyncEnable_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto polarity_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!polarity_tmp.has_value()) { return true; }
    polarity = static_cast<GnssSyncConfig::Polarity>(polarity_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto specType_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!specType_tmp.has_value()) { return true; }
    specType = static_cast<GnssSyncConfig::SpecType>(specType_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto period_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!period_tmp.has_value()) { return true; }
    period = period_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pulseWidth_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!pulseWidth_tmp.has_value()) { return true; }
    pulseWidth = pulseWidth_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto offset_tmp = StringUtils::fromString<int32_t>(start, end);
    if (!offset_tmp.has_value()) { return true; }
    offset = offset_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GnssSyncConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (gnssSyncEnable.has_value() && polarity.has_value() && specType.has_value() && resv.has_value() && period.has_value() && pulseWidth.has_value() &&
        offset.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%u,%d", static_cast<uint8_t>(gnssSyncEnable.value()),
                      static_cast<uint8_t>(polarity.value()), static_cast<uint8_t>(specType.value()), resv.value(), period.value(), pulseWidth.value(),
                      offset.value());
    }

    return result;
}

bool Gnss2SolLla::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps2Tow_tmp = StringUtils::fromString<double>(start, end);
    if (!gps2Tow_tmp.has_value()) { return true; }
    gps2Tow = gps2Tow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps2Week_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!gps2Week_tmp.has_value()) { return true; }
    gps2Week = gps2Week_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2Fix_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss2Fix_tmp.has_value()) { return true; }
    gnss2Fix = static_cast<Gnss2SolLla::Gnss2Fix>(gnss2Fix_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2NumSats_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss2NumSats_tmp.has_value()) { return true; }
    gnss2NumSats = gnss2NumSats_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2Lat_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2Lat_tmp.has_value()) { return true; }
    gnss2Lat = gnss2Lat_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2Lon_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2Lon_tmp.has_value()) { return true; }
    gnss2Lon = gnss2Lon_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2Alt_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2Alt_tmp.has_value()) { return true; }
    gnss2Alt = gnss2Alt_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelN_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelN_tmp.has_value()) { return true; }
    gnss2VelN = gnss2VelN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelE_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelE_tmp.has_value()) { return true; }
    gnss2VelE = gnss2VelE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelD_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelD_tmp.has_value()) { return true; }
    gnss2VelD = gnss2VelD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyN_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyN_tmp.has_value()) { return true; }
    gnss2PosUncertaintyN = gnss2PosUncertaintyN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyE_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyE_tmp.has_value()) { return true; }
    gnss2PosUncertaintyE = gnss2PosUncertaintyE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyD_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyD_tmp.has_value()) { return true; }
    gnss2PosUncertaintyD = gnss2PosUncertaintyD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelUncertainty_tmp.has_value()) { return true; }
    gnss2VelUncertainty = gnss2VelUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2TimeUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2TimeUncertainty_tmp.has_value()) { return true; }
    gnss2TimeUncertainty = gnss2TimeUncertainty_tmp.value();

    return false;  // no errors occurred
}

bool Gnss2SolEcef::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps2Tow_tmp = StringUtils::fromString<double>(start, end);
    if (!gps2Tow_tmp.has_value()) { return true; }
    gps2Tow = gps2Tow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gps2Week_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!gps2Week_tmp.has_value()) { return true; }
    gps2Week = gps2Week_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2Fix_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss2Fix_tmp.has_value()) { return true; }
    gnss2Fix = static_cast<Gnss2SolEcef::Gnss2Fix>(gnss2Fix_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2NumSats_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnss2NumSats_tmp.has_value()) { return true; }
    gnss2NumSats = gnss2NumSats_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosX_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2PosX_tmp.has_value()) { return true; }
    gnss2PosX = gnss2PosX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosY_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2PosY_tmp.has_value()) { return true; }
    gnss2PosY = gnss2PosY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosZ_tmp = StringUtils::fromString<double>(start, end);
    if (!gnss2PosZ_tmp.has_value()) { return true; }
    gnss2PosZ = gnss2PosZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelX_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelX_tmp.has_value()) { return true; }
    gnss2VelX = gnss2VelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelY_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelY_tmp.has_value()) { return true; }
    gnss2VelY = gnss2VelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelZ_tmp.has_value()) { return true; }
    gnss2VelZ = gnss2VelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyX_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyX_tmp.has_value()) { return true; }
    gnss2PosUncertaintyX = gnss2PosUncertaintyX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyY_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyY_tmp.has_value()) { return true; }
    gnss2PosUncertaintyY = gnss2PosUncertaintyY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2PosUncertaintyZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2PosUncertaintyZ_tmp.has_value()) { return true; }
    gnss2PosUncertaintyZ = gnss2PosUncertaintyZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2VelUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2VelUncertainty_tmp.has_value()) { return true; }
    gnss2VelUncertainty = gnss2VelUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnss2TimeUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!gnss2TimeUncertainty_tmp.has_value()) { return true; }
    gnss2TimeUncertainty = gnss2TimeUncertainty_tmp.value();

    return false;  // no errors occurred
}

bool ExtGnssOffset::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionX_tmp = StringUtils::fromString<float>(start, end);
    if (!positionX_tmp.has_value()) { return true; }
    positionX = positionX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionY_tmp = StringUtils::fromString<float>(start, end);
    if (!positionY_tmp.has_value()) { return true; }
    positionY = positionY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionZ_tmp = StringUtils::fromString<float>(start, end);
    if (!positionZ_tmp.has_value()) { return true; }
    positionZ = positionZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage ExtGnssOffset::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (positionX.has_value() && positionY.has_value() && positionZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f", positionX.value(), positionY.value(), positionZ.value());
    }

    return result;
}

}  // namespace GNSS

namespace GNSSCompass
{
bool GnssCompassSignalHealthStatus::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 8 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numSatsPvtA_tmp = StringUtils::fromString<float>(start, end);
    if (!numSatsPvtA_tmp.has_value()) { return true; }
    numSatsPvtA = numSatsPvtA_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numSatsRtkA_tmp = StringUtils::fromString<float>(start, end);
    if (!numSatsRtkA_tmp.has_value()) { return true; }
    numSatsRtkA = numSatsRtkA_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto highestCn0A_tmp = StringUtils::fromString<float>(start, end);
    if (!highestCn0A_tmp.has_value()) { return true; }
    highestCn0A = highestCn0A_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numSatsPvtB_tmp = StringUtils::fromString<float>(start, end);
    if (!numSatsPvtB_tmp.has_value()) { return true; }
    numSatsPvtB = numSatsPvtB_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numSatsRtkB_tmp = StringUtils::fromString<float>(start, end);
    if (!numSatsRtkB_tmp.has_value()) { return true; }
    numSatsRtkB = numSatsRtkB_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto highestCn0B_tmp = StringUtils::fromString<float>(start, end);
    if (!highestCn0B_tmp.has_value()) { return true; }
    highestCn0B = highestCn0B_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numComSatsPvt_tmp = StringUtils::fromString<float>(start, end);
    if (!numComSatsPvt_tmp.has_value()) { return true; }
    numComSatsPvt = numComSatsPvt_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numComSatsRtk_tmp = StringUtils::fromString<float>(start, end);
    if (!numComSatsRtk_tmp.has_value()) { return true; }
    numComSatsRtk = numComSatsRtk_tmp.value();

    return false;  // no errors occurred
}

bool GnssCompassBaseline::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 6 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionX_tmp = StringUtils::fromString<float>(start, end);
    if (!positionX_tmp.has_value()) { return true; }
    positionX = positionX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionY_tmp = StringUtils::fromString<float>(start, end);
    if (!positionY_tmp.has_value()) { return true; }
    positionY = positionY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionZ_tmp = StringUtils::fromString<float>(start, end);
    if (!positionZ_tmp.has_value()) { return true; }
    positionZ = positionZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyX_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyX_tmp.has_value()) { return true; }
    uncertaintyX = uncertaintyX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyY_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyY_tmp.has_value()) { return true; }
    uncertaintyY = uncertaintyY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyZ_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyZ_tmp.has_value()) { return true; }
    uncertaintyZ = uncertaintyZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GnssCompassBaseline::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (positionX.has_value() && positionY.has_value() && positionZ.has_value() && uncertaintyX.has_value() && uncertaintyY.has_value() &&
        uncertaintyZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f", positionX.value(), positionY.value(), positionZ.value(), uncertaintyX.value(),
                      uncertaintyY.value(), uncertaintyZ.value());
    }

    return result;
}

bool GnssCompassEstBaseline::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto estBaselineComplete_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!estBaselineComplete_tmp.has_value()) { return true; }
    estBaselineComplete = estBaselineComplete_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto numMeas_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!numMeas_tmp.has_value()) { return true; }
    numMeas = numMeas_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionX_tmp = StringUtils::fromString<float>(start, end);
    if (!positionX_tmp.has_value()) { return true; }
    positionX = positionX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionY_tmp = StringUtils::fromString<float>(start, end);
    if (!positionY_tmp.has_value()) { return true; }
    positionY = positionY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto positionZ_tmp = StringUtils::fromString<float>(start, end);
    if (!positionZ_tmp.has_value()) { return true; }
    positionZ = positionZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyX_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyX_tmp.has_value()) { return true; }
    uncertaintyX = uncertaintyX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyY_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyY_tmp.has_value()) { return true; }
    uncertaintyY = uncertaintyY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncertaintyZ_tmp = StringUtils::fromString<float>(start, end);
    if (!uncertaintyZ_tmp.has_value()) { return true; }
    uncertaintyZ = uncertaintyZ_tmp.value();

    return false;  // no errors occurred
}

bool GnssCompassStartupStatus::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 2 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto percentComplete_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!percentComplete_tmp.has_value()) { return true; }
    percentComplete = percentComplete_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto currentHeading_tmp = StringUtils::fromString<float>(start, end);
    if (!currentHeading_tmp.has_value()) { return true; }
    currentHeading = currentHeading_tmp.value();

    return false;  // no errors occurred
}

}  // namespace GNSSCompass

namespace HardSoftIronEstimator
{
bool RealTimeHsiControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto mode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!mode_tmp.has_value()) { return true; }
    mode = static_cast<RealTimeHsiControl::Mode>(mode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto applyCompensation_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!applyCompensation_tmp.has_value()) { return true; }
    applyCompensation = static_cast<RealTimeHsiControl::ApplyCompensation>(applyCompensation_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto convergeRate_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!convergeRate_tmp.has_value()) { return true; }
    convergeRate = convergeRate_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage RealTimeHsiControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (mode.has_value() && applyCompensation.has_value() && convergeRate.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u", static_cast<uint8_t>(mode.value()), static_cast<uint8_t>(applyCompensation.value()),
                      convergeRate.value());
    }

    return result;
}

bool EstMagCal::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 12 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain00_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain00_tmp.has_value()) { return true; }
    magGain00 = magGain00_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain01_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain01_tmp.has_value()) { return true; }
    magGain01 = magGain01_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain02_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain02_tmp.has_value()) { return true; }
    magGain02 = magGain02_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain10_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain10_tmp.has_value()) { return true; }
    magGain10 = magGain10_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain11_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain11_tmp.has_value()) { return true; }
    magGain11 = magGain11_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain12_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain12_tmp.has_value()) { return true; }
    magGain12 = magGain12_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain20_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain20_tmp.has_value()) { return true; }
    magGain20 = magGain20_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain21_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain21_tmp.has_value()) { return true; }
    magGain21 = magGain21_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain22_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain22_tmp.has_value()) { return true; }
    magGain22 = magGain22_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasX_tmp.has_value()) { return true; }
    magBiasX = magBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasY_tmp.has_value()) { return true; }
    magBiasY = magBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasZ_tmp.has_value()) { return true; }
    magBiasZ = magBiasZ_tmp.value();

    return false;  // no errors occurred
}

}  // namespace HardSoftIronEstimator

namespace Heave
{
bool HeaveOutputs::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto heave_tmp = StringUtils::fromString<float>(start, end);
    if (!heave_tmp.has_value()) { return true; }
    heave = heave_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto heaveRate_tmp = StringUtils::fromString<float>(start, end);
    if (!heaveRate_tmp.has_value()) { return true; }
    heaveRate = heaveRate_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto delayedHeave_tmp = StringUtils::fromString<float>(start, end);
    if (!delayedHeave_tmp.has_value()) { return true; }
    delayedHeave = delayedHeave_tmp.value();

    return false;  // no errors occurred
}

bool HeaveBasicConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 7 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto initialWavePeriod_tmp = StringUtils::fromString<float>(start, end);
    if (!initialWavePeriod_tmp.has_value()) { return true; }
    initialWavePeriod = initialWavePeriod_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto initialWaveAmplitude_tmp = StringUtils::fromString<float>(start, end);
    if (!initialWaveAmplitude_tmp.has_value()) { return true; }
    initialWaveAmplitude = initialWaveAmplitude_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto maxWavePeriod_tmp = StringUtils::fromString<float>(start, end);
    if (!maxWavePeriod_tmp.has_value()) { return true; }
    maxWavePeriod = maxWavePeriod_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto minWaveAmplitude_tmp = StringUtils::fromString<float>(start, end);
    if (!minWaveAmplitude_tmp.has_value()) { return true; }
    minWaveAmplitude = minWaveAmplitude_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto delayedHeaveCutoffFreq_tmp = StringUtils::fromString<float>(start, end);
    if (!delayedHeaveCutoffFreq_tmp.has_value()) { return true; }
    delayedHeaveCutoffFreq = delayedHeaveCutoffFreq_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto heaveCutoffFreq_tmp = StringUtils::fromString<float>(start, end);
    if (!heaveCutoffFreq_tmp.has_value()) { return true; }
    heaveCutoffFreq = heaveCutoffFreq_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto heaveRateCutoffFreq_tmp = StringUtils::fromString<float>(start, end);
    if (!heaveRateCutoffFreq_tmp.has_value()) { return true; }
    heaveRateCutoffFreq = heaveRateCutoffFreq_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage HeaveBasicConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (initialWavePeriod.has_value() && initialWaveAmplitude.has_value() && maxWavePeriod.has_value() && minWaveAmplitude.has_value() &&
        delayedHeaveCutoffFreq.has_value() && heaveCutoffFreq.has_value() && heaveRateCutoffFreq.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f", initialWavePeriod.value(), initialWaveAmplitude.value(), maxWavePeriod.value(),
                      minWaveAmplitude.value(), delayedHeaveCutoffFreq.value(), heaveCutoffFreq.value(), heaveRateCutoffFreq.value());
    }

    return result;
}

}  // namespace Heave

namespace IMU
{
bool Mag::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magX_tmp = StringUtils::fromString<float>(start, end);
    if (!magX_tmp.has_value()) { return true; }
    magX = magX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magY_tmp = StringUtils::fromString<float>(start, end);
    if (!magY_tmp.has_value()) { return true; }
    magY = magY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magZ_tmp.has_value()) { return true; }
    magZ = magZ_tmp.value();

    return false;  // no errors occurred
}

bool Accel::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    return false;  // no errors occurred
}

bool Gyro::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool MagAccelGyro::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magX_tmp = StringUtils::fromString<float>(start, end);
    if (!magX_tmp.has_value()) { return true; }
    magX = magX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magY_tmp = StringUtils::fromString<float>(start, end);
    if (!magY_tmp.has_value()) { return true; }
    magY = magY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magZ_tmp.has_value()) { return true; }
    magZ = magZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool MagCal::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 12 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain00_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain00_tmp.has_value()) { return true; }
    magGain00 = magGain00_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain01_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain01_tmp.has_value()) { return true; }
    magGain01 = magGain01_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain02_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain02_tmp.has_value()) { return true; }
    magGain02 = magGain02_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain10_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain10_tmp.has_value()) { return true; }
    magGain10 = magGain10_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain11_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain11_tmp.has_value()) { return true; }
    magGain11 = magGain11_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain12_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain12_tmp.has_value()) { return true; }
    magGain12 = magGain12_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain20_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain20_tmp.has_value()) { return true; }
    magGain20 = magGain20_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain21_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain21_tmp.has_value()) { return true; }
    magGain21 = magGain21_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magGain22_tmp = StringUtils::fromString<float>(start, end);
    if (!magGain22_tmp.has_value()) { return true; }
    magGain22 = magGain22_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasX_tmp.has_value()) { return true; }
    magBiasX = magBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasY_tmp.has_value()) { return true; }
    magBiasY = magBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!magBiasZ_tmp.has_value()) { return true; }
    magBiasZ = magBiasZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage MagCal::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (magGain00.has_value() && magGain01.has_value() && magGain02.has_value() && magGain10.has_value() && magGain11.has_value() && magGain12.has_value() &&
        magGain20.has_value() && magGain21.has_value() && magGain22.has_value() && magBiasX.has_value() && magBiasY.has_value() && magBiasZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", magGain00.value(), magGain01.value(), magGain02.value(),
                      magGain10.value(), magGain11.value(), magGain12.value(), magGain20.value(), magGain21.value(), magGain22.value(), magBiasX.value(),
                      magBiasY.value(), magBiasZ.value());
    }

    return result;
}

bool AccelCal::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 12 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain00_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain00_tmp.has_value()) { return true; }
    accelGain00 = accelGain00_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain01_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain01_tmp.has_value()) { return true; }
    accelGain01 = accelGain01_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain02_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain02_tmp.has_value()) { return true; }
    accelGain02 = accelGain02_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain10_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain10_tmp.has_value()) { return true; }
    accelGain10 = accelGain10_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain11_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain11_tmp.has_value()) { return true; }
    accelGain11 = accelGain11_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain12_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain12_tmp.has_value()) { return true; }
    accelGain12 = accelGain12_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain20_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain20_tmp.has_value()) { return true; }
    accelGain20 = accelGain20_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain21_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain21_tmp.has_value()) { return true; }
    accelGain21 = accelGain21_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelGain22_tmp = StringUtils::fromString<float>(start, end);
    if (!accelGain22_tmp.has_value()) { return true; }
    accelGain22 = accelGain22_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasX_tmp.has_value()) { return true; }
    accelBiasX = accelBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasY_tmp.has_value()) { return true; }
    accelBiasY = accelBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasZ_tmp.has_value()) { return true; }
    accelBiasZ = accelBiasZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage AccelCal::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (accelGain00.has_value() && accelGain01.has_value() && accelGain02.has_value() && accelGain10.has_value() && accelGain11.has_value() &&
        accelGain12.has_value() && accelGain20.has_value() && accelGain21.has_value() && accelGain22.has_value() && accelBiasX.has_value() &&
        accelBiasY.has_value() && accelBiasZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", accelGain00.value(), accelGain01.value(), accelGain02.value(),
                      accelGain10.value(), accelGain11.value(), accelGain12.value(), accelGain20.value(), accelGain21.value(), accelGain22.value(),
                      accelBiasX.value(), accelBiasY.value(), accelBiasZ.value());
    }

    return result;
}

bool RefFrameRot::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr00_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr00_tmp.has_value()) { return true; }
    rfr00 = rfr00_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr01_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr01_tmp.has_value()) { return true; }
    rfr01 = rfr01_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr02_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr02_tmp.has_value()) { return true; }
    rfr02 = rfr02_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr10_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr10_tmp.has_value()) { return true; }
    rfr10 = rfr10_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr11_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr11_tmp.has_value()) { return true; }
    rfr11 = rfr11_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr12_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr12_tmp.has_value()) { return true; }
    rfr12 = rfr12_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr20_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr20_tmp.has_value()) { return true; }
    rfr20 = rfr20_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr21_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr21_tmp.has_value()) { return true; }
    rfr21 = rfr21_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rfr22_tmp = StringUtils::fromString<float>(start, end);
    if (!rfr22_tmp.has_value()) { return true; }
    rfr22 = rfr22_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage RefFrameRot::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (rfr00.has_value() && rfr01.has_value() && rfr02.has_value() && rfr10.has_value() && rfr11.has_value() && rfr12.has_value() && rfr20.has_value() &&
        rfr21.has_value() && rfr22.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f", rfr00.value(), rfr01.value(), rfr02.value(), rfr10.value(),
                      rfr11.value(), rfr12.value(), rfr20.value(), rfr21.value(), rfr22.value());
    }

    return result;
}

bool ImuMeas::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 11 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompMagX_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompMagX_tmp.has_value()) { return true; }
    uncompMagX = uncompMagX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompMagY_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompMagY_tmp.has_value()) { return true; }
    uncompMagY = uncompMagY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompMagZ_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompMagZ_tmp.has_value()) { return true; }
    uncompMagZ = uncompMagZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompAccX_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompAccX_tmp.has_value()) { return true; }
    uncompAccX = uncompAccX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompAccY_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompAccY_tmp.has_value()) { return true; }
    uncompAccY = uncompAccY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompAccZ_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompAccZ_tmp.has_value()) { return true; }
    uncompAccZ = uncompAccZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompGyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompGyroX_tmp.has_value()) { return true; }
    uncompGyroX = uncompGyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompGyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompGyroY_tmp.has_value()) { return true; }
    uncompGyroY = uncompGyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto uncompGyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!uncompGyroZ_tmp.has_value()) { return true; }
    uncompGyroZ = uncompGyroZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto temperature_tmp = StringUtils::fromString<float>(start, end);
    if (!temperature_tmp.has_value()) { return true; }
    temperature = temperature_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pressure_tmp = StringUtils::fromString<float>(start, end);
    if (!pressure_tmp.has_value()) { return true; }
    pressure = pressure_tmp.value();

    return false;  // no errors occurred
}

bool DeltaThetaVelocity::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 7 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaTime_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaTime_tmp.has_value()) { return true; }
    deltaTime = deltaTime_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaThetaX_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaThetaX_tmp.has_value()) { return true; }
    deltaThetaX = deltaThetaX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaThetaY_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaThetaY_tmp.has_value()) { return true; }
    deltaThetaY = deltaThetaY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaThetaZ_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaThetaZ_tmp.has_value()) { return true; }
    deltaThetaZ = deltaThetaZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaVelX_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaVelX_tmp.has_value()) { return true; }
    deltaVelX = deltaVelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaVelY_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaVelY_tmp.has_value()) { return true; }
    deltaVelY = deltaVelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto deltaVelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!deltaVelZ_tmp.has_value()) { return true; }
    deltaVelZ = deltaVelZ_tmp.value();

    return false;  // no errors occurred
}

bool DeltaThetaVelConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 5 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto integrationFrame_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!integrationFrame_tmp.has_value()) { return true; }
    integrationFrame = static_cast<DeltaThetaVelConfig::IntegrationFrame>(integrationFrame_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroCompensation_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gyroCompensation_tmp.has_value()) { return true; }
    gyroCompensation = static_cast<DeltaThetaVelConfig::GyroCompensation>(gyroCompensation_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelCompensation_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!accelCompensation_tmp.has_value()) { return true; }
    accelCompensation = static_cast<DeltaThetaVelConfig::AccelCompensation>(accelCompensation_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto earthRateCompensation_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!earthRateCompensation_tmp.has_value()) { return true; }
    earthRateCompensation = static_cast<DeltaThetaVelConfig::EarthRateCompensation>(earthRateCompensation_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage DeltaThetaVelConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (integrationFrame.has_value() && gyroCompensation.has_value() && accelCompensation.has_value() && earthRateCompensation.has_value() && resv.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u", static_cast<uint8_t>(integrationFrame.value()),
                      static_cast<uint8_t>(gyroCompensation.value()), static_cast<uint8_t>(accelCompensation.value()),
                      static_cast<uint8_t>(earthRateCompensation.value()), resv.value());
    }

    return result;
}

bool GyroCal::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 12 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain00_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain00_tmp.has_value()) { return true; }
    gyroGain00 = gyroGain00_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain01_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain01_tmp.has_value()) { return true; }
    gyroGain01 = gyroGain01_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain02_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain02_tmp.has_value()) { return true; }
    gyroGain02 = gyroGain02_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain10_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain10_tmp.has_value()) { return true; }
    gyroGain10 = gyroGain10_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain11_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain11_tmp.has_value()) { return true; }
    gyroGain11 = gyroGain11_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain12_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain12_tmp.has_value()) { return true; }
    gyroGain12 = gyroGain12_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain20_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain20_tmp.has_value()) { return true; }
    gyroGain20 = gyroGain20_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain21_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain21_tmp.has_value()) { return true; }
    gyroGain21 = gyroGain21_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroGain22_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroGain22_tmp.has_value()) { return true; }
    gyroGain22 = gyroGain22_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasX_tmp.has_value()) { return true; }
    gyroBiasX = gyroBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasY_tmp.has_value()) { return true; }
    gyroBiasY = gyroBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasZ_tmp.has_value()) { return true; }
    gyroBiasZ = gyroBiasZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage GyroCal::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (gyroGain00.has_value() && gyroGain01.has_value() && gyroGain02.has_value() && gyroGain10.has_value() && gyroGain11.has_value() &&
        gyroGain12.has_value() && gyroGain20.has_value() && gyroGain21.has_value() && gyroGain22.has_value() && gyroBiasX.has_value() &&
        gyroBiasY.has_value() && gyroBiasZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", gyroGain00.value(), gyroGain01.value(), gyroGain02.value(),
                      gyroGain10.value(), gyroGain11.value(), gyroGain12.value(), gyroGain20.value(), gyroGain21.value(), gyroGain22.value(), gyroBiasX.value(),
                      gyroBiasY.value(), gyroBiasZ.value());
    }

    return result;
}

bool ImuFilterControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 10 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magWindowSize_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!magWindowSize_tmp.has_value()) { return true; }
    magWindowSize = magWindowSize_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelWindowSize_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!accelWindowSize_tmp.has_value()) { return true; }
    accelWindowSize = accelWindowSize_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroWindowSize_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!gyroWindowSize_tmp.has_value()) { return true; }
    gyroWindowSize = gyroWindowSize_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto tempWindowSize_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!tempWindowSize_tmp.has_value()) { return true; }
    tempWindowSize = tempWindowSize_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto presWindowSize_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!presWindowSize_tmp.has_value()) { return true; }
    presWindowSize = presWindowSize_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto magFilterMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!magFilterMode_tmp.has_value()) { return true; }
    magFilterMode = magFilterMode_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelFilterMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!accelFilterMode_tmp.has_value()) { return true; }
    accelFilterMode = accelFilterMode_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroFilterMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gyroFilterMode_tmp.has_value()) { return true; }
    gyroFilterMode = gyroFilterMode_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto tempFilterMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!tempFilterMode_tmp.has_value()) { return true; }
    tempFilterMode = tempFilterMode_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto presFilterMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!presFilterMode_tmp.has_value()) { return true; }
    presFilterMode = presFilterMode_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage ImuFilterControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (magWindowSize.has_value() && accelWindowSize.has_value() && gyroWindowSize.has_value() && tempWindowSize.has_value() && presWindowSize.has_value() &&
        magFilterMode.has_value() && accelFilterMode.has_value() && gyroFilterMode.has_value() && tempFilterMode.has_value() && presFilterMode.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u", magWindowSize.value(), accelWindowSize.value(),
                      gyroWindowSize.value(), tempWindowSize.value(), presWindowSize.value(), uint8_t(magFilterMode.value()), uint8_t(accelFilterMode.value()),
                      uint8_t(gyroFilterMode.value()), uint8_t(tempFilterMode.value()), uint8_t(presFilterMode.value()));
    }

    return result;
}

}  // namespace IMU

namespace INS
{
bool InsSolLla::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto timeGpsTow_tmp = StringUtils::fromString<double>(start, end);
    if (!timeGpsTow_tmp.has_value()) { return true; }
    timeGpsTow = timeGpsTow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto timeGpsWeek_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!timeGpsWeek_tmp.has_value()) { return true; }
    timeGpsWeek = timeGpsWeek_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto insStatus_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!insStatus_tmp.has_value()) { return true; }
    insStatus = insStatus_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posLat_tmp = StringUtils::fromString<double>(start, end);
    if (!posLat_tmp.has_value()) { return true; }
    posLat = posLat_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posLon_tmp = StringUtils::fromString<double>(start, end);
    if (!posLon_tmp.has_value()) { return true; }
    posLon = posLon_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posAlt_tmp = StringUtils::fromString<double>(start, end);
    if (!posAlt_tmp.has_value()) { return true; }
    posAlt = posAlt_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velN_tmp = StringUtils::fromString<float>(start, end);
    if (!velN_tmp.has_value()) { return true; }
    velN = velN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velE_tmp = StringUtils::fromString<float>(start, end);
    if (!velE_tmp.has_value()) { return true; }
    velE = velE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velD_tmp = StringUtils::fromString<float>(start, end);
    if (!velD_tmp.has_value()) { return true; }
    velD = velD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto attUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!attUncertainty_tmp.has_value()) { return true; }
    attUncertainty = attUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!posUncertainty_tmp.has_value()) { return true; }
    posUncertainty = posUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!velUncertainty_tmp.has_value()) { return true; }
    velUncertainty = velUncertainty_tmp.value();

    return false;  // no errors occurred
}

bool InsSolEcef::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto timeGpsTow_tmp = StringUtils::fromString<double>(start, end);
    if (!timeGpsTow_tmp.has_value()) { return true; }
    timeGpsTow = timeGpsTow_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto timeGpsWeek_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!timeGpsWeek_tmp.has_value()) { return true; }
    timeGpsWeek = timeGpsWeek_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto insStatus_tmp = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!insStatus_tmp.has_value()) { return true; }
    insStatus = insStatus_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEx_tmp = StringUtils::fromString<double>(start, end);
    if (!posEx_tmp.has_value()) { return true; }
    posEx = posEx_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEy_tmp = StringUtils::fromString<double>(start, end);
    if (!posEy_tmp.has_value()) { return true; }
    posEy = posEy_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEz_tmp = StringUtils::fromString<double>(start, end);
    if (!posEz_tmp.has_value()) { return true; }
    posEz = posEz_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEx_tmp = StringUtils::fromString<float>(start, end);
    if (!velEx_tmp.has_value()) { return true; }
    velEx = velEx_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEy_tmp = StringUtils::fromString<float>(start, end);
    if (!velEy_tmp.has_value()) { return true; }
    velEy = velEy_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEz_tmp = StringUtils::fromString<float>(start, end);
    if (!velEz_tmp.has_value()) { return true; }
    velEz = velEz_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto attUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!attUncertainty_tmp.has_value()) { return true; }
    attUncertainty = attUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!posUncertainty_tmp.has_value()) { return true; }
    posUncertainty = posUncertainty_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velUncertainty_tmp = StringUtils::fromString<float>(start, end);
    if (!velUncertainty_tmp.has_value()) { return true; }
    velUncertainty = velUncertainty_tmp.value();

    return false;  // no errors occurred
}

bool InsBasicConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 4 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto scenario_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!scenario_tmp.has_value()) { return true; }
    scenario = static_cast<InsBasicConfig::Scenario>(scenario_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto ahrsAiding_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!ahrsAiding_tmp.has_value()) { return true; }
    ahrsAiding = static_cast<InsBasicConfig::AhrsAiding>(ahrsAiding_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto estBaseline_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!estBaseline_tmp.has_value()) { return true; }
    estBaseline = static_cast<InsBasicConfig::EstBaseline>(estBaseline_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage InsBasicConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (scenario.has_value() && ahrsAiding.has_value() && estBaseline.has_value() && resv.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u", static_cast<uint8_t>(scenario.value()), static_cast<uint8_t>(ahrsAiding.value()),
                      static_cast<uint8_t>(estBaseline.value()), resv.value());
    }

    return result;
}

bool InsStateLla::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posLat_tmp = StringUtils::fromString<double>(start, end);
    if (!posLat_tmp.has_value()) { return true; }
    posLat = posLat_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posLon_tmp = StringUtils::fromString<double>(start, end);
    if (!posLon_tmp.has_value()) { return true; }
    posLon = posLon_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posAlt_tmp = StringUtils::fromString<double>(start, end);
    if (!posAlt_tmp.has_value()) { return true; }
    posAlt = posAlt_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velN_tmp = StringUtils::fromString<float>(start, end);
    if (!velN_tmp.has_value()) { return true; }
    velN = velN_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velE_tmp = StringUtils::fromString<float>(start, end);
    if (!velE_tmp.has_value()) { return true; }
    velE = velE_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velD_tmp = StringUtils::fromString<float>(start, end);
    if (!velD_tmp.has_value()) { return true; }
    velD = velD_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool InsStateEcef::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 15 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto yaw_tmp = StringUtils::fromString<float>(start, end);
    if (!yaw_tmp.has_value()) { return true; }
    yaw = yaw_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto pitch_tmp = StringUtils::fromString<float>(start, end);
    if (!pitch_tmp.has_value()) { return true; }
    pitch = pitch_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto roll_tmp = StringUtils::fromString<float>(start, end);
    if (!roll_tmp.has_value()) { return true; }
    roll = roll_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEx_tmp = StringUtils::fromString<double>(start, end);
    if (!posEx_tmp.has_value()) { return true; }
    posEx = posEx_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEy_tmp = StringUtils::fromString<double>(start, end);
    if (!posEy_tmp.has_value()) { return true; }
    posEy = posEy_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto posEz_tmp = StringUtils::fromString<double>(start, end);
    if (!posEz_tmp.has_value()) { return true; }
    posEz = posEz_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEx_tmp = StringUtils::fromString<float>(start, end);
    if (!velEx_tmp.has_value()) { return true; }
    velEx = velEx_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEy_tmp = StringUtils::fromString<float>(start, end);
    if (!velEy_tmp.has_value()) { return true; }
    velEy = velEy_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velEz_tmp = StringUtils::fromString<float>(start, end);
    if (!velEz_tmp.has_value()) { return true; }
    velEz = velEz_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelX_tmp.has_value()) { return true; }
    accelX = accelX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelY_tmp.has_value()) { return true; }
    accelY = accelY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelZ_tmp.has_value()) { return true; }
    accelZ = accelZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroX_tmp.has_value()) { return true; }
    gyroX = gyroX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroY_tmp.has_value()) { return true; }
    gyroY = gyroY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroZ_tmp.has_value()) { return true; }
    gyroZ = gyroZ_tmp.value();

    return false;  // no errors occurred
}

bool FilterStartupBias::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 7 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasX_tmp.has_value()) { return true; }
    gyroBiasX = gyroBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasY_tmp.has_value()) { return true; }
    gyroBiasY = gyroBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gyroBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!gyroBiasZ_tmp.has_value()) { return true; }
    gyroBiasZ = gyroBiasZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasX_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasX_tmp.has_value()) { return true; }
    accelBiasX = accelBiasX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasY_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasY_tmp.has_value()) { return true; }
    accelBiasY = accelBiasY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto accelBiasZ_tmp = StringUtils::fromString<float>(start, end);
    if (!accelBiasZ_tmp.has_value()) { return true; }
    accelBiasZ = accelBiasZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto presBias_tmp = StringUtils::fromString<float>(start, end);
    if (!presBias_tmp.has_value()) { return true; }
    presBias = presBias_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage FilterStartupBias::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (gyroBiasX.has_value() && gyroBiasY.has_value() && gyroBiasZ.has_value() && accelBiasX.has_value() && accelBiasY.has_value() && accelBiasZ.has_value() &&
        presBias.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f,%f", gyroBiasX.value(), gyroBiasY.value(), gyroBiasZ.value(), accelBiasX.value(),
                      accelBiasY.value(), accelBiasZ.value(), presBias.value());
    }

    return result;
}

bool InsRefOffset::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 6 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refOffsetX_tmp = StringUtils::fromString<float>(start, end);
    if (!refOffsetX_tmp.has_value()) { return true; }
    refOffsetX = refOffsetX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refOffsetY_tmp = StringUtils::fromString<float>(start, end);
    if (!refOffsetY_tmp.has_value()) { return true; }
    refOffsetY = refOffsetY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refOffsetZ_tmp = StringUtils::fromString<float>(start, end);
    if (!refOffsetZ_tmp.has_value()) { return true; }
    refOffsetZ = refOffsetZ_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refUncertX_tmp = StringUtils::fromString<float>(start, end);
    if (!refUncertX_tmp.has_value()) { return true; }
    refUncertX = refUncertX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refUncertY_tmp = StringUtils::fromString<float>(start, end);
    if (!refUncertY_tmp.has_value()) { return true; }
    refUncertY = refUncertY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto refUncertZ_tmp = StringUtils::fromString<float>(start, end);
    if (!refUncertZ_tmp.has_value()) { return true; }
    refUncertZ = refUncertZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage InsRefOffset::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (refOffsetX.has_value() && refOffsetY.has_value() && refOffsetZ.has_value() && refUncertX.has_value() && refUncertY.has_value() &&
        refUncertZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f,%f,%f,%f", refOffsetX.value(), refOffsetY.value(), refOffsetZ.value(), refUncertX.value(),
                      refUncertY.value(), refUncertZ.value());
    }

    return result;
}

bool InsGnssSelect::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 6 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto activeReceiverSelect_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!activeReceiverSelect_tmp.has_value()) { return true; }
    activeReceiverSelect = static_cast<InsGnssSelect::ActiveReceiverSelect>(activeReceiverSelect_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto usedForNavTime_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!usedForNavTime_tmp.has_value()) { return true; }
    usedForNavTime = usedForNavTime_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto hysteresisTime_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!hysteresisTime_tmp.has_value()) { return true; }
    hysteresisTime = hysteresisTime_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto useGnssCompass_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!useGnssCompass_tmp.has_value()) { return true; }
    useGnssCompass = static_cast<InsGnssSelect::UseGnssCompass>(useGnssCompass_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv1_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv1_tmp.has_value()) { return true; }
    resv1 = resv1_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv2_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv2_tmp.has_value()) { return true; }
    resv2 = resv2_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage InsGnssSelect::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (activeReceiverSelect.has_value() && usedForNavTime.has_value() && hysteresisTime.has_value() && useGnssCompass.has_value() && resv1.has_value() &&
        resv2.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%u", static_cast<uint8_t>(activeReceiverSelect.value()), usedForNavTime.value(),
                      hysteresisTime.value(), static_cast<uint8_t>(useGnssCompass.value()), resv1.value(), resv2.value());
    }

    return result;
}

}  // namespace INS

namespace System
{
bool UserTag::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 1 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    tag = AsciiMessage(start, end);

    return false;  // no errors occurred
}

AsciiMessage UserTag::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (tag.has_value()) { std::snprintf(result.begin(), result.capacity(), "%s", tag.value().c_str()); }
    return result;
}

bool Model::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 1 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    model = AsciiMessage(start, end);

    return false;  // no errors occurred
}

bool HwVer::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 2 + 1)
    {
        // This register may not have an optional field
        if (tokens.size() != 1 + 1) { return true; }
    }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto hwVer_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!hwVer_tmp.has_value()) { return true; }
    hwVer = hwVer_tmp.value();

    if (tokens.size() == (2 + 1))
    {
        // This is an optional parameter.
        start = tokens[index++] + 1;
        end = tokens[index];
        const auto hwMinVer_tmp = StringUtils::fromString<uint32_t>(start, end);
        if (!hwMinVer_tmp.has_value()) { return true; }
        hwMinVer = hwMinVer_tmp.value();
    }

    return false;  // no errors occurred
}

bool Serial::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 1 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto serialNum_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!serialNum_tmp.has_value()) { return true; }
    serialNum = serialNum_tmp.value();

    return false;  // no errors occurred
}

bool FwVer::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 1 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    fwVer = AsciiMessage(start, end);

    return false;  // no errors occurred
}

bool BaudRate::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 2 + 1)
    {
        // This register may not have an optional field
        if (tokens.size() != 1 + 1) { return true; }
    }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto baudRate_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!baudRate_tmp.has_value()) { return true; }
    baudRate = static_cast<BaudRate::BaudRates>(baudRate_tmp.value());

    if (tokens.size() == (2 + 1))
    {
        // This is an optional parameter.
        start = tokens[index++] + 1;
        end = tokens[index];
        const auto serialPort_tmp = StringUtils::fromString<uint8_t>(start, end);
        if (!serialPort_tmp.has_value()) { return true; }
        serialPort = static_cast<BaudRate::SerialPort>(serialPort_tmp.value());
    }

    return false;  // no errors occurred
}

AsciiMessage BaudRate::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (baudRate.has_value())
    {
        if (serialPort == SerialPort::Poll) { std::snprintf(result.begin(), result.capacity(), "%u,?", static_cast<uint32_t>(baudRate.value())); }
        else if (serialPort == SerialPort::ActiveSerial) { std::snprintf(result.begin(), result.capacity(), "%u", static_cast<uint32_t>(baudRate.value())); }
        else { std::snprintf(result.begin(), result.capacity(), "%u,%u", static_cast<uint32_t>(baudRate.value()), static_cast<uint8_t>(serialPort)); }
    }
    return result;
}

GenericCommand BaudRate::toReadCommand()
{
    AsciiMessage commandString;
    if (serialPort == SerialPort::Poll) { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,?", _id); }
    else { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,%1d", _id, static_cast<uint8_t>(serialPort)); }

    GenericCommand readCommand(commandString, 6);
    return readCommand;
}

bool AsyncOutputType::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 2 + 1)
    {
        // This register may not have an optional field
        if (tokens.size() != 1 + 1) { return true; }
    }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto ador_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!ador_tmp.has_value()) { return true; }
    ador = static_cast<AsyncOutputType::Ador>(ador_tmp.value());

    if (tokens.size() == (2 + 1))
    {
        // This is an optional parameter.
        start = tokens[index++] + 1;
        end = tokens[index];
        const auto serialPort_tmp = StringUtils::fromString<uint8_t>(start, end);
        if (!serialPort_tmp.has_value()) { return true; }
        serialPort = static_cast<AsyncOutputType::SerialPort>(serialPort_tmp.value());
    }

    return false;  // no errors occurred
}

AsciiMessage AsyncOutputType::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (ador.has_value())
    {
        if (serialPort == SerialPort::Poll) { std::snprintf(result.begin(), result.capacity(), "%u,?", static_cast<uint32_t>(ador.value())); }
        else if (serialPort == SerialPort::ActiveSerial) { std::snprintf(result.begin(), result.capacity(), "%u", static_cast<uint32_t>(ador.value())); }
        else { std::snprintf(result.begin(), result.capacity(), "%u,%u", static_cast<uint32_t>(ador.value()), static_cast<uint8_t>(serialPort)); }
    }
    return result;
}

GenericCommand AsyncOutputType::toReadCommand()
{
    AsciiMessage commandString;
    if (serialPort == SerialPort::Poll) { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,?", _id); }
    else { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,%1d", _id, static_cast<uint8_t>(serialPort)); }

    GenericCommand readCommand(commandString, 6);
    return readCommand;
}

bool AsyncOutputFreq::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 2 + 1)
    {
        // This register may not have an optional field
        if (tokens.size() != 1 + 1) { return true; }
    }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto adof_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!adof_tmp.has_value()) { return true; }
    adof = static_cast<AsyncOutputFreq::Adof>(adof_tmp.value());

    if (tokens.size() == (2 + 1))
    {
        // This is an optional parameter.
        start = tokens[index++] + 1;
        end = tokens[index];
        const auto serialPort_tmp = StringUtils::fromString<uint8_t>(start, end);
        if (!serialPort_tmp.has_value()) { return true; }
        serialPort = static_cast<AsyncOutputFreq::SerialPort>(serialPort_tmp.value());
    }

    return false;  // no errors occurred
}

AsciiMessage AsyncOutputFreq::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (adof.has_value())
    {
        if (serialPort == SerialPort::Poll) { std::snprintf(result.begin(), result.capacity(), "%u,?", static_cast<uint32_t>(adof.value())); }
        else if (serialPort == SerialPort::ActiveSerial) { std::snprintf(result.begin(), result.capacity(), "%u", static_cast<uint32_t>(adof.value())); }
        else { std::snprintf(result.begin(), result.capacity(), "%u,%u", static_cast<uint32_t>(adof.value()), static_cast<uint8_t>(serialPort)); }
    }
    return result;
}

GenericCommand AsyncOutputFreq::toReadCommand()
{
    AsciiMessage commandString;
    if (serialPort == SerialPort::Poll) { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,?", _id); }
    else { std::snprintf(commandString.begin(), commandString.capacity(), "RRG,%02d,%1d", _id, static_cast<uint8_t>(serialPort)); }

    GenericCommand readCommand(commandString, 6);
    return readCommand;
}

bool ProtocolControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 7 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto asciiAppendCount_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!asciiAppendCount_tmp.has_value()) { return true; }
    asciiAppendCount = static_cast<ProtocolControl::AsciiAppendCount>(asciiAppendCount_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto asciiAppendStatus_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!asciiAppendStatus_tmp.has_value()) { return true; }
    asciiAppendStatus = static_cast<ProtocolControl::AsciiAppendStatus>(asciiAppendStatus_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto spiAppendCount_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!spiAppendCount_tmp.has_value()) { return true; }
    spiAppendCount = static_cast<ProtocolControl::SpiAppendCount>(spiAppendCount_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto spiAppendStatus_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!spiAppendStatus_tmp.has_value()) { return true; }
    spiAppendStatus = static_cast<ProtocolControl::SpiAppendStatus>(spiAppendStatus_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto asciiChecksum_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!asciiChecksum_tmp.has_value()) { return true; }
    asciiChecksum = static_cast<ProtocolControl::AsciiChecksum>(asciiChecksum_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto spiChecksum_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!spiChecksum_tmp.has_value()) { return true; }
    spiChecksum = static_cast<ProtocolControl::SpiChecksum>(spiChecksum_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto errorMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!errorMode_tmp.has_value()) { return true; }
    errorMode = static_cast<ProtocolControl::ErrorMode>(errorMode_tmp.value());

    return false;  // no errors occurred
}

AsciiMessage ProtocolControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (asciiAppendCount.has_value() && asciiAppendStatus.has_value() && spiAppendCount.has_value() && spiAppendStatus.has_value() &&
        asciiChecksum.has_value() && spiChecksum.has_value() && errorMode.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%u,%u", static_cast<uint8_t>(asciiAppendCount.value()),
                      static_cast<uint8_t>(asciiAppendStatus.value()), static_cast<uint8_t>(spiAppendCount.value()),
                      static_cast<uint8_t>(spiAppendStatus.value()), static_cast<uint8_t>(asciiChecksum.value()), static_cast<uint8_t>(spiChecksum.value()),
                      static_cast<uint8_t>(errorMode.value()));
    }

    return result;
}

bool SyncControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncInMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!syncInMode_tmp.has_value()) { return true; }
    syncInMode = static_cast<SyncControl::SyncInMode>(syncInMode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncInEdge_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!syncInEdge_tmp.has_value()) { return true; }
    syncInEdge = static_cast<SyncControl::SyncInEdge>(syncInEdge_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncInSkipFactor_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!syncInSkipFactor_tmp.has_value()) { return true; }
    syncInSkipFactor = syncInSkipFactor_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv1_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!resv1_tmp.has_value()) { return true; }
    resv1 = resv1_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncOutMode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!syncOutMode_tmp.has_value()) { return true; }
    syncOutMode = static_cast<SyncControl::SyncOutMode>(syncOutMode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncOutPolarity_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!syncOutPolarity_tmp.has_value()) { return true; }
    syncOutPolarity = static_cast<SyncControl::SyncOutPolarity>(syncOutPolarity_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncOutSkipFactor_tmp = StringUtils::fromString<uint16_t>(start, end);
    if (!syncOutSkipFactor_tmp.has_value()) { return true; }
    syncOutSkipFactor = syncOutSkipFactor_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncOutPulseWidth_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!syncOutPulseWidth_tmp.has_value()) { return true; }
    syncOutPulseWidth = syncOutPulseWidth_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv2_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!resv2_tmp.has_value()) { return true; }
    resv2 = resv2_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage SyncControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (syncInMode.has_value() && syncInEdge.has_value() && syncInSkipFactor.has_value() && resv1.has_value() && syncOutMode.has_value() &&
        syncOutPolarity.has_value() && syncOutSkipFactor.has_value() && syncOutPulseWidth.has_value() && resv2.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%u,%u,%u,%u", static_cast<uint8_t>(syncInMode.value()),
                      static_cast<uint8_t>(syncInEdge.value()), syncInSkipFactor.value(), resv1.value(), static_cast<uint8_t>(syncOutMode.value()),
                      static_cast<uint8_t>(syncOutPolarity.value()), syncOutSkipFactor.value(), syncOutPulseWidth.value(), resv2.value());
    }

    return result;
}

bool SyncStatus::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncInCount_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!syncInCount_tmp.has_value()) { return true; }
    syncInCount = syncInCount_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncInTime_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!syncInTime_tmp.has_value()) { return true; }
    syncInTime = syncInTime_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto syncOutCount_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!syncOutCount_tmp.has_value()) { return true; }
    syncOutCount = syncOutCount_tmp.value();

    return false;  // no errors occurred
}

bool BinaryOutput::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    std::optional<uint16_t> tmpRet = StringUtils::fromStringHex<uint16_t>(start, end);
    if (!tmpRet) { return true; }
    asyncMode = *tmpRet;

    start = tokens[index++] + 1;
    end = tokens[index];
    tmpRet = StringUtils::fromString<uint16_t>(start, end);
    if (!tmpRet) { return true; }
    rateDivisor = *tmpRet;

    start = tokens[index++] + 1;
    end = tokens[index];
    tmpRet = StringUtils::fromStringHex<uint8_t>(start, end);
    if (!tmpRet) { return true; }
    uint16_t group = *tmpRet;

    if (group & 0x80)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        tmpRet = StringUtils::fromStringHex<uint8_t>(start, end);
        if (!tmpRet) { return true; }
        group |= *tmpRet << 8;
    }

    if (group & COMMON_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t common_ret = 0;
        auto common_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!common_type.has_value()) { return true; }
        common_ret = common_type.value();
        if (common_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto common_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!common_type_ext.has_value()) { return true; }
            common_ret |= common_type_ext.value() << 16;
        }
        this->common = common_ret;
    }

    if (group & TIME_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t time_ret = 0;
        auto time_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!time_type.has_value()) { return true; }
        time_ret = time_type.value();
        if (time_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto time_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!time_type_ext.has_value()) { return true; }
            time_ret |= time_type_ext.value() << 16;
        }
        this->time = time_ret;
    }

    if (group & IMU_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t imu_ret = 0;
        auto imu_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!imu_type.has_value()) { return true; }
        imu_ret = imu_type.value();
        if (imu_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto imu_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!imu_type_ext.has_value()) { return true; }
            imu_ret |= imu_type_ext.value() << 16;
        }
        this->imu = imu_ret;
    }

    if (group & GNSS_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t gnss_ret = 0;
        auto gnss_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!gnss_type.has_value()) { return true; }
        gnss_ret = gnss_type.value();
        if (gnss_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto gnss_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!gnss_type_ext.has_value()) { return true; }
            gnss_ret |= gnss_type_ext.value() << 16;
        }
        this->gnss = gnss_ret;
    }

    if (group & ATTITUDE_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t attitude_ret = 0;
        auto attitude_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!attitude_type.has_value()) { return true; }
        attitude_ret = attitude_type.value();
        if (attitude_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto attitude_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!attitude_type_ext.has_value()) { return true; }
            attitude_ret |= attitude_type_ext.value() << 16;
        }
        this->attitude = attitude_ret;
    }

    if (group & INS_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t ins_ret = 0;
        auto ins_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!ins_type.has_value()) { return true; }
        ins_ret = ins_type.value();
        if (ins_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto ins_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!ins_type_ext.has_value()) { return true; }
            ins_ret |= ins_type_ext.value() << 16;
        }
        this->ins = ins_ret;
    }

    if (group & GNSS2_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t gnss2_ret = 0;
        auto gnss2_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!gnss2_type.has_value()) { return true; }
        gnss2_ret = gnss2_type.value();
        if (gnss2_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto gnss2_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!gnss2_type_ext.has_value()) { return true; }
            gnss2_ret |= gnss2_type_ext.value() << 16;
        }
        this->gnss2 = gnss2_ret;
    }

    if (group & GNSS3_BIT)
    {
        start = tokens[index++] + 1;
        end = tokens[index];
        uint32_t gnss3_ret = 0;
        auto gnss3_type = StringUtils::fromStringHex<uint16_t>(start, end);
        if (!gnss3_type.has_value()) { return true; }
        gnss3_ret = gnss3_type.value();
        if (gnss3_ret & 0x8000)
        {
            start = tokens[index++] + 1;
            end = tokens[index];
            auto gnss3_type_ext = StringUtils::fromStringHex<uint16_t>(start, end);
            if (!gnss3_type_ext.has_value()) { return true; }
            gnss3_ret |= gnss3_type_ext.value() << 16;
        }
        this->gnss3 = gnss3_ret;
    }

    return false;  // no errors occurred
}

AsciiMessage BinaryOutput::toString() const
{
    AsciiMessage result = "";
    BinaryHeader binaryHeader = toBinaryHeader();
    if (asyncMode.has_value() && rateDivisor.has_value())
    {
        std::snprintf(result.end(), result.numAvailable(), "%1X,%d%s", uint16_t(asyncMode.value()), rateDivisor.value(),
                      binaryHeaderToString<AsciiMessage::capacity()>(binaryHeader).c_str());
    }
    return result;
}

BinaryHeader BinaryOutputMeasurements::toBinaryHeader() const noexcept
{
    BinaryHeader result;
    uint8_t group = 0;
    uint8_t groupExt = 0;
    if (uint32_t(this->common) > 0) group += COMMON_BIT;
    if (uint32_t(this->time) > 0) group += TIME_BIT;
    if (uint32_t(this->imu) > 0) group += IMU_BIT;
    if (uint32_t(this->gnss) > 0) group += GNSS_BIT;
    if (uint32_t(this->attitude) > 0) group += ATTITUDE_BIT;
    if (uint32_t(this->ins) > 0) group += INS_BIT;
    if (uint32_t(this->gnss2) > 0) group += GNSS2_BIT;
    if (uint32_t(this->gnss3) > 0) groupExt += (GNSS3_BIT >> 8);
    if (uint32_t(this->gnss3) > 0) group |= 0x80;
    result.outputGroups.push_back(group);
    if (groupExt > 0) result.outputGroups.push_back(groupExt);

    if (uint32_t(this->common) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->common));
        if ((uint32_t(this->common) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->common) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->time) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->time));
        if ((uint32_t(this->time) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->time) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->imu) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->imu));
        if ((uint32_t(this->imu) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->imu) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->gnss) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->gnss));
        if ((uint32_t(this->gnss) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->gnss) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->attitude) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->attitude));
        if ((uint32_t(this->attitude) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->attitude) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->ins) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->ins));
        if ((uint32_t(this->ins) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->ins) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->gnss2) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->gnss2));
        if ((uint32_t(this->gnss2) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->gnss2) & 0xFFFF0000) >> 16);
        }
    }

    if (uint32_t(this->gnss3) > 0)
    {
        result.outputTypes.push_back(uint32_t(this->gnss3));
        if ((uint32_t(this->gnss3) & 0xFFFF0000) > 0)
        {
            result.outputTypes.back() |= 0x8000;  // Set extension bit
            result.outputTypes.push_back((uint32_t(this->gnss3) & 0xFFFF0000) >> 16);
        }
    }

    return result;
}

Vector<uint8_t, binaryHeaderMaxLength> BinaryOutputMeasurements::toHeaderBytes() const noexcept { return this->toBinaryHeader().toHeaderBytes(); }

bool NmeaOutput1::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 5 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto port_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!port_tmp.has_value()) { return true; }
    port = static_cast<NmeaOutput1::Port>(port_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rate_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!rate_tmp.has_value()) { return true; }
    rate = static_cast<NmeaOutput1::Rate>(rate_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto mode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!mode_tmp.has_value()) { return true; }
    mode = static_cast<NmeaOutput1::Mode>(mode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnssSelect_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnssSelect_tmp.has_value()) { return true; }
    gnssSelect = static_cast<NmeaOutput1::GnssSelect>(gnssSelect_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto msgSelection_tmp = StringUtils::fromStringHex<uint32_t>(start, end);
    if (!msgSelection_tmp.has_value()) { return true; }
    msgSelection = msgSelection_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage NmeaOutput1::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (port.has_value() && rate.has_value() && mode.has_value() && gnssSelect.has_value() && msgSelection.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%X", static_cast<uint8_t>(port.value()), static_cast<uint8_t>(rate.value()),
                      static_cast<uint8_t>(mode.value()), static_cast<uint8_t>(gnssSelect.value()), uint32_t(msgSelection.value()));
    }

    return result;
}

bool NmeaOutput2::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 5 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto port_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!port_tmp.has_value()) { return true; }
    port = static_cast<NmeaOutput2::Port>(port_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto rate_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!rate_tmp.has_value()) { return true; }
    rate = static_cast<NmeaOutput2::Rate>(rate_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto mode_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!mode_tmp.has_value()) { return true; }
    mode = static_cast<NmeaOutput2::Mode>(mode_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnssSelect_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnssSelect_tmp.has_value()) { return true; }
    gnssSelect = static_cast<NmeaOutput2::GnssSelect>(gnssSelect_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto msgSelection_tmp = StringUtils::fromStringHex<uint32_t>(start, end);
    if (!msgSelection_tmp.has_value()) { return true; }
    msgSelection = msgSelection_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage NmeaOutput2::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (port.has_value() && rate.has_value() && mode.has_value() && gnssSelect.has_value() && msgSelection.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%X", static_cast<uint8_t>(port.value()), static_cast<uint8_t>(rate.value()),
                      static_cast<uint8_t>(mode.value()), static_cast<uint8_t>(gnssSelect.value()), uint32_t(msgSelection.value()));
    }

    return result;
}

bool LegacyCompatibilitySettings::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 4 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto insLegacy_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!insLegacy_tmp.has_value()) { return true; }
    insLegacy = static_cast<LegacyCompatibilitySettings::InsLegacy>(insLegacy_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto gnssLegacy_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!gnssLegacy_tmp.has_value()) { return true; }
    gnssLegacy = gnssLegacy_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto imuLegacy_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!imuLegacy_tmp.has_value()) { return true; }
    imuLegacy = static_cast<LegacyCompatibilitySettings::ImuLegacy>(imuLegacy_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto hwLegacy_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!hwLegacy_tmp.has_value()) { return true; }
    hwLegacy = static_cast<LegacyCompatibilitySettings::HwLegacy>(hwLegacy_tmp.value());

    return false;  // no errors occurred
}

AsciiMessage LegacyCompatibilitySettings::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (insLegacy.has_value() && gnssLegacy.has_value() && imuLegacy.has_value() && hwLegacy.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u", static_cast<uint8_t>(insLegacy.value()), uint8_t(gnssLegacy.value()),
                      static_cast<uint8_t>(imuLegacy.value()), static_cast<uint8_t>(hwLegacy.value()));
    }

    return result;
}

}  // namespace System

namespace VelocityAiding
{
bool VelAidingMeas::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velocityX_tmp = StringUtils::fromString<float>(start, end);
    if (!velocityX_tmp.has_value()) { return true; }
    velocityX = velocityX_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velocityY_tmp = StringUtils::fromString<float>(start, end);
    if (!velocityY_tmp.has_value()) { return true; }
    velocityY = velocityY_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velocityZ_tmp = StringUtils::fromString<float>(start, end);
    if (!velocityZ_tmp.has_value()) { return true; }
    velocityZ = velocityZ_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage VelAidingMeas::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (velocityX.has_value() && velocityY.has_value() && velocityZ.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%f,%f,%f", velocityX.value(), velocityY.value(), velocityZ.value());
    }

    return result;
}

bool VelAidingControl::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 3 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velAidEnable_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!velAidEnable_tmp.has_value()) { return true; }
    velAidEnable = static_cast<VelAidingControl::VelAidEnable>(velAidEnable_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto velUncertTuning_tmp = StringUtils::fromString<float>(start, end);
    if (!velUncertTuning_tmp.has_value()) { return true; }
    velUncertTuning = velUncertTuning_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv_tmp = StringUtils::fromString<float>(start, end);
    if (!resv_tmp.has_value()) { return true; }
    resv = resv_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage VelAidingControl::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (velAidEnable.has_value() && velUncertTuning.has_value() && resv.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%f,%f", static_cast<uint8_t>(velAidEnable.value()), velUncertTuning.value(), resv.value());
    }

    return result;
}

}  // namespace VelocityAiding

namespace WorldMagGravityModel
{
bool RefModelConfig::fromString(const AsciiMessage& response)
{
    const auto tokens = findIndexOfFieldSeparators(response);
    if (tokens.size() != 9 + 1) { return true; }

    int index = 0;
    const char* start;
    const char* end;

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto enableMagModel_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!enableMagModel_tmp.has_value()) { return true; }
    enableMagModel = static_cast<RefModelConfig::EnableMagModel>(enableMagModel_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto enableGravityModel_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!enableGravityModel_tmp.has_value()) { return true; }
    enableGravityModel = static_cast<RefModelConfig::EnableGravityModel>(enableGravityModel_tmp.value());

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv1_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv1_tmp.has_value()) { return true; }
    resv1 = resv1_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto resv2_tmp = StringUtils::fromString<uint8_t>(start, end);
    if (!resv2_tmp.has_value()) { return true; }
    resv2 = resv2_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto recalcThreshold_tmp = StringUtils::fromString<uint32_t>(start, end);
    if (!recalcThreshold_tmp.has_value()) { return true; }
    recalcThreshold = recalcThreshold_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto year_tmp = StringUtils::fromString<float>(start, end);
    if (!year_tmp.has_value()) { return true; }
    year = year_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto latitude_tmp = StringUtils::fromString<double>(start, end);
    if (!latitude_tmp.has_value()) { return true; }
    latitude = latitude_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto longitude_tmp = StringUtils::fromString<double>(start, end);
    if (!longitude_tmp.has_value()) { return true; }
    longitude = longitude_tmp.value();

    start = tokens[index++] + 1;
    end = tokens[index];
    const auto altitude_tmp = StringUtils::fromString<double>(start, end);
    if (!altitude_tmp.has_value()) { return true; }
    altitude = altitude_tmp.value();

    return false;  // no errors occurred
}

AsciiMessage RefModelConfig::toString() const
{
    AsciiMessage result = "";

    // verify that all fields have a value set
    if (enableMagModel.has_value() && enableGravityModel.has_value() && resv1.has_value() && resv2.has_value() && recalcThreshold.has_value() &&
        year.has_value() && latitude.has_value() && longitude.has_value() && altitude.has_value())
    {
        std::snprintf(result.begin(), result.capacity(), "%u,%u,%u,%u,%u,%f,%f,%f,%f", static_cast<uint8_t>(enableMagModel.value()),
                      static_cast<uint8_t>(enableGravityModel.value()), resv1.value(), resv2.value(), recalcThreshold.value(), year.value(), latitude.value(),
                      longitude.value(), altitude.value());
    }

    return result;
}

}  // namespace WorldMagGravityModel

Vector<const char*, Config::PacketFinders::asciiMaxFieldCount> findIndexOfFieldSeparators(const AsciiMessage& input)
{
    Vector<const char*, Config::PacketFinders::asciiMaxFieldCount> indexes;
    size_t offset = 7;
    if (input[0] != '$')
    {
        indexes.push_back(input.begin() - 1);
        offset = 0;
    }

    for (size_t i = input.find(',', offset); i < input.length(); i++)
    {
        if (input[i] == ',' || input[i] == '*') { indexes.push_back(&input[i]); }
    }

    if (input[0] != '$') { indexes.push_back(input.end()); }

    return indexes;
}

}  // namespace Registers
}  // namespace VN
