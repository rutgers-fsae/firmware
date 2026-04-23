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

#include "vectornav/Interface/CompositeData.hpp"

namespace py = pybind11;
namespace VN
{
void init_composite_data(py::module& m)
{
    py::class_<VN::CompositeData, std::shared_ptr<CompositeData>> compositeData(m, "CompositeData");
    compositeData.def("matchesMessage", py::overload_cast<const AsciiHeader&>(&CompositeData::matchesMessage, py::const_))
    .def("matchesMessage", py::overload_cast<const Registers::System::BinaryOutput&>(&CompositeData::matchesMessage, py::const_))
    .def_readwrite("time", &CompositeData::time)
    .def_readwrite("imu", &CompositeData::imu)
    .def_readwrite("gnss", &CompositeData::gnss)
    .def_readwrite("attitude", &CompositeData::attitude)
    .def_readwrite("ins", &CompositeData::ins)
    .def_readwrite("gnss2", &CompositeData::gnss2)
    #if (GNSS3_GROUP_ENABLE)
    .def_readwrite("gnss3", &CompositeData::gnss3)
    #endif
    ;
    // Time Group
    py::class_<VN::CompositeData::TimeGroup>(compositeData, "TimeGroup")
    .def_readwrite("timeStartup", &CompositeData::TimeGroup::timeStartup)
    .def_readwrite("timeGps", &CompositeData::TimeGroup::timeGps)
    .def_readwrite("timeGpsTow", &CompositeData::TimeGroup::timeGpsTow)
    .def_readwrite("timeGpsWeek", &CompositeData::TimeGroup::timeGpsWeek)
    .def_readwrite("timeSyncIn", &CompositeData::TimeGroup::timeSyncIn)
    .def_readwrite("timeGpsPps", &CompositeData::TimeGroup::timeGpsPps)
    .def_readwrite("timeUtc", &CompositeData::TimeGroup::timeUtc)
    .def_readwrite("syncInCnt", &CompositeData::TimeGroup::syncInCnt)
    .def_readwrite("syncOutCnt", &CompositeData::TimeGroup::syncOutCnt)
    .def_readwrite("timeStatus", &CompositeData::TimeGroup::timeStatus)
    ;
    // Imu Group
    py::class_<VN::CompositeData::ImuGroup>(compositeData, "ImuGroup")
    .def_readwrite("imuStatus", &CompositeData::ImuGroup::imuStatus)
    .def_readwrite("uncompMag", &CompositeData::ImuGroup::uncompMag)
    .def_readwrite("uncompAccel", &CompositeData::ImuGroup::uncompAccel)
    .def_readwrite("uncompGyro", &CompositeData::ImuGroup::uncompGyro)
    .def_readwrite("temperature", &CompositeData::ImuGroup::temperature)
    .def_readwrite("pressure", &CompositeData::ImuGroup::pressure)
    .def_readwrite("deltaTheta", &CompositeData::ImuGroup::deltaTheta)
    .def_readwrite("deltaVel", &CompositeData::ImuGroup::deltaVel)
    .def_readwrite("mag", &CompositeData::ImuGroup::mag)
    .def_readwrite("accel", &CompositeData::ImuGroup::accel)
    .def_readwrite("angularRate", &CompositeData::ImuGroup::angularRate)
    .def_readwrite("sensSat", &CompositeData::ImuGroup::sensSat)
    ;
    // Gnss Group
    py::class_<VN::CompositeData::GnssGroup>(compositeData, "GnssGroup")
    .def_readwrite("gnss1TimeUtc", &CompositeData::GnssGroup::gnss1TimeUtc)
    .def_readwrite("gps1Tow", &CompositeData::GnssGroup::gps1Tow)
    .def_readwrite("gps1Week", &CompositeData::GnssGroup::gps1Week)
    .def_readwrite("gnss1NumSats", &CompositeData::GnssGroup::gnss1NumSats)
    .def_readwrite("gnss1Fix", &CompositeData::GnssGroup::gnss1Fix)
    .def_readwrite("gnss1PosLla", &CompositeData::GnssGroup::gnss1PosLla)
    .def_readwrite("gnss1PosEcef", &CompositeData::GnssGroup::gnss1PosEcef)
    .def_readwrite("gnss1VelNed", &CompositeData::GnssGroup::gnss1VelNed)
    .def_readwrite("gnss1VelEcef", &CompositeData::GnssGroup::gnss1VelEcef)
    .def_readwrite("gnss1PosUncertainty", &CompositeData::GnssGroup::gnss1PosUncertainty)
    .def_readwrite("gnss1VelUncertainty", &CompositeData::GnssGroup::gnss1VelUncertainty)
    .def_readwrite("gnss1TimeUncertainty", &CompositeData::GnssGroup::gnss1TimeUncertainty)
    .def_readwrite("gnss1TimeInfo", &CompositeData::GnssGroup::gnss1TimeInfo)
    .def_readwrite("gnss1Dop", &CompositeData::GnssGroup::gnss1Dop)
    .def_readwrite("gnss1SatInfo", &CompositeData::GnssGroup::gnss1SatInfo)
    .def_readwrite("gnss1RawMeas", &CompositeData::GnssGroup::gnss1RawMeas)
    .def_readwrite("gnss1Status", &CompositeData::GnssGroup::gnss1Status)
    .def_readwrite("gnss1AltMsl", &CompositeData::GnssGroup::gnss1AltMsl)
    ;
    // Attitude Group
    py::class_<VN::CompositeData::AttitudeGroup>(compositeData, "AttitudeGroup")
    .def_readwrite("ypr", &CompositeData::AttitudeGroup::ypr)
    .def_readwrite("quaternion", &CompositeData::AttitudeGroup::quaternion)
    .def_readwrite("dcm", &CompositeData::AttitudeGroup::dcm)
    .def_readwrite("magNed", &CompositeData::AttitudeGroup::magNed)
    .def_readwrite("accelNed", &CompositeData::AttitudeGroup::accelNed)
    .def_readwrite("linBodyAcc", &CompositeData::AttitudeGroup::linBodyAcc)
    .def_readwrite("linAccelNed", &CompositeData::AttitudeGroup::linAccelNed)
    .def_readwrite("yprU", &CompositeData::AttitudeGroup::yprU)
    .def_readwrite("heave", &CompositeData::AttitudeGroup::heave)
    .def_readwrite("attU", &CompositeData::AttitudeGroup::attU)
    ;
    // Ins Group
    py::class_<VN::CompositeData::InsGroup>(compositeData, "InsGroup")
    .def_readwrite("insStatus", &CompositeData::InsGroup::insStatus)
    .def_readwrite("posLla", &CompositeData::InsGroup::posLla)
    .def_readwrite("posEcef", &CompositeData::InsGroup::posEcef)
    .def_readwrite("velBody", &CompositeData::InsGroup::velBody)
    .def_readwrite("velNed", &CompositeData::InsGroup::velNed)
    .def_readwrite("velEcef", &CompositeData::InsGroup::velEcef)
    .def_readwrite("magEcef", &CompositeData::InsGroup::magEcef)
    .def_readwrite("accelEcef", &CompositeData::InsGroup::accelEcef)
    .def_readwrite("linAccelEcef", &CompositeData::InsGroup::linAccelEcef)
    .def_readwrite("posU", &CompositeData::InsGroup::posU)
    .def_readwrite("velU", &CompositeData::InsGroup::velU)
    ;
    // Gnss2 Group
    py::class_<VN::CompositeData::Gnss2Group>(compositeData, "Gnss2Group")
    .def_readwrite("gnss2TimeUtc", &CompositeData::Gnss2Group::gnss2TimeUtc)
    .def_readwrite("gps2Tow", &CompositeData::Gnss2Group::gps2Tow)
    .def_readwrite("gps2Week", &CompositeData::Gnss2Group::gps2Week)
    .def_readwrite("gnss2NumSats", &CompositeData::Gnss2Group::gnss2NumSats)
    .def_readwrite("gnss2Fix", &CompositeData::Gnss2Group::gnss2Fix)
    .def_readwrite("gnss2PosLla", &CompositeData::Gnss2Group::gnss2PosLla)
    .def_readwrite("gnss2PosEcef", &CompositeData::Gnss2Group::gnss2PosEcef)
    .def_readwrite("gnss2VelNed", &CompositeData::Gnss2Group::gnss2VelNed)
    .def_readwrite("gnss2VelEcef", &CompositeData::Gnss2Group::gnss2VelEcef)
    .def_readwrite("gnss2PosUncertainty", &CompositeData::Gnss2Group::gnss2PosUncertainty)
    .def_readwrite("gnss2VelUncertainty", &CompositeData::Gnss2Group::gnss2VelUncertainty)
    .def_readwrite("gnss2TimeUncertainty", &CompositeData::Gnss2Group::gnss2TimeUncertainty)
    .def_readwrite("gnss2TimeInfo", &CompositeData::Gnss2Group::gnss2TimeInfo)
    .def_readwrite("gnss2Dop", &CompositeData::Gnss2Group::gnss2Dop)
    .def_readwrite("gnss2SatInfo", &CompositeData::Gnss2Group::gnss2SatInfo)
    .def_readwrite("gnss2RawMeas", &CompositeData::Gnss2Group::gnss2RawMeas)
    .def_readwrite("gnss2Status", &CompositeData::Gnss2Group::gnss2Status)
    .def_readwrite("gnss2AltMsl", &CompositeData::Gnss2Group::gnss2AltMsl)
    ;
    #if (GNSS3_GROUP_ENABLE)
    // Gnss3 Group
    py::class_<VN::CompositeData::Gnss3Group>(compositeData, "Gnss3Group")
    .def_readwrite("gnss3TimeUtc", &CompositeData::Gnss3Group::gnss3TimeUtc)
    .def_readwrite("gps3Tow", &CompositeData::Gnss3Group::gps3Tow)
    .def_readwrite("gps3Week", &CompositeData::Gnss3Group::gps3Week)
    .def_readwrite("gnss3NumSats", &CompositeData::Gnss3Group::gnss3NumSats)
    .def_readwrite("gnss3Fix", &CompositeData::Gnss3Group::gnss3Fix)
    .def_readwrite("gnss3PosLla", &CompositeData::Gnss3Group::gnss3PosLla)
    .def_readwrite("gnss3PosEcef", &CompositeData::Gnss3Group::gnss3PosEcef)
    .def_readwrite("gnss3VelNed", &CompositeData::Gnss3Group::gnss3VelNed)
    .def_readwrite("gnss3VelEcef", &CompositeData::Gnss3Group::gnss3VelEcef)
    .def_readwrite("gnss3PosUncertainty", &CompositeData::Gnss3Group::gnss3PosUncertainty)
    .def_readwrite("gnss3VelUncertainty", &CompositeData::Gnss3Group::gnss3VelUncertainty)
    .def_readwrite("gnss3TimeUncertainty", &CompositeData::Gnss3Group::gnss3TimeUncertainty)
    .def_readwrite("gnss3TimeInfo", &CompositeData::Gnss3Group::gnss3TimeInfo)
    .def_readwrite("gnss3Dop", &CompositeData::Gnss3Group::gnss3Dop)
    .def_readwrite("gnss3SatInfo", &CompositeData::Gnss3Group::gnss3SatInfo)
    .def_readwrite("gnss3RawMeas", &CompositeData::Gnss3Group::gnss3RawMeas)
    .def_readwrite("gnss3Status", &CompositeData::Gnss3Group::gnss3Status)
    .def_readwrite("gnss3AltMsl", &CompositeData::Gnss3Group::gnss3AltMsl)
    ;
    #endif
}
}
// clang-format on

