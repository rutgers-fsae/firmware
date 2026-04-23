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

#include "vectornav/Implementation/MeasurementDatatypes.hpp"

#include "PyTemplates.hpp"

namespace py = pybind11;
namespace VN
{

void init_vntypes(py::module& m)
{
  py::module vntypes = m.def_submodule("VnTypes", "VnTypes Submodule");

  py::class_<Ypr>(vntypes, "Ypr")
    .def(py::init<>())
    .def_readwrite("yaw", &Ypr::yaw)
    .def_readwrite("pitch", &Ypr::pitch)
    .def_readwrite("roll", &Ypr::roll)
    .def("__repr__",
      [](const Ypr& ypr) {
        return std::to_string(ypr.yaw) + "," + std::to_string(ypr.pitch) + "," + std::to_string(ypr.roll);
      }
    );

		py::class_<TimeStatus>(vntypes, "TimeStatus")
		.def(py::init<const uint8_t&>())
		.def_property("towValid",
      [](TimeStatus& self) -> uint8_t { return self.towValid; },
      [](TimeStatus& self, const uint8_t value) { self.towValid= value; }
    )
		.def_property("dateValid",
      [](TimeStatus& self) -> uint8_t { return self.dateValid; },
      [](TimeStatus& self, const uint8_t value) { self.dateValid = value; }
    )
		.def_property("utcValid",
      [](TimeStatus& self) -> uint8_t { return self.utcValid; },
      [](TimeStatus& self, const uint8_t value) { self.utcValid = value; }
    );

		py::class_<InsStatus>(vntypes, "InsStatus")
		.def(py::init<const uint16_t&>())
		.def_property("mode",
      [](InsStatus& self) -> uint16_t { return self.mode; },
      [](InsStatus& self, const uint16_t value) { self.mode = value; }
    )
		.def_property("gnssFix",
      [](InsStatus& self) -> uint16_t { return self.gnssFix; },
      [](InsStatus& self, const uint16_t value) { self.gnssFix = value; }
    )
		.def_property("imuErr",
      [](InsStatus& self) -> uint16_t { return self.imuErr; },
      [](InsStatus& self, const uint16_t value) { self.imuErr = value; }
    )
		.def_property("magPresErr",
      [](InsStatus& self) -> uint16_t { return self.magPresErr; },
      [](InsStatus& self, const uint16_t value) { self.magPresErr = value; }
    )
		.def_property("gnssErr",
      [](InsStatus& self) -> uint16_t { return self.gnssErr; },
      [](InsStatus& self, const uint16_t value) { self.gnssErr = value; }
    )
		.def_property("gnssCompassFix",
      [](InsStatus& self) -> uint16_t { return self.gnssCompassFix; },
      [](InsStatus& self, const uint16_t value) { self.gnssCompassFix = value; }
    );
	
	py::class_<AhrsStatus>(vntypes, "AhrsStatus")
		.def(py::init<const uint16_t&>())
		.def_property("attitudeQuality",
      [](AhrsStatus& self) -> uint16_t { return self.attitudeQuality; },
      [](AhrsStatus& self, const uint16_t value) { self.attitudeQuality = value; }
    )
		.def_property("gyroSaturation",
      [](AhrsStatus& self) -> uint16_t { return self.gyroSaturation; },
      [](AhrsStatus& self, const uint16_t value) { self.gyroSaturation = value; }
    )
		.def_property("gyroSaturationRecovery",
      [](AhrsStatus& self) -> uint16_t { return self.gyroSaturationRecovery; },
      [](AhrsStatus& self, const uint16_t value) { self.gyroSaturationRecovery = value; }
    )
		.def_property("magDistrubance",
      [](AhrsStatus& self) -> uint16_t { return self.magDistrubance; },
      [](AhrsStatus& self, const uint16_t value) { self.magDistrubance = value; }
    )
		.def_property("magSaturation",
      [](AhrsStatus& self) -> uint16_t { return self.magSaturation; },
      [](AhrsStatus& self, const uint16_t value) { self.magSaturation = value; }
    )
		.def_property("accDisturbance",
      [](AhrsStatus& self) -> uint16_t { return self.accDisturbance; },
      [](AhrsStatus& self, const uint16_t value) { self.accDisturbance = value; }
    )
		.def_property("accSaturation",
      [](AhrsStatus& self) -> uint16_t { return self.accSaturation; },
      [](AhrsStatus& self, const uint16_t value) { self.accSaturation = value; }
    )
		.def_property("knownMagDisturbance",
      [](AhrsStatus& self) -> uint16_t { return self.knownMagDisturbance; },
      [](AhrsStatus& self, const uint16_t value) { self.knownMagDisturbance = value; }
    )
		.def_property("knownAccDisturbance",
      [](AhrsStatus& self) -> uint16_t { return self.knownAccDisturbance; },
      [](AhrsStatus& self, const uint16_t value) { self.knownAccDisturbance = value; }
    );
    
	py::class_<ImuStatus>(vntypes, "ImuStatus")
		.def(py::init<const uint16_t&>())
		.def_property("gyroStatus",
      [](ImuStatus& self) -> uint16_t { return self.gyroStatus; },
      [](ImuStatus& self, const uint16_t value) { return self.gyroStatus; }
    )
		.def_property("accelStatus",
      [](ImuStatus& self) -> uint16_t { return self.accelStatus; },
      [](ImuStatus& self, const uint16_t value) { self.accelStatus = value; }
    )
		.def_property("magStatus",
      [](ImuStatus& self) -> uint16_t { return self.magStatus; },
      [](ImuStatus& self, const uint16_t value) { self.magStatus = value; }
    )
		.def_property("presTempStatus",
      [](ImuStatus& self) -> uint16_t { return self.presTempStatus; },
      [](ImuStatus& self, const uint16_t value) { self.presTempStatus = value; }
    );
    
	py::class_<GnssStatus>(vntypes, "GnssStatus")
		.def(py::init<const uint16_t&>())
		.def_property("enabled",
      [](GnssStatus& self) -> uint16_t { return self.enabled; },
      [](GnssStatus& self, const uint16_t value) { self.enabled = value; }
    )
		.def_property("operational",
      [](GnssStatus& self) -> uint16_t { return self.operational; },
      [](GnssStatus& self, const uint16_t value) { self.operational = value; }
    )
		.def_property("fix",
      [](GnssStatus& self) -> uint16_t { return self.fix; },
      [](GnssStatus& self, const uint16_t value) { self.fix = value; }
    )
		.def_property("antennaSignalError",
      [](GnssStatus& self) -> uint16_t { return self.antennaSignalError; },
      [](GnssStatus& self, const uint16_t value) { self.antennaSignalError = value; }
    )
		.def_property("usedForNav",
      [](GnssStatus& self) -> uint16_t { return self.usedForNav; },
      [](GnssStatus& self, const uint16_t value) { self.usedForNav = value; }
    )
		.def_property("dataSource",
      [](GnssStatus& self) -> uint16_t { return self.dataSource; },
      [](GnssStatus& self, const uint16_t value) { self.dataSource = value; }
    )
		.def_property("usedForNavCurr",
      [](GnssStatus& self) -> uint16_t { return self.usedForNavCurr; },
      [](GnssStatus& self, const uint16_t value) { self.usedForNavCurr = value; }
    )
		.def_property("ppsUsedForTime",
      [](GnssStatus& self) -> uint16_t { return self.ppsUsedForTime; },
      [](GnssStatus& self, const uint16_t value) { self.ppsUsedForTime = value; }
    );
    
  py::class_<Time>(vntypes, "Time")
    .def(py::init<>())
    .def(py::init<uint64_t>())
    .def(py::init<double>())
    .def("nanoseconds", &Time::nanoseconds)
    .def("microseconds", &Time::microseconds)
    .def("milliseconds", &Time::milliseconds)
    .def("seconds", &Time::seconds);

  py::class_<DeltaTheta>(vntypes, "DeltaTheta")
    .def(py::init<>())
    .def_readwrite("deltaTime",     &DeltaTheta::deltaTime)
    .def_readwrite("deltaTheta",     &DeltaTheta::deltaTheta);

  py::class_<Quat>(vntypes, "Quat")
    .def(py::init<>())
    .def_readwrite("vector", &Quat::vector)
    .def_readwrite("scalar", &Quat::scalar);
    
  py::class_<Lla>(vntypes, "Lla")
    .def(py::init<>())
    .def_readwrite("lat", &Lla::lat)
    .def_readwrite("lon", &Lla::lon)
    .def_readwrite("alt", &Lla::alt);

  py::class_<TimeUtc>(vntypes, "TimeUtc")
    .def(py::init<>())
    .def_readwrite("year", &TimeUtc::year)
    .def_readwrite("month", &TimeUtc::month)
    .def_readwrite("day", &TimeUtc::day)
    .def_readwrite("hour", &TimeUtc::hour)
    .def_readwrite("minute", &TimeUtc::minute)
    .def_readwrite("second", &TimeUtc::second)
    .def_readwrite("fracSec", &TimeUtc::fracSec);
    
  py::class_<GnssTimeInfo>(vntypes, "GnssTimeInfo")
    .def(py::init<>())
    .def_readwrite("gnssTimeStatus", &GnssTimeInfo::gnssTimeStatus)
    .def_readwrite("leapSeconds", &GnssTimeInfo::leapSeconds);
    
  py::class_<GnssDop>(vntypes, "GnssDop")
    .def(py::init<>())
    .def_readwrite("gDop", &GnssDop::gDop)
    .def_readwrite("pDop", &GnssDop::pDop)
    .def_readwrite("tDop", &GnssDop::tDop)
    .def_readwrite("vDop", &GnssDop::vDop)
    .def_readwrite("hDop", &GnssDop::hDop)
    .def_readwrite("nDop", &GnssDop::nDop)
    .def_readwrite("eDop", &GnssDop::eDop);
    
  py::class_<GnssSatInfo>(vntypes, "GnssSatInfo")
    .def(py::init<>())
    .def_readwrite("numSats", &GnssSatInfo::numSats)
    .def_readwrite("resv",  &GnssSatInfo::resv)
    .def_readwrite("sys",   &GnssSatInfo::sys)
    .def_readwrite("svId",  &GnssSatInfo::svId)
    .def_readwrite("flags", &GnssSatInfo::flags)
    .def_readwrite("cno",   &GnssSatInfo::cno)
    .def_readwrite("qi",    &GnssSatInfo::qi)
    .def_readwrite("el",    &GnssSatInfo::el)
    .def_readwrite("az",    &GnssSatInfo::az);

  py::class_<GnssRawMeas>(vntypes, "GnssRawMeas")
    .def(py::init<>())
    .def_readwrite("tow",     &GnssRawMeas::tow)
    .def_readwrite("week",    &GnssRawMeas::week)
    .def_readwrite("numMeas", &GnssRawMeas::numMeas)
    .def_readwrite("resv",    &GnssRawMeas::resv)
    .def_readwrite("sys",     &GnssRawMeas::sys)
    .def_readwrite("svId",    &GnssRawMeas::svId)
    .def_readwrite("band",    &GnssRawMeas::band)
    .def_readwrite("chan",    &GnssRawMeas::chan)
    .def_readwrite("freqNum",    &GnssRawMeas::freqNum)
    .def_readwrite("cno",     &GnssRawMeas::cno)
    .def_readwrite("flags",   &GnssRawMeas::flags)
    .def_readwrite("pr",      &GnssRawMeas::pr)
    .def_readwrite("cp",      &GnssRawMeas::cp)
    .def_readwrite("dp",      &GnssRawMeas::dp);


  declare_matrix<3, 1, float>(vntypes, "Vec3f");
  declare_matrix<3, 1, double>(vntypes, "Vec3d");
  declare_matrix<3, 3, float>(vntypes, "Mat3f");

}
}
// clang-format on
