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


#include "vectornav/ExporterCsvUtils.hpp"

#include "vectornav/Exporter.hpp"
#include "vectornav/Implementation/AsciiPacketProtocol.hpp"
#include "vectornav/Implementation/FaPacketProtocol.hpp"

namespace VN
{
namespace DataExport
{
std::array<std::array<CsvTypeInfo, 19>, 13> dataTypes{{
    {CsvTypeInfo{U64, 1},
     {U64, 1},
     {U64, 1},
     {FLO, 3},
     {FLO, 4},
     {FLO, 3},
     {DUB, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 6},
     {FLO, 5},
     {FLO, 7},
     {U16, 1},
     {U32, 1},
     {U64, 1},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0}},  // COMMON
    {CsvTypeInfo{U64, 1},
     {U64, 1},
     {U64, 1},
     {U16, 1},
     {U64, 1},
     {U64, 1},
     {UTC, 1},
     {U32, 1},
     {U32, 1},
     {U8, 1},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0}},  // TIME
    {CsvTypeInfo{U16, 1},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 1},
     {FLO, 1},
     {FLO, 4},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {U16, 1},
     {UNK, 40},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0}},  // IMU
    {CsvTypeInfo{UTC, 1},
     {U64, 1},
     {U16, 1},
     {U8, 1},
     {U8, 1},
     {DUB, 3},
     {DUB, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 1},
     {FLO, 1},
     {U16, 1},
     {FLO, 7},
     {SAT, 1},
     {NON, 0},
     {RAW, 1},
     {U16, 1},
     {DUB, 1}},  // GNSS
    {CsvTypeInfo{U16, 1},
     {FLO, 3},
     {FLO, 4},
     {FLO, 9},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {UNK, 12},
     {UNK, 28},
     {UNK, 24},
     {FLO, 3},
     {FLO, 1},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0}},  // ATTITUDE
    {CsvTypeInfo{U16, 1},
     {DUB, 3},
     {DUB, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 1},
     {FLO, 1},
     {UNK, 68},
     {UNK, 64},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0},
     {NON, 0}},  // INS
    {CsvTypeInfo{UTC, 1},
     {U64, 1},
     {U16, 1},
     {U8, 1},
     {U8, 1},
     {DUB, 3},
     {DUB, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 1},
     {FLO, 1},
     {U16, 1},
     {FLO, 7},
     {SAT, 1},
     {NON, 0},
     {RAW, 1},
     {U16, 1},
     {DUB, 1}},  // GNSS2
    {},
    {},
    {},
    {},
    {},
    {CsvTypeInfo{UTC, 1},
     {U64, 1},
     {U16, 1},
     {U8, 1},
     {U8, 1},
     {DUB, 3},
     {DUB, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 3},
     {FLO, 1},
     {FLO, 1},
     {U16, 1},
     {FLO, 7},
     {SAT, 1},
     {NON, 0},
     {RAW, 1},
     {U16, 1},
     {DUB, 1}},  // GNSS3
}};

CsvTypeInfo csvTypeLookup(size_t group, size_t field) { return dataTypes[group][field]; }

int getMeasurementString(FaPacketExtractor& packet, const CsvTypeInfo& typeInfo, char* ptr, const uint16_t remaining)
{
    switch (typeInfo.type)
    {
        case U8:
        {
            return extractToString<uint8_t>(packet, typeInfo.len, ptr, remaining);
        }
        case U16:
        {
            return extractToString<uint16_t>(packet, typeInfo.len, ptr, remaining);
        }
        case U32:
        {
            return extractToString<uint32_t>(packet, typeInfo.len, ptr, remaining);
        }
        case U64:
        {
            return extractToString<uint64_t>(packet, typeInfo.len, ptr, remaining);
        }
        case FLO:
        {
            return extractToString<float>(packet, typeInfo.len, ptr, remaining);
        }
        case DUB:
        {
            return extractToString<double>(packet, typeInfo.len, ptr, remaining);
        }
        case UTC:
        {
            int offset = 0;
            offset += extractToString<int8_t>(packet, 1, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<uint8_t>(packet, 5, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<uint16_t>(packet, 1, ptr + offset, remaining - offset);
            return offset;
        }
        case SAT:
        {
            int offset = 0;
            offset += extractToString<uint8_t>(packet, 5, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<int8_t>(packet, 1, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<uint16_t>(packet, 1, ptr + offset, remaining - offset);
            return offset;
        }
        case RAW:
        {
            int offset = 0;
            offset += extractToString<uint8_t>(packet, 4, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<int8_t>(packet, 1, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<uint8_t>(packet, 1, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<uint16_t>(packet, 1, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<double>(packet, 2, ptr + offset, remaining - offset);
            offset += snprintf(ptr + offset, remaining - offset, ",");
            offset += extractToString<float>(packet, 1, ptr + offset, remaining - offset);

            return offset;
        }
        case UNK:
        {
            packet.discard(typeInfo.len);
            return 0;
        }
        default:
        {  // NON
            VN_ABORT();
        }
    };
}

const char* getMeasurementName(const size_t binaryGroup, const size_t binaryField)
{
    switch (binaryGroup)
    {
        case 0:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "TimeStartup";
                }
                case 1:
                {
                    return "TimeGps";
                }
                case 2:
                {
                    return "TimeSyncIn";
                }
                case 3:
                {
                    return "Yaw,Pitch,Roll";
                }
                case 4:
                {
                    return "QuatX,QuatY,QuatZ,QuatS";
                }
                case 5:
                {
                    return "GyroX,GyroY,GyroZ";
                }
                case 6:
                {
                    return "PosLat,PosLon,PosAlt";
                }
                case 7:
                {
                    return "VelN,VelE,VelD";
                }
                case 8:
                {
                    return "AccelX,AccelY,AccelZ";
                }
                case 9:
                {
                    return "UncompAccX,UncompAccY,UncompAccZ,UncompGyroX,UncompGyroY,UncompGyroZ";
                }
                case 10:
                {
                    return "MagX,MagY,MagZ,Temperature,Pressure";
                }
                case 11:
                {
                    return "DeltaTime,DeltaThetaX,DeltaThetaY,DeltaThetaZ,DeltaVelX,DeltaVelY,DeltaVelZ";
                }
                case 12:
                {
                    return "InsStatus";
                }
                case 13:
                {
                    return "SyncInCnt";
                }
                case 14:
                {
                    return "TimeGpsPps";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 1:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "TimeStartup";
                }
                case 1:
                {
                    return "TimeGps";
                }
                case 2:
                {
                    return "TimeGpsTow";
                }
                case 3:
                {
                    return "TimeGpsWeek";
                }
                case 4:
                {
                    return "TimeSyncIn";
                }
                case 5:
                {
                    return "TimeGpsPps";
                }
                case 6:
                {
                    return "UtcYear,UtcMonth,UtcDay,UtcHour,UtcMinute,UtcSecond,UtcFracSec";
                }
                case 7:
                {
                    return "SyncInCnt";
                }
                case 8:
                {
                    return "SyncOutCnt";
                }
                case 9:
                {
                    return "TimeStatus";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 2:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "ImuStatus";
                }
                case 1:
                {
                    return "UncompMagX,UncompMagY,UncompMagZ";
                }
                case 2:
                {
                    return "UncompAccX,UncompAccY,UncompAccZ";
                }
                case 3:
                {
                    return "UncompGyroX,UncompGyroY,UncompGyroZ";
                }
                case 4:
                {
                    return "Temperature";
                }
                case 5:
                {
                    return "Pressure";
                }
                case 6:
                {
                    return "DeltaTime,DeltaThetaX,DeltaThetaY,DeltaThetaZ";
                }
                case 7:
                {
                    return "DeltaVelX,DeltaVelY,DeltaVelZ";
                }
                case 8:
                {
                    return "MagX,MagY,MagZ";
                }
                case 9:
                {
                    return "AccelX,AccelY,AccelZ";
                }
                case 10:
                {
                    return "GyroX,GyroY,GyroZ";
                }
                case 11:
                {
                    return "SensSat";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 3:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "Gps1UtcYear,Gps1UtcMonth,Gps1UtcDay,Gps1UtcHour,Gps1UtcMinute,Gps1UtcSecond,Gps1UtcFracSec";
                }
                case 1:
                {
                    return "Gps1Tow";
                }
                case 2:
                {
                    return "Gps1Week";
                }
                case 3:
                {
                    return "Gnss1NumSats";
                }
                case 4:
                {
                    return "Gnss1Fix";
                }
                case 5:
                {
                    return "Gnss1Lat,Gnss1Lon,Gnss1Alt";
                }
                case 6:
                {
                    return "Gnss1PosX,Gnss1PosY,Gnss1PosZ";
                }
                case 7:
                {
                    return "Gnss1VelN,Gnss1VelE,Gnss1VelD";
                }
                case 8:
                {
                    return "Gnss1VelX,Gnss1VelY,Gnss1VelZ";
                }
                case 9:
                {
                    return "Gnss1PosUncertaintyN,Gnss1PosUncertaintyE,Gnss1PosUncertaintyD";
                }
                case 10:
                {
                    return "Gnss1VelUncertainty";
                }
                case 11:
                {
                    return "Gnss1TimeUncertainty";
                }
                case 12:
                {
                    return "Gnss1TimeStatus,Gnss1LeapSeconds";
                }
                case 13:
                {
                    return "Gnss1Gdop,Gnss1Pdop,Gnss1Tdop,Gnss1Vdop,Gnss1Hdop,Gnss1Ndop,Gnss1Edop";
                }
                case 14:
                {
                    return "Gnss1NumSats,Gnss1Sys_??,Gnss1SvId_??,Gnss1Flags_??,Gnss1Cno_??,Gnss1Qi_??,Gnss1El_??,Gnss1Az_??";
                }
                case 16:
                {
                    return "Gnss1Tow,Gnss1Week,Gnss1NumMeas,Gnss1Sys,Gnss1SvId,Gnss1Band,Gnss1Chan,Gnss1FreqNum,Gnss1Cno,Gnss1Flags,Gnss1Pr,Gnss1Cp,Gnss1Dp";
                }
                case 17:
                {
                    return "Gnss1Status";
                }
                case 18:
                {
                    return "Gnss1AltMSL";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 4:
        {
            switch (binaryField)
            {
                case 1:
                {
                    return "Yaw,Pitch,Roll";
                }
                case 2:
                {
                    return "QuatX,QuatY,QuatZ,QuatS";
                }
                case 3:
                {
                    return "DCM00,DCM01,DCM02,DCM10,DCM11,DCM12,DCM20,DCM21,DCM22";
                }
                case 4:
                {
                    return "MagN,MagE,MagD";
                }
                case 5:
                {
                    return "AccelN,AccelE,AccelD";
                }
                case 6:
                {
                    return "LinAccelX,LinAccelY,LinAccelZ";
                }
                case 7:
                {
                    return "LinAccelN,LinAccelE,LinAccelD";
                }
                case 8:
                {
                    return "YawU,PitchU,RollU";
                }
                case 12:
                {
                    return "Heave,HeaveRate,DelayedHeave";
                }
                case 13:
                {
                    return "AttUncertainty";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 5:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "InsStatus";
                }
                case 1:
                {
                    return "PosLat,PosLon,PosAlt";
                }
                case 2:
                {
                    return "PosEX,PosEY,PosEZ";
                }
                case 3:
                {
                    return "VelX,VelY,VelZ";
                }
                case 4:
                {
                    return "VelN,VelE,VelD";
                }
                case 5:
                {
                    return "VelEX,VelEY,VelEZ";
                }
                case 6:
                {
                    return "MagEX,MagEY,MagEZ";
                }
                case 7:
                {
                    return "AccelEX,AccelEY,AccelEZ";
                }
                case 8:
                {
                    return "LinAccelEX,LinAccelEY,LinAccelEZ";
                }
                case 9:
                {
                    return "PosUncertainty";
                }
                case 10:
                {
                    return "VelUncertainty";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 6:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "Gps2UtcYear,Gps2UtcMonth,Gps2UtcDay,Gps2UtcHour,Gps2UtcMinute,Gps2UtcSecond,Gps2UtcFracSec";
                }
                case 1:
                {
                    return "Gps2Tow";
                }
                case 2:
                {
                    return "Gps2Week";
                }
                case 3:
                {
                    return "Gnss2NumSats";
                }
                case 4:
                {
                    return "Gnss2Fix";
                }
                case 5:
                {
                    return "Gnss2Lat,Gnss2Lon,Gnss2Alt";
                }
                case 6:
                {
                    return "Gnss2PosX,Gnss2PosY,Gnss2PosZ";
                }
                case 7:
                {
                    return "Gnss2VelN,Gnss2VelE,Gnss2VelD";
                }
                case 8:
                {
                    return "Gnss2VelX,Gnss2VelY,Gnss2VelZ";
                }
                case 9:
                {
                    return "Gnss2PosUncertaintyN,Gnss2PosUncertaintyE,Gnss2PosUncertaintyD";
                }
                case 10:
                {
                    return "Gnss2VelUncertainty";
                }
                case 11:
                {
                    return "Gnss2TimeUncertainty";
                }
                case 12:
                {
                    return "Gnss2TimeStatus,Gnss2LeapSeconds";
                }
                case 13:
                {
                    return "Gnss2Gdop,Gnss2Pdop,Gnss2Tdop,Gnss2Vdop,Gnss2Hdop,Gnss2Ndop,Gnss2Edop";
                }
                case 14:
                {
                    return "Gnss2NumSats,Gnss2Sys_??,Gnss2SvId_??,Gnss2Flags_??,Gnss2Cno_??,Gnss2Qi_??,Gnss2El_??,Gnss2Az_??";
                }
                case 16:
                {
                    return "Gnss2Tow,Gnss2Week,Gnss2NumMeas,Gnss2Sys,Gnss2SvId,Gnss2Band,Gnss2Chan,Gnss2FreqNum,Gnss2Cno,Gnss2Flags,Gnss2Pr,Gnss2Cp,Gnss2Dp";
                }
                case 17:
                {
                    return "Gnss2Status";
                }
                case 18:
                {
                    return "Gnss2AltMSL";
                }
                default:
                {
                    return "";
                }
            }
        }
        case 12:
        {
            switch (binaryField)
            {
                case 0:
                {
                    return "Gps3UtcYear,Gps3UtcMonth,Gps3UtcDay,Gps3UtcHour,Gps3UtcMinute,Gps3UtcSecond,Gps3UtcFracSec";
                }
                case 1:
                {
                    return "Gps3Tow";
                }
                case 2:
                {
                    return "Gps3Week";
                }
                case 3:
                {
                    return "Gnss3NumSats";
                }
                case 4:
                {
                    return "Gnss3Fix";
                }
                case 5:
                {
                    return "Gnss3Lat,Gnss3Lon,Gnss3Alt";
                }
                case 6:
                {
                    return "Gnss3PosX,Gnss3PosY,Gnss3PosZ";
                }
                case 7:
                {
                    return "Gnss3VelN,Gnss3VelE,Gnss3VelD";
                }
                case 8:
                {
                    return "Gnss3VelX,Gnss3VelY,Gnss3VelZ";
                }
                case 9:
                {
                    return "Gnss3PosUncertaintyN,Gnss3PosUncertaintyE,Gnss3PosUncertaintyD";
                }
                case 10:
                {
                    return "Gnss3VelUncertainty";
                }
                case 11:
                {
                    return "Gnss3TimeUncertainty";
                }
                case 12:
                {
                    return "Gnss3TimeStatus,Gnss3LeapSeconds";
                }
                case 13:
                {
                    return "Gnss3Gdop,Gnss3Pdop,Gnss3Tdop,Gnss3Vdop,Gnss3Hdop,Gnss3Ndop,Gnss3Edop";
                }
                case 14:
                {
                    return "Gnss3NumSats,Gnss3Sys_??,Gnss3SvId_??,Gnss3Flags_??,Gnss3Cno_??,Gnss3Qi_??,Gnss3El_??,Gnss3Az_??";
                }
                case 16:
                {
                    return "Gnss3Tow,Gnss3Week,Gnss3NumMeas,Gnss3Sys,Gnss3SvId,Gnss3Band,Gnss3Chan,Gnss3FreqNum,Gnss3Cno,Gnss3Flags,Gnss3Pr,Gnss3Cp,Gnss3Dp";
                }
                case 17:
                {
                    return "Gnss3Status";
                }
                case 18:
                {
                    return "Gnss3AltMSL";
                }
                default:
                {
                    return "";
                }
            }
        }
        default:
        {
            VN_ABORT();
        }
    }
    VN_ABORT();
}

const char* getMeasurementString(const AsciiPacketProtocol::AsciiMeasurementHeader& msg)
{
    switch (msg)
    {
        case AsciiPacketProtocol::AsciiMeasurementHeader::YPR:
            return "Yaw,Pitch,Roll";
        case AsciiPacketProtocol::AsciiMeasurementHeader::QTN:
            return "QuatX,QuatY,QuatZ,QuatS";
        case AsciiPacketProtocol::AsciiMeasurementHeader::QMR:
            return "QuatX,QuatY,QuatZ,QuatS,MagX,MagY,MagZ,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::MAG:
            return "MagX,MagY,MagZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::ACC:
            return "AccelX,AccelY,AccelZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::GYR:
            return "GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::MAR:
            return "MagX,MagY,MagZ,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::YMR:
            return "Yaw,Pitch,Roll,MagX,MagY,MagZ,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::IMU:
            return "UncompMagX,UncompMagY,UncompMagZ,UncompAccX,UncompAccY,UncompAccZ,UncompGyroX,UncompGyroY,UncompGyroZ,Temperature,Pressure";
        case AsciiPacketProtocol::AsciiMeasurementHeader::GPS:
            return "Gps1Tow,Gps1Week,Gnss1Fix,Gnss1NumSats,Gnss1Lat,Gnss1Lon,Gnss1Alt,Gnss1VelN,Gnss1VelE,Gnss1VelD,Gnss1PosUncertaintyN,Gnss1PosUncertaintyE,"
                   "Gnss1PosUncertaintyD,Gnss1VelUncertainty,Gnss1TimeUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::GPE:
            return "Gps1Tow,Gps1Week,Gnss1Fix,Gnss1NumSats,Gnss1PosX,Gnss1PosY,Gnss1PosZ,Gnss1VelX,Gnss1VelY,Gnss1VelZ,Gnss1PosUncertaintyX,"
                   "Gnss1PosUncertaintyY,Gnss1PosUncertaintyZ,Gnss1VelUncertainty,Gnss1TimeUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::INS:
            return "TimeGpsTow,TimeGpsWeek,InsStatus,Yaw,Pitch,Roll,PosLat,PosLon,PosAlt,VelN,VelE,VelD,AttUncertainty,PosUncertainty,VelUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::INE:
            return "TimeGpsTow,TimeGpsWeek,InsStatus,Yaw,Pitch,Roll,PosEX,PosEY,PosEZ,VelEX,VelEY,VelEZ,AttUncertainty,PosUncertainty,VelUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::ISL:
            return "Yaw,Pitch,Roll,PosLat,PosLon,PosAlt,VelN,VelE,VelD,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::ISE:
            return "Yaw,Pitch,Roll,PosEX,PosEY,PosEZ,VelEX,VelEY,VelEZ,AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::DTV:
            return "DeltaTime,DeltaThetaX,DeltaThetaY,DeltaThetaZ,DeltaVelX,DeltaVelY,DeltaVelZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::G2S:
            return "Gps2Tow,Gps2Week,Gnss2Fix,Gnss2NumSats,Gnss2Lat,Gnss2Lon,Gnss2Alt,Gnss2VelN,Gnss2VelE,Gnss2VelD,Gnss2PosUncertaintyN,Gnss2PosUncertaintyE,"
                   "Gnss2PosUncertaintyD,Gnss2VelUncertainty,Gnss2TimeUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::G2E:
            return "Gps2Tow,Gps2Week,Gnss2Fix,Gnss2NumSats,Gnss2PosX,Gnss2PosY,Gnss2PosZ,Gnss2VelX,Gnss2VelY,Gnss2VelZ,Gnss2PosUncertaintyX,"
                   "Gnss2PosUncertaintyY,Gnss2PosUncertaintyZ,Gnss2VelUncertainty,Gnss2TimeUncertainty";
        case AsciiPacketProtocol::AsciiMeasurementHeader::HVE:
            return "Heave,HeaveRate,DelayedHeave";
        case AsciiPacketProtocol::AsciiMeasurementHeader::YBA:
            return "Yaw,Pitch,Roll,LinAccelX,LinAccelY,LinAccelZ,GyroX,GyroY,GyroZ";
        case AsciiPacketProtocol::AsciiMeasurementHeader::YIA:
            return "Yaw,Pitch,Roll,LinAccelN,LinAccelE,LinAccelD,GyroX,GyroY,GyroZ";
        default:
            return "";
    }
}

}  // namespace DataExport
}  // namespace VN
