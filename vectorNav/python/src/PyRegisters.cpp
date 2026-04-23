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

#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Implementation/BinaryHeader.hpp"

namespace py = pybind11;
namespace VN
{
void init_registers(py::module& m)
{
    py::module Registers = m.def_submodule("Registers", "Registers submodule");
    py::class_<Register>(Registers, "Register");
    py::class_<ConfigurationRegister, Register>(Registers, "ConfigurationRegister")
    .def("toReadCommand", &ConfigurationRegister::toReadCommand)
    .def("toWriteCommand", &ConfigurationRegister::toWriteCommand)
    .def("__eq__", [](const ConfigurationRegister& self, const ConfigurationRegister& other) 
    {
        return Registers::isConfigEqual(self, other);
    }
    );
    
    py::class_<MeasurementRegister, Register>(Registers, "MeasurementRegister")
    .def("toReadCommand", &MeasurementRegister::toReadCommand);
    
    // Module Attitude
    py::module Attitude = Registers.def_submodule("Attitude", "Attitude Submodule");
    // Register 8 YawPitchRoll
    py::class_<Registers::Attitude::YawPitchRoll, MeasurementRegister> yawPitchRoll(Attitude, "YawPitchRoll");
    yawPitchRoll.def(py::init<>())
    .def_readwrite("yaw", &Registers::Attitude::YawPitchRoll::yaw)
    .def_readwrite("pitch", &Registers::Attitude::YawPitchRoll::pitch)
    .def_readwrite("roll", &Registers::Attitude::YawPitchRoll::roll)
    ;
    // Register 9 Quaternion
    py::class_<Registers::Attitude::Quaternion, MeasurementRegister> quaternion(Attitude, "Quaternion");
    quaternion.def(py::init<>())
    .def_readwrite("quatX", &Registers::Attitude::Quaternion::quatX)
    .def_readwrite("quatY", &Registers::Attitude::Quaternion::quatY)
    .def_readwrite("quatZ", &Registers::Attitude::Quaternion::quatZ)
    .def_readwrite("quatS", &Registers::Attitude::Quaternion::quatS)
    ;
    // Register 15 QuatMagAccelRate
    py::class_<Registers::Attitude::QuatMagAccelRate, MeasurementRegister> quatMagAccelRate(Attitude, "QuatMagAccelRate");
    quatMagAccelRate.def(py::init<>())
    .def_readwrite("quatX", &Registers::Attitude::QuatMagAccelRate::quatX)
    .def_readwrite("quatY", &Registers::Attitude::QuatMagAccelRate::quatY)
    .def_readwrite("quatZ", &Registers::Attitude::QuatMagAccelRate::quatZ)
    .def_readwrite("quatS", &Registers::Attitude::QuatMagAccelRate::quatS)
    .def_readwrite("magX", &Registers::Attitude::QuatMagAccelRate::magX)
    .def_readwrite("magY", &Registers::Attitude::QuatMagAccelRate::magY)
    .def_readwrite("magZ", &Registers::Attitude::QuatMagAccelRate::magZ)
    .def_readwrite("accelX", &Registers::Attitude::QuatMagAccelRate::accelX)
    .def_readwrite("accelY", &Registers::Attitude::QuatMagAccelRate::accelY)
    .def_readwrite("accelZ", &Registers::Attitude::QuatMagAccelRate::accelZ)
    .def_readwrite("gyroX", &Registers::Attitude::QuatMagAccelRate::gyroX)
    .def_readwrite("gyroY", &Registers::Attitude::QuatMagAccelRate::gyroY)
    .def_readwrite("gyroZ", &Registers::Attitude::QuatMagAccelRate::gyroZ)
    ;
    // Register 21 MagGravRefVec
    py::class_<Registers::Attitude::MagGravRefVec, ConfigurationRegister> magGravRefVec(Attitude, "MagGravRefVec");
    magGravRefVec.def(py::init<>())
    .def_readwrite("magRefN", &Registers::Attitude::MagGravRefVec::magRefN)
    .def_readwrite("magRefE", &Registers::Attitude::MagGravRefVec::magRefE)
    .def_readwrite("magRefD", &Registers::Attitude::MagGravRefVec::magRefD)
    .def_readwrite("gravRefN", &Registers::Attitude::MagGravRefVec::gravRefN)
    .def_readwrite("gravRefE", &Registers::Attitude::MagGravRefVec::gravRefE)
    .def_readwrite("gravRefD", &Registers::Attitude::MagGravRefVec::gravRefD)
    ;
    // Register 27 YprMagAccelAngularRates
    py::class_<Registers::Attitude::YprMagAccelAngularRates, MeasurementRegister> yprMagAccelAngularRates(Attitude, "YprMagAccelAngularRates");
    yprMagAccelAngularRates.def(py::init<>())
    .def_readwrite("yaw", &Registers::Attitude::YprMagAccelAngularRates::yaw)
    .def_readwrite("pitch", &Registers::Attitude::YprMagAccelAngularRates::pitch)
    .def_readwrite("roll", &Registers::Attitude::YprMagAccelAngularRates::roll)
    .def_readwrite("magX", &Registers::Attitude::YprMagAccelAngularRates::magX)
    .def_readwrite("magY", &Registers::Attitude::YprMagAccelAngularRates::magY)
    .def_readwrite("magZ", &Registers::Attitude::YprMagAccelAngularRates::magZ)
    .def_readwrite("accelX", &Registers::Attitude::YprMagAccelAngularRates::accelX)
    .def_readwrite("accelY", &Registers::Attitude::YprMagAccelAngularRates::accelY)
    .def_readwrite("accelZ", &Registers::Attitude::YprMagAccelAngularRates::accelZ)
    .def_readwrite("gyroX", &Registers::Attitude::YprMagAccelAngularRates::gyroX)
    .def_readwrite("gyroY", &Registers::Attitude::YprMagAccelAngularRates::gyroY)
    .def_readwrite("gyroZ", &Registers::Attitude::YprMagAccelAngularRates::gyroZ)
    ;
    // Register 35 VpeBasicControl
    py::class_<Registers::Attitude::VpeBasicControl, ConfigurationRegister> vpeBasicControl(Attitude, "VpeBasicControl");
    vpeBasicControl.def(py::init<>())
    .def_readwrite("resv", &Registers::Attitude::VpeBasicControl::resv)
    .def_readwrite("headingMode", &Registers::Attitude::VpeBasicControl::headingMode)
    .def_readwrite("filteringMode", &Registers::Attitude::VpeBasicControl::filteringMode)
    .def_readwrite("tuningMode", &Registers::Attitude::VpeBasicControl::tuningMode)
    ;
    // Register 36 VpeMagBasicTuning
    py::class_<Registers::Attitude::VpeMagBasicTuning, ConfigurationRegister> vpeMagBasicTuning(Attitude, "VpeMagBasicTuning");
    vpeMagBasicTuning.def(py::init<>())
    .def_readwrite("baseTuningX", &Registers::Attitude::VpeMagBasicTuning::baseTuningX)
    .def_readwrite("baseTuningY", &Registers::Attitude::VpeMagBasicTuning::baseTuningY)
    .def_readwrite("baseTuningZ", &Registers::Attitude::VpeMagBasicTuning::baseTuningZ)
    .def_readwrite("adaptiveTuningX", &Registers::Attitude::VpeMagBasicTuning::adaptiveTuningX)
    .def_readwrite("adaptiveTuningY", &Registers::Attitude::VpeMagBasicTuning::adaptiveTuningY)
    .def_readwrite("adaptiveTuningZ", &Registers::Attitude::VpeMagBasicTuning::adaptiveTuningZ)
    .def_readwrite("adaptiveFilteringX", &Registers::Attitude::VpeMagBasicTuning::adaptiveFilteringX)
    .def_readwrite("adaptiveFilteringY", &Registers::Attitude::VpeMagBasicTuning::adaptiveFilteringY)
    .def_readwrite("adaptiveFilteringZ", &Registers::Attitude::VpeMagBasicTuning::adaptiveFilteringZ)
    ;
    // Register 38 VpeAccelBasicTuning
    py::class_<Registers::Attitude::VpeAccelBasicTuning, ConfigurationRegister> vpeAccelBasicTuning(Attitude, "VpeAccelBasicTuning");
    vpeAccelBasicTuning.def(py::init<>())
    .def_readwrite("baseTuningX", &Registers::Attitude::VpeAccelBasicTuning::baseTuningX)
    .def_readwrite("baseTuningY", &Registers::Attitude::VpeAccelBasicTuning::baseTuningY)
    .def_readwrite("baseTuningZ", &Registers::Attitude::VpeAccelBasicTuning::baseTuningZ)
    .def_readwrite("adaptiveTuningX", &Registers::Attitude::VpeAccelBasicTuning::adaptiveTuningX)
    .def_readwrite("adaptiveTuningY", &Registers::Attitude::VpeAccelBasicTuning::adaptiveTuningY)
    .def_readwrite("adaptiveTuningZ", &Registers::Attitude::VpeAccelBasicTuning::adaptiveTuningZ)
    .def_readwrite("adaptiveFilteringX", &Registers::Attitude::VpeAccelBasicTuning::adaptiveFilteringX)
    .def_readwrite("adaptiveFilteringY", &Registers::Attitude::VpeAccelBasicTuning::adaptiveFilteringY)
    .def_readwrite("adaptiveFilteringZ", &Registers::Attitude::VpeAccelBasicTuning::adaptiveFilteringZ)
    ;
    // Register 239 YprLinearBodyAccelAngularRates
    py::class_<Registers::Attitude::YprLinearBodyAccelAngularRates, MeasurementRegister> yprLinearBodyAccelAngularRates(Attitude, "YprLinearBodyAccelAngularRates");
    yprLinearBodyAccelAngularRates.def(py::init<>())
    .def_readwrite("yaw", &Registers::Attitude::YprLinearBodyAccelAngularRates::yaw)
    .def_readwrite("pitch", &Registers::Attitude::YprLinearBodyAccelAngularRates::pitch)
    .def_readwrite("roll", &Registers::Attitude::YprLinearBodyAccelAngularRates::roll)
    .def_readwrite("linAccelX", &Registers::Attitude::YprLinearBodyAccelAngularRates::linAccelX)
    .def_readwrite("linAccelY", &Registers::Attitude::YprLinearBodyAccelAngularRates::linAccelY)
    .def_readwrite("linAccelZ", &Registers::Attitude::YprLinearBodyAccelAngularRates::linAccelZ)
    .def_readwrite("gyroX", &Registers::Attitude::YprLinearBodyAccelAngularRates::gyroX)
    .def_readwrite("gyroY", &Registers::Attitude::YprLinearBodyAccelAngularRates::gyroY)
    .def_readwrite("gyroZ", &Registers::Attitude::YprLinearBodyAccelAngularRates::gyroZ)
    ;
    // Register 240 YprLinearInertialAccelAngularRates
    py::class_<Registers::Attitude::YprLinearInertialAccelAngularRates, MeasurementRegister> yprLinearInertialAccelAngularRates(Attitude, "YprLinearInertialAccelAngularRates");
    yprLinearInertialAccelAngularRates.def(py::init<>())
    .def_readwrite("yaw", &Registers::Attitude::YprLinearInertialAccelAngularRates::yaw)
    .def_readwrite("pitch", &Registers::Attitude::YprLinearInertialAccelAngularRates::pitch)
    .def_readwrite("roll", &Registers::Attitude::YprLinearInertialAccelAngularRates::roll)
    .def_readwrite("linAccelN", &Registers::Attitude::YprLinearInertialAccelAngularRates::linAccelN)
    .def_readwrite("linAccelE", &Registers::Attitude::YprLinearInertialAccelAngularRates::linAccelE)
    .def_readwrite("linAccelD", &Registers::Attitude::YprLinearInertialAccelAngularRates::linAccelD)
    .def_readwrite("gyroX", &Registers::Attitude::YprLinearInertialAccelAngularRates::gyroX)
    .def_readwrite("gyroY", &Registers::Attitude::YprLinearInertialAccelAngularRates::gyroY)
    .def_readwrite("gyroZ", &Registers::Attitude::YprLinearInertialAccelAngularRates::gyroZ)
    ;
    // Module end Attitude
    
    // Module GNSS
    py::module GNSS = Registers.def_submodule("GNSS", "GNSS Submodule");
    // Register 55 GnssBasicConfig
    py::class_<Registers::GNSS::GnssBasicConfig, ConfigurationRegister> gnssBasicConfig(GNSS, "GnssBasicConfig");
    gnssBasicConfig.def(py::init<>())
    .def_readwrite("receiverEnable", &Registers::GNSS::GnssBasicConfig::receiverEnable)
    .def_readwrite("ppsSource", &Registers::GNSS::GnssBasicConfig::ppsSource)
    .def_readwrite("rate", &Registers::GNSS::GnssBasicConfig::rate)
    .def_readwrite("resv4", &Registers::GNSS::GnssBasicConfig::resv4)
    .def_readwrite("antPower", &Registers::GNSS::GnssBasicConfig::antPower)
    ;
    // Register 57 GnssAOffset
    py::class_<Registers::GNSS::GnssAOffset, ConfigurationRegister> gnssAOffset(GNSS, "GnssAOffset");
    gnssAOffset.def(py::init<>())
    .def_readwrite("positionX", &Registers::GNSS::GnssAOffset::positionX)
    .def_readwrite("positionY", &Registers::GNSS::GnssAOffset::positionY)
    .def_readwrite("positionZ", &Registers::GNSS::GnssAOffset::positionZ)
    ;
    // Register 58 GnssSolLla
    py::class_<Registers::GNSS::GnssSolLla, ConfigurationRegister> gnssSolLla(GNSS, "GnssSolLla");
    gnssSolLla.def(py::init<>())
    .def_readwrite("gps1Tow", &Registers::GNSS::GnssSolLla::gps1Tow)
    .def_readwrite("gps1Week", &Registers::GNSS::GnssSolLla::gps1Week)
    .def_readwrite("gnss1Fix", &Registers::GNSS::GnssSolLla::gnss1Fix)
    .def_readwrite("gnss1NumSats", &Registers::GNSS::GnssSolLla::gnss1NumSats)
    .def_readwrite("gnss1Lat", &Registers::GNSS::GnssSolLla::gnss1Lat)
    .def_readwrite("gnss1Lon", &Registers::GNSS::GnssSolLla::gnss1Lon)
    .def_readwrite("gnss1Alt", &Registers::GNSS::GnssSolLla::gnss1Alt)
    .def_readwrite("gnss1VelN", &Registers::GNSS::GnssSolLla::gnss1VelN)
    .def_readwrite("gnss1VelE", &Registers::GNSS::GnssSolLla::gnss1VelE)
    .def_readwrite("gnss1VelD", &Registers::GNSS::GnssSolLla::gnss1VelD)
    .def_readwrite("gnss1PosUncertaintyN", &Registers::GNSS::GnssSolLla::gnss1PosUncertaintyN)
    .def_readwrite("gnss1PosUncertaintyE", &Registers::GNSS::GnssSolLla::gnss1PosUncertaintyE)
    .def_readwrite("gnss1PosUncertaintyD", &Registers::GNSS::GnssSolLla::gnss1PosUncertaintyD)
    .def_readwrite("gnss1VelUncertainty", &Registers::GNSS::GnssSolLla::gnss1VelUncertainty)
    .def_readwrite("gnss1TimeUncertainty", &Registers::GNSS::GnssSolLla::gnss1TimeUncertainty)
    ;
    // Register 59 GnssSolEcef
    py::class_<Registers::GNSS::GnssSolEcef, ConfigurationRegister> gnssSolEcef(GNSS, "GnssSolEcef");
    gnssSolEcef.def(py::init<>())
    .def_readwrite("gps1Tow", &Registers::GNSS::GnssSolEcef::gps1Tow)
    .def_readwrite("gps1Week", &Registers::GNSS::GnssSolEcef::gps1Week)
    .def_readwrite("gnss1Fix", &Registers::GNSS::GnssSolEcef::gnss1Fix)
    .def_readwrite("gnss1NumSats", &Registers::GNSS::GnssSolEcef::gnss1NumSats)
    .def_readwrite("gnss1PosX", &Registers::GNSS::GnssSolEcef::gnss1PosX)
    .def_readwrite("gnss1PosY", &Registers::GNSS::GnssSolEcef::gnss1PosY)
    .def_readwrite("gnss1PosZ", &Registers::GNSS::GnssSolEcef::gnss1PosZ)
    .def_readwrite("gnss1VelX", &Registers::GNSS::GnssSolEcef::gnss1VelX)
    .def_readwrite("gnss1VelY", &Registers::GNSS::GnssSolEcef::gnss1VelY)
    .def_readwrite("gnss1VelZ", &Registers::GNSS::GnssSolEcef::gnss1VelZ)
    .def_readwrite("gnss1PosUncertaintyX", &Registers::GNSS::GnssSolEcef::gnss1PosUncertaintyX)
    .def_readwrite("gnss1PosUncertaintyY", &Registers::GNSS::GnssSolEcef::gnss1PosUncertaintyY)
    .def_readwrite("gnss1PosUncertaintyZ", &Registers::GNSS::GnssSolEcef::gnss1PosUncertaintyZ)
    .def_readwrite("gnss1VelUncertainty", &Registers::GNSS::GnssSolEcef::gnss1VelUncertainty)
    .def_readwrite("gnss1TimeUncertainty", &Registers::GNSS::GnssSolEcef::gnss1TimeUncertainty)
    ;
    // Register 99 GnssSystemConfig
    py::class_<Registers::GNSS::GnssSystemConfig, ConfigurationRegister> gnssSystemConfig(GNSS, "GnssSystemConfig");
    gnssSystemConfig.def(py::init<>())
    .def_property("systems",
    [](Registers::GNSS::GnssSystemConfig &self) -> Registers::GNSS::GnssSystemConfig::Systems& {
        if (!self.systems) {
            self.systems.emplace(0);
        }
        return *self.systems;
    },
    [](Registers::GNSS::GnssSystemConfig &self, const Registers::GNSS::GnssSystemConfig::Systems& value) {
        self.systems = value;
    }
    )
    .def_readwrite("minCno", &Registers::GNSS::GnssSystemConfig::minCno)
    .def_readwrite("minElev", &Registers::GNSS::GnssSystemConfig::minElev)
    .def_readwrite("maxSats", &Registers::GNSS::GnssSystemConfig::maxSats)
    .def_property("sbasMode",
    [](Registers::GNSS::GnssSystemConfig &self) -> Registers::GNSS::GnssSystemConfig::SbasMode& {
        if (!self.sbasMode) {
            self.sbasMode.emplace(0);
        }
        return *self.sbasMode;
    },
    [](Registers::GNSS::GnssSystemConfig &self, const Registers::GNSS::GnssSystemConfig::SbasMode& value) {
        self.sbasMode = value;
    }
    )
    .def_property("sbasSelect1",
    [](Registers::GNSS::GnssSystemConfig &self) -> Registers::GNSS::GnssSystemConfig::SbasSelect1& {
        if (!self.sbasSelect1) {
            self.sbasSelect1.emplace(0);
        }
        return *self.sbasSelect1;
    },
    [](Registers::GNSS::GnssSystemConfig &self, const Registers::GNSS::GnssSystemConfig::SbasSelect1& value) {
        self.sbasSelect1 = value;
    }
    )
    .def_property("sbasSelect2",
    [](Registers::GNSS::GnssSystemConfig &self) -> Registers::GNSS::GnssSystemConfig::SbasSelect2& {
        if (!self.sbasSelect2) {
            self.sbasSelect2.emplace(0);
        }
        return *self.sbasSelect2;
    },
    [](Registers::GNSS::GnssSystemConfig &self, const Registers::GNSS::GnssSystemConfig::SbasSelect2& value) {
        self.sbasSelect2 = value;
    }
    )
    .def_property("sbasSelect3",
    [](Registers::GNSS::GnssSystemConfig &self) -> Registers::GNSS::GnssSystemConfig::SbasSelect3& {
        if (!self.sbasSelect3) {
            self.sbasSelect3.emplace(0);
        }
        return *self.sbasSelect3;
    },
    [](Registers::GNSS::GnssSystemConfig &self, const Registers::GNSS::GnssSystemConfig::SbasSelect3& value) {
        self.sbasSelect3 = value;
    }
    )
    .def_readwrite("receiverSelect", &Registers::GNSS::GnssSystemConfig::receiverSelect)
    ;
    py::class_<Registers::GNSS::GnssSystemConfig::Systems>(gnssSystemConfig, "Systems")
    .def(py::init<>())
    .def(py::init<const uint16_t&>())
    .def_property("gps", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.gps;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.gps = value;
    })
    .def_property("sbas", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.sbas;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.sbas = value;
    })
    .def_property("glonass", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.glonass;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.glonass = value;
    })
    .def_property("beidou", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.beidou;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.beidou = value;
    })
    .def_property("galileo", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.galileo;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.galileo = value;
    })
    .def_property("imes", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.imes;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.imes = value;
    })
    .def_property("qzssL1Ca", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.qzssL1Ca;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.qzssL1Ca = value;
    })
    .def_property("qzssL1Saif", 
    [](Registers::GNSS::GnssSystemConfig::Systems& self) -> uint16_t
    {
        return self.qzssL1Saif;
    },
    [](Registers::GNSS::GnssSystemConfig::Systems& self, const uint16_t value) 
    {
        self.qzssL1Saif = value;
    })
    ;
    py::class_<Registers::GNSS::GnssSystemConfig::SbasMode>(gnssSystemConfig, "SbasMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("ranging", 
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self) -> uint8_t
    {
        return self.ranging;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self, const uint8_t value) 
    {
        self.ranging = value;
    })
    .def_property("diffCorr", 
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self) -> uint8_t
    {
        return self.diffCorr;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self, const uint8_t value) 
    {
        self.diffCorr = value;
    })
    .def_property("integrity", 
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self) -> uint8_t
    {
        return self.integrity;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self, const uint8_t value) 
    {
        self.integrity = value;
    })
    .def_property("testMode", 
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self) -> uint8_t
    {
        return self.testMode;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasMode& self, const uint8_t value) 
    {
        self.testMode = value;
    })
    ;
    py::class_<Registers::GNSS::GnssSystemConfig::SbasSelect1>(gnssSystemConfig, "SbasSelect1")
    .def(py::init<>())
    .def(py::init<const uint16_t&>())
    .def_property("sbas120", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas120;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas120 = value;
    })
    .def_property("sbas121", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas121;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas121 = value;
    })
    .def_property("sbas122", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas122;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas122 = value;
    })
    .def_property("sbas123", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas123;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas123 = value;
    })
    .def_property("sbas124", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas124;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas124 = value;
    })
    .def_property("sbas125", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas125;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas125 = value;
    })
    .def_property("sbas126", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas126;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas126 = value;
    })
    .def_property("sbas127", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas127;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas127 = value;
    })
    .def_property("sbas128", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas128;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas128 = value;
    })
    .def_property("sbas129", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas129;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas129 = value;
    })
    .def_property("sbas130", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas130;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas130 = value;
    })
    .def_property("sbas131", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas131;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas131 = value;
    })
    .def_property("sbas132", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas132;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas132 = value;
    })
    .def_property("sbas133", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas133;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas133 = value;
    })
    .def_property("sbas134", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas134;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas134 = value;
    })
    .def_property("sbas135", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self) -> uint16_t
    {
        return self.sbas135;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect1& self, const uint16_t value) 
    {
        self.sbas135 = value;
    })
    ;
    py::class_<Registers::GNSS::GnssSystemConfig::SbasSelect2>(gnssSystemConfig, "SbasSelect2")
    .def(py::init<>())
    .def(py::init<const uint16_t&>())
    .def_property("sbas136", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas136;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas136 = value;
    })
    .def_property("sbas137", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas137;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas137 = value;
    })
    .def_property("sbas138", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas138;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas138 = value;
    })
    .def_property("sbas139", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas139;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas139 = value;
    })
    .def_property("sbas140", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas140;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas140 = value;
    })
    .def_property("sbas141", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas141;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas141 = value;
    })
    .def_property("sbas142", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas142;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas142 = value;
    })
    .def_property("sbas143", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas143;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas143 = value;
    })
    .def_property("sbas144", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas144;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas144 = value;
    })
    .def_property("sbas145", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas145;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas145 = value;
    })
    .def_property("sbas146", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas146;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas146 = value;
    })
    .def_property("sbas147", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas147;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas147 = value;
    })
    .def_property("sbas148", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas148;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas148 = value;
    })
    .def_property("sbas149", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas149;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas149 = value;
    })
    .def_property("sbas150", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas150;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas150 = value;
    })
    .def_property("sbas151", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self) -> uint16_t
    {
        return self.sbas151;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect2& self, const uint16_t value) 
    {
        self.sbas151 = value;
    })
    ;
    py::class_<Registers::GNSS::GnssSystemConfig::SbasSelect3>(gnssSystemConfig, "SbasSelect3")
    .def(py::init<>())
    .def(py::init<const uint16_t&>())
    .def_property("sbas152", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas152;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas152 = value;
    })
    .def_property("sbas153", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas153;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas153 = value;
    })
    .def_property("sbas154", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas154;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas154 = value;
    })
    .def_property("sbas155", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas155;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas155 = value;
    })
    .def_property("sbas156", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas156;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas156 = value;
    })
    .def_property("sbas157", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas157;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas157 = value;
    })
    .def_property("sbas158", 
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self) -> uint16_t
    {
        return self.sbas158;
    },
    [](Registers::GNSS::GnssSystemConfig::SbasSelect3& self, const uint16_t value) 
    {
        self.sbas158 = value;
    })
    ;
    // Register 100 GnssSyncConfig
    py::class_<Registers::GNSS::GnssSyncConfig, ConfigurationRegister> gnssSyncConfig(GNSS, "GnssSyncConfig");
    gnssSyncConfig.def(py::init<>())
    .def_readwrite("gnssSyncEnable", &Registers::GNSS::GnssSyncConfig::gnssSyncEnable)
    .def_readwrite("polarity", &Registers::GNSS::GnssSyncConfig::polarity)
    .def_readwrite("specType", &Registers::GNSS::GnssSyncConfig::specType)
    .def_readwrite("resv", &Registers::GNSS::GnssSyncConfig::resv)
    .def_readwrite("period", &Registers::GNSS::GnssSyncConfig::period)
    .def_readwrite("pulseWidth", &Registers::GNSS::GnssSyncConfig::pulseWidth)
    .def_readwrite("offset", &Registers::GNSS::GnssSyncConfig::offset)
    ;
    // Register 103 Gnss2SolLla
    py::class_<Registers::GNSS::Gnss2SolLla, MeasurementRegister> gnss2SolLla(GNSS, "Gnss2SolLla");
    gnss2SolLla.def(py::init<>())
    .def_readwrite("gps2Tow", &Registers::GNSS::Gnss2SolLla::gps2Tow)
    .def_readwrite("gps2Week", &Registers::GNSS::Gnss2SolLla::gps2Week)
    .def_readwrite("gnss2Fix", &Registers::GNSS::Gnss2SolLla::gnss2Fix)
    .def_readwrite("gnss2NumSats", &Registers::GNSS::Gnss2SolLla::gnss2NumSats)
    .def_readwrite("gnss2Lat", &Registers::GNSS::Gnss2SolLla::gnss2Lat)
    .def_readwrite("gnss2Lon", &Registers::GNSS::Gnss2SolLla::gnss2Lon)
    .def_readwrite("gnss2Alt", &Registers::GNSS::Gnss2SolLla::gnss2Alt)
    .def_readwrite("gnss2VelN", &Registers::GNSS::Gnss2SolLla::gnss2VelN)
    .def_readwrite("gnss2VelE", &Registers::GNSS::Gnss2SolLla::gnss2VelE)
    .def_readwrite("gnss2VelD", &Registers::GNSS::Gnss2SolLla::gnss2VelD)
    .def_readwrite("gnss2PosUncertaintyN", &Registers::GNSS::Gnss2SolLla::gnss2PosUncertaintyN)
    .def_readwrite("gnss2PosUncertaintyE", &Registers::GNSS::Gnss2SolLla::gnss2PosUncertaintyE)
    .def_readwrite("gnss2PosUncertaintyD", &Registers::GNSS::Gnss2SolLla::gnss2PosUncertaintyD)
    .def_readwrite("gnss2VelUncertainty", &Registers::GNSS::Gnss2SolLla::gnss2VelUncertainty)
    .def_readwrite("gnss2TimeUncertainty", &Registers::GNSS::Gnss2SolLla::gnss2TimeUncertainty)
    ;
    // Register 104 Gnss2SolEcef
    py::class_<Registers::GNSS::Gnss2SolEcef, MeasurementRegister> gnss2SolEcef(GNSS, "Gnss2SolEcef");
    gnss2SolEcef.def(py::init<>())
    .def_readwrite("gps2Tow", &Registers::GNSS::Gnss2SolEcef::gps2Tow)
    .def_readwrite("gps2Week", &Registers::GNSS::Gnss2SolEcef::gps2Week)
    .def_readwrite("gnss2Fix", &Registers::GNSS::Gnss2SolEcef::gnss2Fix)
    .def_readwrite("gnss2NumSats", &Registers::GNSS::Gnss2SolEcef::gnss2NumSats)
    .def_readwrite("gnss2PosX", &Registers::GNSS::Gnss2SolEcef::gnss2PosX)
    .def_readwrite("gnss2PosY", &Registers::GNSS::Gnss2SolEcef::gnss2PosY)
    .def_readwrite("gnss2PosZ", &Registers::GNSS::Gnss2SolEcef::gnss2PosZ)
    .def_readwrite("gnss2VelX", &Registers::GNSS::Gnss2SolEcef::gnss2VelX)
    .def_readwrite("gnss2VelY", &Registers::GNSS::Gnss2SolEcef::gnss2VelY)
    .def_readwrite("gnss2VelZ", &Registers::GNSS::Gnss2SolEcef::gnss2VelZ)
    .def_readwrite("gnss2PosUncertaintyX", &Registers::GNSS::Gnss2SolEcef::gnss2PosUncertaintyX)
    .def_readwrite("gnss2PosUncertaintyY", &Registers::GNSS::Gnss2SolEcef::gnss2PosUncertaintyY)
    .def_readwrite("gnss2PosUncertaintyZ", &Registers::GNSS::Gnss2SolEcef::gnss2PosUncertaintyZ)
    .def_readwrite("gnss2VelUncertainty", &Registers::GNSS::Gnss2SolEcef::gnss2VelUncertainty)
    .def_readwrite("gnss2TimeUncertainty", &Registers::GNSS::Gnss2SolEcef::gnss2TimeUncertainty)
    ;
    // Register 157 ExtGnssOffset
    py::class_<Registers::GNSS::ExtGnssOffset, ConfigurationRegister> extGnssOffset(GNSS, "ExtGnssOffset");
    extGnssOffset.def(py::init<>())
    .def_readwrite("positionX", &Registers::GNSS::ExtGnssOffset::positionX)
    .def_readwrite("positionY", &Registers::GNSS::ExtGnssOffset::positionY)
    .def_readwrite("positionZ", &Registers::GNSS::ExtGnssOffset::positionZ)
    ;
    // Module end GNSS
    
    // Module GNSSCompass
    py::module GNSSCompass = Registers.def_submodule("GNSSCompass", "GNSSCompass Submodule");
    // Register 86 GnssCompassSignalHealthStatus
    py::class_<Registers::GNSSCompass::GnssCompassSignalHealthStatus, MeasurementRegister> gnssCompassSignalHealthStatus(GNSSCompass, "GnssCompassSignalHealthStatus");
    gnssCompassSignalHealthStatus.def(py::init<>())
    .def_readwrite("numSatsPvtA", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numSatsPvtA)
    .def_readwrite("numSatsRtkA", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numSatsRtkA)
    .def_readwrite("highestCn0A", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::highestCn0A)
    .def_readwrite("numSatsPvtB", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numSatsPvtB)
    .def_readwrite("numSatsRtkB", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numSatsRtkB)
    .def_readwrite("highestCn0B", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::highestCn0B)
    .def_readwrite("numComSatsPvt", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numComSatsPvt)
    .def_readwrite("numComSatsRtk", &Registers::GNSSCompass::GnssCompassSignalHealthStatus::numComSatsRtk)
    ;
    // Register 93 GnssCompassBaseline
    py::class_<Registers::GNSSCompass::GnssCompassBaseline, ConfigurationRegister> gnssCompassBaseline(GNSSCompass, "GnssCompassBaseline");
    gnssCompassBaseline.def(py::init<>())
    .def_readwrite("positionX", &Registers::GNSSCompass::GnssCompassBaseline::positionX)
    .def_readwrite("positionY", &Registers::GNSSCompass::GnssCompassBaseline::positionY)
    .def_readwrite("positionZ", &Registers::GNSSCompass::GnssCompassBaseline::positionZ)
    .def_readwrite("uncertaintyX", &Registers::GNSSCompass::GnssCompassBaseline::uncertaintyX)
    .def_readwrite("uncertaintyY", &Registers::GNSSCompass::GnssCompassBaseline::uncertaintyY)
    .def_readwrite("uncertaintyZ", &Registers::GNSSCompass::GnssCompassBaseline::uncertaintyZ)
    ;
    // Register 97 GnssCompassEstBaseline
    py::class_<Registers::GNSSCompass::GnssCompassEstBaseline, MeasurementRegister> gnssCompassEstBaseline(GNSSCompass, "GnssCompassEstBaseline");
    gnssCompassEstBaseline.def(py::init<>())
    .def_readwrite("estBaselineComplete", &Registers::GNSSCompass::GnssCompassEstBaseline::estBaselineComplete)
    .def_readwrite("resv", &Registers::GNSSCompass::GnssCompassEstBaseline::resv)
    .def_readwrite("numMeas", &Registers::GNSSCompass::GnssCompassEstBaseline::numMeas)
    .def_readwrite("positionX", &Registers::GNSSCompass::GnssCompassEstBaseline::positionX)
    .def_readwrite("positionY", &Registers::GNSSCompass::GnssCompassEstBaseline::positionY)
    .def_readwrite("positionZ", &Registers::GNSSCompass::GnssCompassEstBaseline::positionZ)
    .def_readwrite("uncertaintyX", &Registers::GNSSCompass::GnssCompassEstBaseline::uncertaintyX)
    .def_readwrite("uncertaintyY", &Registers::GNSSCompass::GnssCompassEstBaseline::uncertaintyY)
    .def_readwrite("uncertaintyZ", &Registers::GNSSCompass::GnssCompassEstBaseline::uncertaintyZ)
    ;
    // Register 98 GnssCompassStartupStatus
    py::class_<Registers::GNSSCompass::GnssCompassStartupStatus, MeasurementRegister> gnssCompassStartupStatus(GNSSCompass, "GnssCompassStartupStatus");
    gnssCompassStartupStatus.def(py::init<>())
    .def_readwrite("percentComplete", &Registers::GNSSCompass::GnssCompassStartupStatus::percentComplete)
    .def_readwrite("currentHeading", &Registers::GNSSCompass::GnssCompassStartupStatus::currentHeading)
    ;
    // Module end GNSSCompass
    
    // Module HardSoftIronEstimator
    py::module HardSoftIronEstimator = Registers.def_submodule("HardSoftIronEstimator", "HardSoftIronEstimator Submodule");
    // Register 44 RealTimeHsiControl
    py::class_<Registers::HardSoftIronEstimator::RealTimeHsiControl, ConfigurationRegister> realTimeHsiControl(HardSoftIronEstimator, "RealTimeHsiControl");
    realTimeHsiControl.def(py::init<>())
    .def_readwrite("mode", &Registers::HardSoftIronEstimator::RealTimeHsiControl::mode)
    .def_readwrite("applyCompensation", &Registers::HardSoftIronEstimator::RealTimeHsiControl::applyCompensation)
    .def_readwrite("convergeRate", &Registers::HardSoftIronEstimator::RealTimeHsiControl::convergeRate)
    ;
    // Register 47 EstMagCal
    py::class_<Registers::HardSoftIronEstimator::EstMagCal, MeasurementRegister> estMagCal(HardSoftIronEstimator, "EstMagCal");
    estMagCal.def(py::init<>())
    .def_readwrite("magGain00", &Registers::HardSoftIronEstimator::EstMagCal::magGain00)
    .def_readwrite("magGain01", &Registers::HardSoftIronEstimator::EstMagCal::magGain01)
    .def_readwrite("magGain02", &Registers::HardSoftIronEstimator::EstMagCal::magGain02)
    .def_readwrite("magGain10", &Registers::HardSoftIronEstimator::EstMagCal::magGain10)
    .def_readwrite("magGain11", &Registers::HardSoftIronEstimator::EstMagCal::magGain11)
    .def_readwrite("magGain12", &Registers::HardSoftIronEstimator::EstMagCal::magGain12)
    .def_readwrite("magGain20", &Registers::HardSoftIronEstimator::EstMagCal::magGain20)
    .def_readwrite("magGain21", &Registers::HardSoftIronEstimator::EstMagCal::magGain21)
    .def_readwrite("magGain22", &Registers::HardSoftIronEstimator::EstMagCal::magGain22)
    .def_readwrite("magBiasX", &Registers::HardSoftIronEstimator::EstMagCal::magBiasX)
    .def_readwrite("magBiasY", &Registers::HardSoftIronEstimator::EstMagCal::magBiasY)
    .def_readwrite("magBiasZ", &Registers::HardSoftIronEstimator::EstMagCal::magBiasZ)
    ;
    // Module end HardSoftIronEstimator
    
    // Module Heave
    py::module Heave = Registers.def_submodule("Heave", "Heave Submodule");
    // Register 115 HeaveOutputs
    py::class_<Registers::Heave::HeaveOutputs, MeasurementRegister> heaveOutputs(Heave, "HeaveOutputs");
    heaveOutputs.def(py::init<>())
    .def_readwrite("heave", &Registers::Heave::HeaveOutputs::heave)
    .def_readwrite("heaveRate", &Registers::Heave::HeaveOutputs::heaveRate)
    .def_readwrite("delayedHeave", &Registers::Heave::HeaveOutputs::delayedHeave)
    ;
    // Register 116 HeaveBasicConfig
    py::class_<Registers::Heave::HeaveBasicConfig, ConfigurationRegister> heaveBasicConfig(Heave, "HeaveBasicConfig");
    heaveBasicConfig.def(py::init<>())
    .def_readwrite("initialWavePeriod", &Registers::Heave::HeaveBasicConfig::initialWavePeriod)
    .def_readwrite("initialWaveAmplitude", &Registers::Heave::HeaveBasicConfig::initialWaveAmplitude)
    .def_readwrite("maxWavePeriod", &Registers::Heave::HeaveBasicConfig::maxWavePeriod)
    .def_readwrite("minWaveAmplitude", &Registers::Heave::HeaveBasicConfig::minWaveAmplitude)
    .def_readwrite("delayedHeaveCutoffFreq", &Registers::Heave::HeaveBasicConfig::delayedHeaveCutoffFreq)
    .def_readwrite("heaveCutoffFreq", &Registers::Heave::HeaveBasicConfig::heaveCutoffFreq)
    .def_readwrite("heaveRateCutoffFreq", &Registers::Heave::HeaveBasicConfig::heaveRateCutoffFreq)
    ;
    // Module end Heave
    
    // Module IMU
    py::module IMU = Registers.def_submodule("IMU", "IMU Submodule");
    // Register 17 Mag
    py::class_<Registers::IMU::Mag, MeasurementRegister> mag(IMU, "Mag");
    mag.def(py::init<>())
    .def_readwrite("magX", &Registers::IMU::Mag::magX)
    .def_readwrite("magY", &Registers::IMU::Mag::magY)
    .def_readwrite("magZ", &Registers::IMU::Mag::magZ)
    ;
    // Register 18 Accel
    py::class_<Registers::IMU::Accel, MeasurementRegister> accel(IMU, "Accel");
    accel.def(py::init<>())
    .def_readwrite("accelX", &Registers::IMU::Accel::accelX)
    .def_readwrite("accelY", &Registers::IMU::Accel::accelY)
    .def_readwrite("accelZ", &Registers::IMU::Accel::accelZ)
    ;
    // Register 19 Gyro
    py::class_<Registers::IMU::Gyro, MeasurementRegister> gyro(IMU, "Gyro");
    gyro.def(py::init<>())
    .def_readwrite("gyroX", &Registers::IMU::Gyro::gyroX)
    .def_readwrite("gyroY", &Registers::IMU::Gyro::gyroY)
    .def_readwrite("gyroZ", &Registers::IMU::Gyro::gyroZ)
    ;
    // Register 20 MagAccelGyro
    py::class_<Registers::IMU::MagAccelGyro, MeasurementRegister> magAccelGyro(IMU, "MagAccelGyro");
    magAccelGyro.def(py::init<>())
    .def_readwrite("magX", &Registers::IMU::MagAccelGyro::magX)
    .def_readwrite("magY", &Registers::IMU::MagAccelGyro::magY)
    .def_readwrite("magZ", &Registers::IMU::MagAccelGyro::magZ)
    .def_readwrite("accelX", &Registers::IMU::MagAccelGyro::accelX)
    .def_readwrite("accelY", &Registers::IMU::MagAccelGyro::accelY)
    .def_readwrite("accelZ", &Registers::IMU::MagAccelGyro::accelZ)
    .def_readwrite("gyroX", &Registers::IMU::MagAccelGyro::gyroX)
    .def_readwrite("gyroY", &Registers::IMU::MagAccelGyro::gyroY)
    .def_readwrite("gyroZ", &Registers::IMU::MagAccelGyro::gyroZ)
    ;
    // Register 23 MagCal
    py::class_<Registers::IMU::MagCal, ConfigurationRegister> magCal(IMU, "MagCal");
    magCal.def(py::init<>())
    .def_readwrite("magGain00", &Registers::IMU::MagCal::magGain00)
    .def_readwrite("magGain01", &Registers::IMU::MagCal::magGain01)
    .def_readwrite("magGain02", &Registers::IMU::MagCal::magGain02)
    .def_readwrite("magGain10", &Registers::IMU::MagCal::magGain10)
    .def_readwrite("magGain11", &Registers::IMU::MagCal::magGain11)
    .def_readwrite("magGain12", &Registers::IMU::MagCal::magGain12)
    .def_readwrite("magGain20", &Registers::IMU::MagCal::magGain20)
    .def_readwrite("magGain21", &Registers::IMU::MagCal::magGain21)
    .def_readwrite("magGain22", &Registers::IMU::MagCal::magGain22)
    .def_readwrite("magBiasX", &Registers::IMU::MagCal::magBiasX)
    .def_readwrite("magBiasY", &Registers::IMU::MagCal::magBiasY)
    .def_readwrite("magBiasZ", &Registers::IMU::MagCal::magBiasZ)
    ;
    // Register 25 AccelCal
    py::class_<Registers::IMU::AccelCal, ConfigurationRegister> accelCal(IMU, "AccelCal");
    accelCal.def(py::init<>())
    .def_readwrite("accelGain00", &Registers::IMU::AccelCal::accelGain00)
    .def_readwrite("accelGain01", &Registers::IMU::AccelCal::accelGain01)
    .def_readwrite("accelGain02", &Registers::IMU::AccelCal::accelGain02)
    .def_readwrite("accelGain10", &Registers::IMU::AccelCal::accelGain10)
    .def_readwrite("accelGain11", &Registers::IMU::AccelCal::accelGain11)
    .def_readwrite("accelGain12", &Registers::IMU::AccelCal::accelGain12)
    .def_readwrite("accelGain20", &Registers::IMU::AccelCal::accelGain20)
    .def_readwrite("accelGain21", &Registers::IMU::AccelCal::accelGain21)
    .def_readwrite("accelGain22", &Registers::IMU::AccelCal::accelGain22)
    .def_readwrite("accelBiasX", &Registers::IMU::AccelCal::accelBiasX)
    .def_readwrite("accelBiasY", &Registers::IMU::AccelCal::accelBiasY)
    .def_readwrite("accelBiasZ", &Registers::IMU::AccelCal::accelBiasZ)
    ;
    // Register 26 RefFrameRot
    py::class_<Registers::IMU::RefFrameRot, ConfigurationRegister> refFrameRot(IMU, "RefFrameRot");
    refFrameRot.def(py::init<>())
    .def_readwrite("rfr00", &Registers::IMU::RefFrameRot::rfr00)
    .def_readwrite("rfr01", &Registers::IMU::RefFrameRot::rfr01)
    .def_readwrite("rfr02", &Registers::IMU::RefFrameRot::rfr02)
    .def_readwrite("rfr10", &Registers::IMU::RefFrameRot::rfr10)
    .def_readwrite("rfr11", &Registers::IMU::RefFrameRot::rfr11)
    .def_readwrite("rfr12", &Registers::IMU::RefFrameRot::rfr12)
    .def_readwrite("rfr20", &Registers::IMU::RefFrameRot::rfr20)
    .def_readwrite("rfr21", &Registers::IMU::RefFrameRot::rfr21)
    .def_readwrite("rfr22", &Registers::IMU::RefFrameRot::rfr22)
    ;
    // Register 54 ImuMeas
    py::class_<Registers::IMU::ImuMeas, MeasurementRegister> imuMeas(IMU, "ImuMeas");
    imuMeas.def(py::init<>())
    .def_readwrite("uncompMagX", &Registers::IMU::ImuMeas::uncompMagX)
    .def_readwrite("uncompMagY", &Registers::IMU::ImuMeas::uncompMagY)
    .def_readwrite("uncompMagZ", &Registers::IMU::ImuMeas::uncompMagZ)
    .def_readwrite("uncompAccX", &Registers::IMU::ImuMeas::uncompAccX)
    .def_readwrite("uncompAccY", &Registers::IMU::ImuMeas::uncompAccY)
    .def_readwrite("uncompAccZ", &Registers::IMU::ImuMeas::uncompAccZ)
    .def_readwrite("uncompGyroX", &Registers::IMU::ImuMeas::uncompGyroX)
    .def_readwrite("uncompGyroY", &Registers::IMU::ImuMeas::uncompGyroY)
    .def_readwrite("uncompGyroZ", &Registers::IMU::ImuMeas::uncompGyroZ)
    .def_readwrite("temperature", &Registers::IMU::ImuMeas::temperature)
    .def_readwrite("pressure", &Registers::IMU::ImuMeas::pressure)
    ;
    // Register 80 DeltaThetaVelocity
    py::class_<Registers::IMU::DeltaThetaVelocity, MeasurementRegister> deltaThetaVelocity(IMU, "DeltaThetaVelocity");
    deltaThetaVelocity.def(py::init<>())
    .def_readwrite("deltaTime", &Registers::IMU::DeltaThetaVelocity::deltaTime)
    .def_readwrite("deltaThetaX", &Registers::IMU::DeltaThetaVelocity::deltaThetaX)
    .def_readwrite("deltaThetaY", &Registers::IMU::DeltaThetaVelocity::deltaThetaY)
    .def_readwrite("deltaThetaZ", &Registers::IMU::DeltaThetaVelocity::deltaThetaZ)
    .def_readwrite("deltaVelX", &Registers::IMU::DeltaThetaVelocity::deltaVelX)
    .def_readwrite("deltaVelY", &Registers::IMU::DeltaThetaVelocity::deltaVelY)
    .def_readwrite("deltaVelZ", &Registers::IMU::DeltaThetaVelocity::deltaVelZ)
    ;
    // Register 82 DeltaThetaVelConfig
    py::class_<Registers::IMU::DeltaThetaVelConfig, ConfigurationRegister> deltaThetaVelConfig(IMU, "DeltaThetaVelConfig");
    deltaThetaVelConfig.def(py::init<>())
    .def_readwrite("integrationFrame", &Registers::IMU::DeltaThetaVelConfig::integrationFrame)
    .def_readwrite("gyroCompensation", &Registers::IMU::DeltaThetaVelConfig::gyroCompensation)
    .def_readwrite("accelCompensation", &Registers::IMU::DeltaThetaVelConfig::accelCompensation)
    .def_readwrite("earthRateCompensation", &Registers::IMU::DeltaThetaVelConfig::earthRateCompensation)
    .def_readwrite("resv", &Registers::IMU::DeltaThetaVelConfig::resv)
    ;
    // Register 84 GyroCal
    py::class_<Registers::IMU::GyroCal, ConfigurationRegister> gyroCal(IMU, "GyroCal");
    gyroCal.def(py::init<>())
    .def_readwrite("gyroGain00", &Registers::IMU::GyroCal::gyroGain00)
    .def_readwrite("gyroGain01", &Registers::IMU::GyroCal::gyroGain01)
    .def_readwrite("gyroGain02", &Registers::IMU::GyroCal::gyroGain02)
    .def_readwrite("gyroGain10", &Registers::IMU::GyroCal::gyroGain10)
    .def_readwrite("gyroGain11", &Registers::IMU::GyroCal::gyroGain11)
    .def_readwrite("gyroGain12", &Registers::IMU::GyroCal::gyroGain12)
    .def_readwrite("gyroGain20", &Registers::IMU::GyroCal::gyroGain20)
    .def_readwrite("gyroGain21", &Registers::IMU::GyroCal::gyroGain21)
    .def_readwrite("gyroGain22", &Registers::IMU::GyroCal::gyroGain22)
    .def_readwrite("gyroBiasX", &Registers::IMU::GyroCal::gyroBiasX)
    .def_readwrite("gyroBiasY", &Registers::IMU::GyroCal::gyroBiasY)
    .def_readwrite("gyroBiasZ", &Registers::IMU::GyroCal::gyroBiasZ)
    ;
    // Register 85 ImuFilterControl
    py::class_<Registers::IMU::ImuFilterControl, ConfigurationRegister> imuFilterControl(IMU, "ImuFilterControl");
    imuFilterControl.def(py::init<>())
    .def_readwrite("magWindowSize", &Registers::IMU::ImuFilterControl::magWindowSize)
    .def_readwrite("accelWindowSize", &Registers::IMU::ImuFilterControl::accelWindowSize)
    .def_readwrite("gyroWindowSize", &Registers::IMU::ImuFilterControl::gyroWindowSize)
    .def_readwrite("tempWindowSize", &Registers::IMU::ImuFilterControl::tempWindowSize)
    .def_readwrite("presWindowSize", &Registers::IMU::ImuFilterControl::presWindowSize)
    .def_property("magFilterMode",
    [](Registers::IMU::ImuFilterControl &self) -> Registers::IMU::ImuFilterControl::MagFilterMode& {
        if (!self.magFilterMode) {
            self.magFilterMode.emplace(0);
        }
        return *self.magFilterMode;
    },
    [](Registers::IMU::ImuFilterControl &self, const Registers::IMU::ImuFilterControl::MagFilterMode& value) {
        self.magFilterMode = value;
    }
    )
    .def_property("accelFilterMode",
    [](Registers::IMU::ImuFilterControl &self) -> Registers::IMU::ImuFilterControl::AccelFilterMode& {
        if (!self.accelFilterMode) {
            self.accelFilterMode.emplace(0);
        }
        return *self.accelFilterMode;
    },
    [](Registers::IMU::ImuFilterControl &self, const Registers::IMU::ImuFilterControl::AccelFilterMode& value) {
        self.accelFilterMode = value;
    }
    )
    .def_property("gyroFilterMode",
    [](Registers::IMU::ImuFilterControl &self) -> Registers::IMU::ImuFilterControl::GyroFilterMode& {
        if (!self.gyroFilterMode) {
            self.gyroFilterMode.emplace(0);
        }
        return *self.gyroFilterMode;
    },
    [](Registers::IMU::ImuFilterControl &self, const Registers::IMU::ImuFilterControl::GyroFilterMode& value) {
        self.gyroFilterMode = value;
    }
    )
    .def_property("tempFilterMode",
    [](Registers::IMU::ImuFilterControl &self) -> Registers::IMU::ImuFilterControl::TempFilterMode& {
        if (!self.tempFilterMode) {
            self.tempFilterMode.emplace(0);
        }
        return *self.tempFilterMode;
    },
    [](Registers::IMU::ImuFilterControl &self, const Registers::IMU::ImuFilterControl::TempFilterMode& value) {
        self.tempFilterMode = value;
    }
    )
    .def_property("presFilterMode",
    [](Registers::IMU::ImuFilterControl &self) -> Registers::IMU::ImuFilterControl::PresFilterMode& {
        if (!self.presFilterMode) {
            self.presFilterMode.emplace(0);
        }
        return *self.presFilterMode;
    },
    [](Registers::IMU::ImuFilterControl &self, const Registers::IMU::ImuFilterControl::PresFilterMode& value) {
        self.presFilterMode = value;
    }
    )
    ;
    py::class_<Registers::IMU::ImuFilterControl::MagFilterMode>(imuFilterControl, "MagFilterMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("uncomp", 
    [](Registers::IMU::ImuFilterControl::MagFilterMode& self) -> uint8_t
    {
        return self.uncomp;
    },
    [](Registers::IMU::ImuFilterControl::MagFilterMode& self, const uint8_t value) 
    {
        self.uncomp = value;
    })
    .def_property("comp", 
    [](Registers::IMU::ImuFilterControl::MagFilterMode& self) -> uint8_t
    {
        return self.comp;
    },
    [](Registers::IMU::ImuFilterControl::MagFilterMode& self, const uint8_t value) 
    {
        self.comp = value;
    })
    ;
    py::class_<Registers::IMU::ImuFilterControl::AccelFilterMode>(imuFilterControl, "AccelFilterMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("uncomp", 
    [](Registers::IMU::ImuFilterControl::AccelFilterMode& self) -> uint8_t
    {
        return self.uncomp;
    },
    [](Registers::IMU::ImuFilterControl::AccelFilterMode& self, const uint8_t value) 
    {
        self.uncomp = value;
    })
    .def_property("comp", 
    [](Registers::IMU::ImuFilterControl::AccelFilterMode& self) -> uint8_t
    {
        return self.comp;
    },
    [](Registers::IMU::ImuFilterControl::AccelFilterMode& self, const uint8_t value) 
    {
        self.comp = value;
    })
    ;
    py::class_<Registers::IMU::ImuFilterControl::GyroFilterMode>(imuFilterControl, "GyroFilterMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("uncomp", 
    [](Registers::IMU::ImuFilterControl::GyroFilterMode& self) -> uint8_t
    {
        return self.uncomp;
    },
    [](Registers::IMU::ImuFilterControl::GyroFilterMode& self, const uint8_t value) 
    {
        self.uncomp = value;
    })
    .def_property("comp", 
    [](Registers::IMU::ImuFilterControl::GyroFilterMode& self) -> uint8_t
    {
        return self.comp;
    },
    [](Registers::IMU::ImuFilterControl::GyroFilterMode& self, const uint8_t value) 
    {
        self.comp = value;
    })
    ;
    py::class_<Registers::IMU::ImuFilterControl::TempFilterMode>(imuFilterControl, "TempFilterMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("uncomp", 
    [](Registers::IMU::ImuFilterControl::TempFilterMode& self) -> uint8_t
    {
        return self.uncomp;
    },
    [](Registers::IMU::ImuFilterControl::TempFilterMode& self, const uint8_t value) 
    {
        self.uncomp = value;
    })
    .def_property("comp", 
    [](Registers::IMU::ImuFilterControl::TempFilterMode& self) -> uint8_t
    {
        return self.comp;
    },
    [](Registers::IMU::ImuFilterControl::TempFilterMode& self, const uint8_t value) 
    {
        self.comp = value;
    })
    ;
    py::class_<Registers::IMU::ImuFilterControl::PresFilterMode>(imuFilterControl, "PresFilterMode")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("uncomp", 
    [](Registers::IMU::ImuFilterControl::PresFilterMode& self) -> uint8_t
    {
        return self.uncomp;
    },
    [](Registers::IMU::ImuFilterControl::PresFilterMode& self, const uint8_t value) 
    {
        self.uncomp = value;
    })
    .def_property("comp", 
    [](Registers::IMU::ImuFilterControl::PresFilterMode& self) -> uint8_t
    {
        return self.comp;
    },
    [](Registers::IMU::ImuFilterControl::PresFilterMode& self, const uint8_t value) 
    {
        self.comp = value;
    })
    ;
    // Module end IMU
    
    // Module INS
    py::module INS = Registers.def_submodule("INS", "INS Submodule");
    // Register 63 InsSolLla
    py::class_<Registers::INS::InsSolLla, MeasurementRegister> insSolLla(INS, "InsSolLla");
    insSolLla.def(py::init<>())
    .def_readwrite("timeGpsTow", &Registers::INS::InsSolLla::timeGpsTow)
    .def_readwrite("timeGpsWeek", &Registers::INS::InsSolLla::timeGpsWeek)
    .def_readwrite("insStatus", &Registers::INS::InsSolLla::insStatus)
    .def_readwrite("yaw", &Registers::INS::InsSolLla::yaw)
    .def_readwrite("pitch", &Registers::INS::InsSolLla::pitch)
    .def_readwrite("roll", &Registers::INS::InsSolLla::roll)
    .def_readwrite("posLat", &Registers::INS::InsSolLla::posLat)
    .def_readwrite("posLon", &Registers::INS::InsSolLla::posLon)
    .def_readwrite("posAlt", &Registers::INS::InsSolLla::posAlt)
    .def_readwrite("velN", &Registers::INS::InsSolLla::velN)
    .def_readwrite("velE", &Registers::INS::InsSolLla::velE)
    .def_readwrite("velD", &Registers::INS::InsSolLla::velD)
    .def_readwrite("attUncertainty", &Registers::INS::InsSolLla::attUncertainty)
    .def_readwrite("posUncertainty", &Registers::INS::InsSolLla::posUncertainty)
    .def_readwrite("velUncertainty", &Registers::INS::InsSolLla::velUncertainty)
    ;
    // Register 64 InsSolEcef
    py::class_<Registers::INS::InsSolEcef, MeasurementRegister> insSolEcef(INS, "InsSolEcef");
    insSolEcef.def(py::init<>())
    .def_readwrite("timeGpsTow", &Registers::INS::InsSolEcef::timeGpsTow)
    .def_readwrite("timeGpsWeek", &Registers::INS::InsSolEcef::timeGpsWeek)
    .def_readwrite("insStatus", &Registers::INS::InsSolEcef::insStatus)
    .def_readwrite("yaw", &Registers::INS::InsSolEcef::yaw)
    .def_readwrite("pitch", &Registers::INS::InsSolEcef::pitch)
    .def_readwrite("roll", &Registers::INS::InsSolEcef::roll)
    .def_readwrite("posEx", &Registers::INS::InsSolEcef::posEx)
    .def_readwrite("posEy", &Registers::INS::InsSolEcef::posEy)
    .def_readwrite("posEz", &Registers::INS::InsSolEcef::posEz)
    .def_readwrite("velEx", &Registers::INS::InsSolEcef::velEx)
    .def_readwrite("velEy", &Registers::INS::InsSolEcef::velEy)
    .def_readwrite("velEz", &Registers::INS::InsSolEcef::velEz)
    .def_readwrite("attUncertainty", &Registers::INS::InsSolEcef::attUncertainty)
    .def_readwrite("posUncertainty", &Registers::INS::InsSolEcef::posUncertainty)
    .def_readwrite("velUncertainty", &Registers::INS::InsSolEcef::velUncertainty)
    ;
    // Register 67 InsBasicConfig
    py::class_<Registers::INS::InsBasicConfig, ConfigurationRegister> insBasicConfig(INS, "InsBasicConfig");
    insBasicConfig.def(py::init<>())
    .def_readwrite("scenario", &Registers::INS::InsBasicConfig::scenario)
    .def_readwrite("ahrsAiding", &Registers::INS::InsBasicConfig::ahrsAiding)
    .def_readwrite("estBaseline", &Registers::INS::InsBasicConfig::estBaseline)
    .def_readwrite("resv", &Registers::INS::InsBasicConfig::resv)
    ;
    // Register 72 InsStateLla
    py::class_<Registers::INS::InsStateLla, MeasurementRegister> insStateLla(INS, "InsStateLla");
    insStateLla.def(py::init<>())
    .def_readwrite("yaw", &Registers::INS::InsStateLla::yaw)
    .def_readwrite("pitch", &Registers::INS::InsStateLla::pitch)
    .def_readwrite("roll", &Registers::INS::InsStateLla::roll)
    .def_readwrite("posLat", &Registers::INS::InsStateLla::posLat)
    .def_readwrite("posLon", &Registers::INS::InsStateLla::posLon)
    .def_readwrite("posAlt", &Registers::INS::InsStateLla::posAlt)
    .def_readwrite("velN", &Registers::INS::InsStateLla::velN)
    .def_readwrite("velE", &Registers::INS::InsStateLla::velE)
    .def_readwrite("velD", &Registers::INS::InsStateLla::velD)
    .def_readwrite("accelX", &Registers::INS::InsStateLla::accelX)
    .def_readwrite("accelY", &Registers::INS::InsStateLla::accelY)
    .def_readwrite("accelZ", &Registers::INS::InsStateLla::accelZ)
    .def_readwrite("gyroX", &Registers::INS::InsStateLla::gyroX)
    .def_readwrite("gyroY", &Registers::INS::InsStateLla::gyroY)
    .def_readwrite("gyroZ", &Registers::INS::InsStateLla::gyroZ)
    ;
    // Register 73 InsStateEcef
    py::class_<Registers::INS::InsStateEcef, MeasurementRegister> insStateEcef(INS, "InsStateEcef");
    insStateEcef.def(py::init<>())
    .def_readwrite("yaw", &Registers::INS::InsStateEcef::yaw)
    .def_readwrite("pitch", &Registers::INS::InsStateEcef::pitch)
    .def_readwrite("roll", &Registers::INS::InsStateEcef::roll)
    .def_readwrite("posEx", &Registers::INS::InsStateEcef::posEx)
    .def_readwrite("posEy", &Registers::INS::InsStateEcef::posEy)
    .def_readwrite("posEz", &Registers::INS::InsStateEcef::posEz)
    .def_readwrite("velEx", &Registers::INS::InsStateEcef::velEx)
    .def_readwrite("velEy", &Registers::INS::InsStateEcef::velEy)
    .def_readwrite("velEz", &Registers::INS::InsStateEcef::velEz)
    .def_readwrite("accelX", &Registers::INS::InsStateEcef::accelX)
    .def_readwrite("accelY", &Registers::INS::InsStateEcef::accelY)
    .def_readwrite("accelZ", &Registers::INS::InsStateEcef::accelZ)
    .def_readwrite("gyroX", &Registers::INS::InsStateEcef::gyroX)
    .def_readwrite("gyroY", &Registers::INS::InsStateEcef::gyroY)
    .def_readwrite("gyroZ", &Registers::INS::InsStateEcef::gyroZ)
    ;
    // Register 74 FilterStartupBias
    py::class_<Registers::INS::FilterStartupBias, ConfigurationRegister> filterStartupBias(INS, "FilterStartupBias");
    filterStartupBias.def(py::init<>())
    .def_readwrite("gyroBiasX", &Registers::INS::FilterStartupBias::gyroBiasX)
    .def_readwrite("gyroBiasY", &Registers::INS::FilterStartupBias::gyroBiasY)
    .def_readwrite("gyroBiasZ", &Registers::INS::FilterStartupBias::gyroBiasZ)
    .def_readwrite("accelBiasX", &Registers::INS::FilterStartupBias::accelBiasX)
    .def_readwrite("accelBiasY", &Registers::INS::FilterStartupBias::accelBiasY)
    .def_readwrite("accelBiasZ", &Registers::INS::FilterStartupBias::accelBiasZ)
    .def_readwrite("presBias", &Registers::INS::FilterStartupBias::presBias)
    ;
    // Register 105 InsRefOffset
    py::class_<Registers::INS::InsRefOffset, ConfigurationRegister> insRefOffset(INS, "InsRefOffset");
    insRefOffset.def(py::init<>())
    .def_readwrite("refOffsetX", &Registers::INS::InsRefOffset::refOffsetX)
    .def_readwrite("refOffsetY", &Registers::INS::InsRefOffset::refOffsetY)
    .def_readwrite("refOffsetZ", &Registers::INS::InsRefOffset::refOffsetZ)
    .def_readwrite("refUncertX", &Registers::INS::InsRefOffset::refUncertX)
    .def_readwrite("refUncertY", &Registers::INS::InsRefOffset::refUncertY)
    .def_readwrite("refUncertZ", &Registers::INS::InsRefOffset::refUncertZ)
    ;
    // Register 144 InsGnssSelect
    py::class_<Registers::INS::InsGnssSelect, ConfigurationRegister> insGnssSelect(INS, "InsGnssSelect");
    insGnssSelect.def(py::init<>())
    .def_readwrite("activeReceiverSelect", &Registers::INS::InsGnssSelect::activeReceiverSelect)
    .def_readwrite("usedForNavTime", &Registers::INS::InsGnssSelect::usedForNavTime)
    .def_readwrite("hysteresisTime", &Registers::INS::InsGnssSelect::hysteresisTime)
    .def_readwrite("useGnssCompass", &Registers::INS::InsGnssSelect::useGnssCompass)
    .def_readwrite("resv1", &Registers::INS::InsGnssSelect::resv1)
    .def_readwrite("resv2", &Registers::INS::InsGnssSelect::resv2)
    ;
    // Module end INS
    
    // Module System
    py::module System = Registers.def_submodule("System", "System Submodule");
    // Register 0 UserTag
    py::class_<Registers::System::UserTag, ConfigurationRegister> userTag(System, "UserTag");
    userTag.def(py::init<>())
    .def_readwrite("tag", &Registers::System::UserTag::tag)
    ;
    // Register 1 Model
    py::class_<Registers::System::Model, MeasurementRegister> model(System, "Model");
    model.def(py::init<>())
    .def_readwrite("model", &Registers::System::Model::model)
    ;
    // Register 2 HwVer
    py::class_<Registers::System::HwVer, MeasurementRegister> hwVer(System, "HwVer");
    hwVer.def(py::init<>())
    .def_readwrite("hwVer", &Registers::System::HwVer::hwVer)
    .def_readwrite("hwMinVer", &Registers::System::HwVer::hwMinVer)
    ;
    // Register 3 Serial
    py::class_<Registers::System::Serial, MeasurementRegister> serial(System, "Serial");
    serial.def(py::init<>())
    .def_readwrite("serialNum", &Registers::System::Serial::serialNum)
    ;
    // Register 4 FwVer
    py::class_<Registers::System::FwVer, MeasurementRegister> fwVer(System, "FwVer");
    fwVer.def(py::init<>())
    .def_readwrite("fwVer", &Registers::System::FwVer::fwVer)
    ;
    // Register 5 BaudRate
    py::class_<Registers::System::BaudRate, ConfigurationRegister> baudRate(System, "BaudRate");
    baudRate.def(py::init<>())
    .def_readwrite("baudRate", &Registers::System::BaudRate::baudRate)
    .def_readwrite("serialPort", &Registers::System::BaudRate::serialPort)
    ;
    // Register 6 AsyncOutputType
    py::class_<Registers::System::AsyncOutputType, ConfigurationRegister> asyncOutputType(System, "AsyncOutputType");
    asyncOutputType.def(py::init<>())
    .def_readwrite("ador", &Registers::System::AsyncOutputType::ador)
    .def_readwrite("serialPort", &Registers::System::AsyncOutputType::serialPort)
    ;
    // Register 7 AsyncOutputFreq
    py::class_<Registers::System::AsyncOutputFreq, ConfigurationRegister> asyncOutputFreq(System, "AsyncOutputFreq");
    asyncOutputFreq.def(py::init<>())
    .def_readwrite("adof", &Registers::System::AsyncOutputFreq::adof)
    .def_readwrite("serialPort", &Registers::System::AsyncOutputFreq::serialPort)
    ;
    // Register 30 ProtocolControl
    py::class_<Registers::System::ProtocolControl, ConfigurationRegister> protocolControl(System, "ProtocolControl");
    protocolControl.def(py::init<>())
    .def_readwrite("asciiAppendCount", &Registers::System::ProtocolControl::asciiAppendCount)
    .def_readwrite("asciiAppendStatus", &Registers::System::ProtocolControl::asciiAppendStatus)
    .def_readwrite("spiAppendCount", &Registers::System::ProtocolControl::spiAppendCount)
    .def_readwrite("spiAppendStatus", &Registers::System::ProtocolControl::spiAppendStatus)
    .def_readwrite("asciiChecksum", &Registers::System::ProtocolControl::asciiChecksum)
    .def_readwrite("spiChecksum", &Registers::System::ProtocolControl::spiChecksum)
    .def_readwrite("errorMode", &Registers::System::ProtocolControl::errorMode)
    ;
    // Register 32 SyncControl
    py::class_<Registers::System::SyncControl, ConfigurationRegister> syncControl(System, "SyncControl");
    syncControl.def(py::init<>())
    .def_readwrite("syncInMode", &Registers::System::SyncControl::syncInMode)
    .def_readwrite("syncInEdge", &Registers::System::SyncControl::syncInEdge)
    .def_readwrite("syncInSkipFactor", &Registers::System::SyncControl::syncInSkipFactor)
    .def_readwrite("resv1", &Registers::System::SyncControl::resv1)
    .def_readwrite("syncOutMode", &Registers::System::SyncControl::syncOutMode)
    .def_readwrite("syncOutPolarity", &Registers::System::SyncControl::syncOutPolarity)
    .def_readwrite("syncOutSkipFactor", &Registers::System::SyncControl::syncOutSkipFactor)
    .def_readwrite("syncOutPulseWidth", &Registers::System::SyncControl::syncOutPulseWidth)
    .def_readwrite("resv2", &Registers::System::SyncControl::resv2)
    ;
    // Register 33 SyncStatus
    py::class_<Registers::System::SyncStatus, MeasurementRegister> syncStatus(System, "SyncStatus");
    syncStatus.def(py::init<>())
    .def_readwrite("syncInCount", &Registers::System::SyncStatus::syncInCount)
    .def_readwrite("syncInTime", &Registers::System::SyncStatus::syncInTime)
    .def_readwrite("syncOutCount", &Registers::System::SyncStatus::syncOutCount)
    ;
    // Binary Output Registers
    py::class_<Registers::System::BinaryOutputMeasurements> binaryOutputMeasurements(System, "BinaryOutputMeasurements", py::module_local());
    binaryOutputMeasurements.def(py::init<>())
    .def_readwrite("common", &Registers::System::BinaryOutputMeasurements::common)
    .def_readwrite("time", &Registers::System::BinaryOutputMeasurements::time)
    .def_readwrite("imu", &Registers::System::BinaryOutputMeasurements::imu)
    .def_readwrite("gnss", &Registers::System::BinaryOutputMeasurements::gnss)
    .def_readwrite("attitude", &Registers::System::BinaryOutputMeasurements::attitude)
    .def_readwrite("ins", &Registers::System::BinaryOutputMeasurements::ins)
    .def_readwrite("gnss2", &Registers::System::BinaryOutputMeasurements::gnss2)
    .def_readwrite("gnss3", &Registers::System::BinaryOutputMeasurements::gnss3)
    .def("toHeaderBytes", [](Registers::System::BinaryOutputMeasurements& cls) -> py::bytes {
        const auto headerBytes = cls.toHeaderBytes();
        std::vector<uint8_t> ret;
        for (auto byt : headerBytes) { ret.push_back(byt); }
        return py::bytes(reinterpret_cast<const char*>(ret.data()), ret.size());
    })
    ;
    // Common Group
    py::class_<Registers::System::BinaryOutputMeasurements::Common>(binaryOutputMeasurements, "Common")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("timeStartup",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.timeStartup;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.timeStartup = value;
    })
    .def_property("timeGps",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.timeGps;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.timeGps = value;
    })
    .def_property("timeSyncIn",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.timeSyncIn;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.timeSyncIn = value;
    })
    .def_property("ypr",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.ypr;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.ypr = value;
    })
    .def_property("quaternion",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.quaternion;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.quaternion = value;
    })
    .def_property("angularRate",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.angularRate;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.angularRate = value;
    })
    .def_property("posLla",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.posLla;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.posLla = value;
    })
    .def_property("velNed",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.velNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.velNed = value;
    })
    .def_property("accel",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.accel;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.accel = value;
    })
    .def_property("imu",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.imu;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.imu = value;
    })
    .def_property("magPres",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.magPres;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.magPres = value;
    })
    .def_property("deltas",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.deltas;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.deltas = value;
    })
    .def_property("insStatus",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.insStatus;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.insStatus = value;
    })
    .def_property("syncInCnt",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.syncInCnt;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.syncInCnt = value;
    })
    .def_property("timeGpsPps",
    [](Registers::System::BinaryOutputMeasurements::Common& self) -> uint32_t
    {
        return self.timeGpsPps;
    },
    [](Registers::System::BinaryOutputMeasurements::Common& self, const uint32_t value)
    {
        self.timeGpsPps = value;
    })
    ;
    
    // Time Group
    py::class_<Registers::System::BinaryOutputMeasurements::Time>(binaryOutputMeasurements, "Time")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("timeStartup",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeStartup;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeStartup = value;
    })
    .def_property("timeGps",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeGps;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeGps = value;
    })
    .def_property("timeGpsTow",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeGpsTow;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeGpsTow = value;
    })
    .def_property("timeGpsWeek",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeGpsWeek;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeGpsWeek = value;
    })
    .def_property("timeSyncIn",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeSyncIn;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeSyncIn = value;
    })
    .def_property("timeGpsPps",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeGpsPps;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeGpsPps = value;
    })
    .def_property("timeUtc",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeUtc;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeUtc = value;
    })
    .def_property("syncInCnt",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.syncInCnt;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.syncInCnt = value;
    })
    .def_property("syncOutCnt",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.syncOutCnt;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.syncOutCnt = value;
    })
    .def_property("timeStatus",
    [](Registers::System::BinaryOutputMeasurements::Time& self) -> uint32_t
    {
        return self.timeStatus;
    },
    [](Registers::System::BinaryOutputMeasurements::Time& self, const uint32_t value)
    {
        self.timeStatus = value;
    })
    ;
    
    // Imu Group
    py::class_<Registers::System::BinaryOutputMeasurements::Imu>(binaryOutputMeasurements, "Imu")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("imuStatus",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.imuStatus;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.imuStatus = value;
    })
    .def_property("uncompMag",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.uncompMag;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.uncompMag = value;
    })
    .def_property("uncompAccel",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.uncompAccel;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.uncompAccel = value;
    })
    .def_property("uncompGyro",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.uncompGyro;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.uncompGyro = value;
    })
    .def_property("temperature",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.temperature;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.temperature = value;
    })
    .def_property("pressure",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.pressure;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.pressure = value;
    })
    .def_property("deltaTheta",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.deltaTheta;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.deltaTheta = value;
    })
    .def_property("deltaVel",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.deltaVel;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.deltaVel = value;
    })
    .def_property("mag",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.mag;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.mag = value;
    })
    .def_property("accel",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.accel;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.accel = value;
    })
    .def_property("angularRate",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.angularRate;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.angularRate = value;
    })
    .def_property("sensSat",
    [](Registers::System::BinaryOutputMeasurements::Imu& self) -> uint32_t
    {
        return self.sensSat;
    },
    [](Registers::System::BinaryOutputMeasurements::Imu& self, const uint32_t value)
    {
        self.sensSat = value;
    })
    ;
    
    // Gnss Group
    py::class_<Registers::System::BinaryOutputMeasurements::Gnss>(binaryOutputMeasurements, "Gnss")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("gnss1TimeUtc",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1TimeUtc;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1TimeUtc = value;
    })
    .def_property("gps1Tow",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gps1Tow;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gps1Tow = value;
    })
    .def_property("gps1Week",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gps1Week;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gps1Week = value;
    })
    .def_property("gnss1NumSats",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1NumSats;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1NumSats = value;
    })
    .def_property("gnss1Fix",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1Fix;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1Fix = value;
    })
    .def_property("gnss1PosLla",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1PosLla;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1PosLla = value;
    })
    .def_property("gnss1PosEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1PosEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1PosEcef = value;
    })
    .def_property("gnss1VelNed",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1VelNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1VelNed = value;
    })
    .def_property("gnss1VelEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1VelEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1VelEcef = value;
    })
    .def_property("gnss1PosUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1PosUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1PosUncertainty = value;
    })
    .def_property("gnss1VelUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1VelUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1VelUncertainty = value;
    })
    .def_property("gnss1TimeUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1TimeUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1TimeUncertainty = value;
    })
    .def_property("gnss1TimeInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1TimeInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1TimeInfo = value;
    })
    .def_property("gnss1Dop",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1Dop;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1Dop = value;
    })
    .def_property("gnss1SatInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1SatInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1SatInfo = value;
    })
    .def_property("gnss1RawMeas",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1RawMeas;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1RawMeas = value;
    })
    .def_property("gnss1Status",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1Status;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1Status = value;
    })
    .def_property("gnss1AltMsl",
    [](Registers::System::BinaryOutputMeasurements::Gnss& self) -> uint32_t
    {
        return self.gnss1AltMsl;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss& self, const uint32_t value)
    {
        self.gnss1AltMsl = value;
    })
    ;
    
    // Attitude Group
    py::class_<Registers::System::BinaryOutputMeasurements::Attitude>(binaryOutputMeasurements, "Attitude")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("ypr",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.ypr;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.ypr = value;
    })
    .def_property("quaternion",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.quaternion;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.quaternion = value;
    })
    .def_property("dcm",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.dcm;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.dcm = value;
    })
    .def_property("magNed",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.magNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.magNed = value;
    })
    .def_property("accelNed",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.accelNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.accelNed = value;
    })
    .def_property("linBodyAcc",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.linBodyAcc;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.linBodyAcc = value;
    })
    .def_property("linAccelNed",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.linAccelNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.linAccelNed = value;
    })
    .def_property("yprU",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.yprU;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.yprU = value;
    })
    .def_property("heave",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.heave;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.heave = value;
    })
    .def_property("attU",
    [](Registers::System::BinaryOutputMeasurements::Attitude& self) -> uint32_t
    {
        return self.attU;
    },
    [](Registers::System::BinaryOutputMeasurements::Attitude& self, const uint32_t value)
    {
        self.attU = value;
    })
    ;
    
    // Ins Group
    py::class_<Registers::System::BinaryOutputMeasurements::Ins>(binaryOutputMeasurements, "Ins")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("insStatus",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.insStatus;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.insStatus = value;
    })
    .def_property("posLla",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.posLla;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.posLla = value;
    })
    .def_property("posEcef",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.posEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.posEcef = value;
    })
    .def_property("velBody",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.velBody;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.velBody = value;
    })
    .def_property("velNed",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.velNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.velNed = value;
    })
    .def_property("velEcef",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.velEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.velEcef = value;
    })
    .def_property("magEcef",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.magEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.magEcef = value;
    })
    .def_property("accelEcef",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.accelEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.accelEcef = value;
    })
    .def_property("linAccelEcef",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.linAccelEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.linAccelEcef = value;
    })
    .def_property("posU",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.posU;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.posU = value;
    })
    .def_property("velU",
    [](Registers::System::BinaryOutputMeasurements::Ins& self) -> uint32_t
    {
        return self.velU;
    },
    [](Registers::System::BinaryOutputMeasurements::Ins& self, const uint32_t value)
    {
        self.velU = value;
    })
    ;
    
    // Gnss2 Group
    py::class_<Registers::System::BinaryOutputMeasurements::Gnss2>(binaryOutputMeasurements, "Gnss2")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("gnss2TimeUtc",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2TimeUtc;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2TimeUtc = value;
    })
    .def_property("gps2Tow",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gps2Tow;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gps2Tow = value;
    })
    .def_property("gps2Week",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gps2Week;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gps2Week = value;
    })
    .def_property("gnss2NumSats",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2NumSats;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2NumSats = value;
    })
    .def_property("gnss2Fix",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2Fix;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2Fix = value;
    })
    .def_property("gnss2PosLla",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2PosLla;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2PosLla = value;
    })
    .def_property("gnss2PosEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2PosEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2PosEcef = value;
    })
    .def_property("gnss2VelNed",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2VelNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2VelNed = value;
    })
    .def_property("gnss2VelEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2VelEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2VelEcef = value;
    })
    .def_property("gnss2PosUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2PosUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2PosUncertainty = value;
    })
    .def_property("gnss2VelUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2VelUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2VelUncertainty = value;
    })
    .def_property("gnss2TimeUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2TimeUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2TimeUncertainty = value;
    })
    .def_property("gnss2TimeInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2TimeInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2TimeInfo = value;
    })
    .def_property("gnss2Dop",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2Dop;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2Dop = value;
    })
    .def_property("gnss2SatInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2SatInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2SatInfo = value;
    })
    .def_property("gnss2RawMeas",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2RawMeas;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2RawMeas = value;
    })
    .def_property("gnss2Status",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2Status;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2Status = value;
    })
    .def_property("gnss2AltMsl",
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self) -> uint32_t
    {
        return self.gnss2AltMsl;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss2& self, const uint32_t value)
    {
        self.gnss2AltMsl = value;
    })
    ;
    
    // Gnss3 Group
    py::class_<Registers::System::BinaryOutputMeasurements::Gnss3>(binaryOutputMeasurements, "Gnss3")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("gnss3TimeUtc",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3TimeUtc;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3TimeUtc = value;
    })
    .def_property("gps3Tow",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gps3Tow;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gps3Tow = value;
    })
    .def_property("gps3Week",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gps3Week;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gps3Week = value;
    })
    .def_property("gnss3NumSats",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3NumSats;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3NumSats = value;
    })
    .def_property("gnss3Fix",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3Fix;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3Fix = value;
    })
    .def_property("gnss3PosLla",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3PosLla;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3PosLla = value;
    })
    .def_property("gnss3PosEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3PosEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3PosEcef = value;
    })
    .def_property("gnss3VelNed",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3VelNed;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3VelNed = value;
    })
    .def_property("gnss3VelEcef",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3VelEcef;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3VelEcef = value;
    })
    .def_property("gnss3PosUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3PosUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3PosUncertainty = value;
    })
    .def_property("gnss3VelUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3VelUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3VelUncertainty = value;
    })
    .def_property("gnss3TimeUncertainty",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3TimeUncertainty;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3TimeUncertainty = value;
    })
    .def_property("gnss3TimeInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3TimeInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3TimeInfo = value;
    })
    .def_property("gnss3Dop",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3Dop;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3Dop = value;
    })
    .def_property("gnss3SatInfo",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3SatInfo;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3SatInfo = value;
    })
    .def_property("gnss3RawMeas",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3RawMeas;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3RawMeas = value;
    })
    .def_property("gnss3Status",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3Status;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3Status = value;
    })
    .def_property("gnss3AltMsl",
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self) -> uint32_t
    {
        return self.gnss3AltMsl;
    },
    [](Registers::System::BinaryOutputMeasurements::Gnss3& self, const uint32_t value)
    {
        self.gnss3AltMsl = value;
    })
    ;
    
    py::class_<Registers::System::BinaryOutput, ConfigurationRegister, Registers::System::BinaryOutputMeasurements> binaryOutput(System, "BinaryOutput");
    binaryOutput
    .def_property("asyncMode",
    [](Registers::System::BinaryOutput &self) -> Registers::System::BinaryOutput::AsyncMode& {
        if (!self.asyncMode) {
            self.asyncMode.emplace(0);
        }
        return *self.asyncMode;
    },
    [](Registers::System::BinaryOutput &self, const Registers::System::BinaryOutput::AsyncMode& value) {
        self.asyncMode = value;
    }
    )
    .def_readwrite("rateDivisor", &Registers::System::BinaryOutput::rateDivisor)
    ;
    
    py::class_<Registers::System::BinaryOutput::AsyncMode>(binaryOutput, "AsyncMode")
    .def(py::init<>())
    .def(py::init<const uint16_t&>())
    .def_property("serial1", 
    [](Registers::System::BinaryOutput::AsyncMode& self) -> uint16_t
    {
        return self.serial1;
    },
    [](Registers::System::BinaryOutput::AsyncMode& self, const uint16_t value) 
    {
        self.serial1 = value;
    })
    .def_property("serial2", 
    [](Registers::System::BinaryOutput::AsyncMode& self) -> uint16_t
    {
        return self.serial2;
    },
    [](Registers::System::BinaryOutput::AsyncMode& self, const uint16_t value) 
    {
        self.serial2 = value;
    })
    ;
    
    // Register 75 BinaryOutput1
    py::class_<Registers::System::BinaryOutput1, Registers::System::BinaryOutput>(System, "BinaryOutput1")
    .def(py::init<>());
    
    // Register 76 BinaryOutput2
    py::class_<Registers::System::BinaryOutput2, Registers::System::BinaryOutput>(System, "BinaryOutput2")
    .def(py::init<>());
    
    // Register 77 BinaryOutput3
    py::class_<Registers::System::BinaryOutput3, Registers::System::BinaryOutput>(System, "BinaryOutput3")
    .def(py::init<>());
    
    // Register 101 NmeaOutput1
    py::class_<Registers::System::NmeaOutput1, ConfigurationRegister> nmeaOutput1(System, "NmeaOutput1");
    nmeaOutput1.def(py::init<>())
    .def_readwrite("port", &Registers::System::NmeaOutput1::port)
    .def_readwrite("rate", &Registers::System::NmeaOutput1::rate)
    .def_readwrite("mode", &Registers::System::NmeaOutput1::mode)
    .def_readwrite("gnssSelect", &Registers::System::NmeaOutput1::gnssSelect)
    .def_property("msgSelection",
    [](Registers::System::NmeaOutput1 &self) -> Registers::System::NmeaOutput1::MsgSelection& {
        if (!self.msgSelection) {
            self.msgSelection.emplace(0);
        }
        return *self.msgSelection;
    },
    [](Registers::System::NmeaOutput1 &self, const Registers::System::NmeaOutput1::MsgSelection& value) {
        self.msgSelection = value;
    }
    )
    ;
    py::class_<Registers::System::NmeaOutput1::MsgSelection>(nmeaOutput1, "MsgSelection")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("rmcGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.rmcGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.rmcGnss = value;
    })
    .def_property("rmcIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.rmcIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.rmcIns = value;
    })
    .def_property("ggaGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.ggaGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.ggaGnss = value;
    })
    .def_property("ggaIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.ggaIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.ggaIns = value;
    })
    .def_property("gllGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.gllGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.gllGnss = value;
    })
    .def_property("gllIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.gllIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.gllIns = value;
    })
    .def_property("gsaGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.gsaGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.gsaGnss = value;
    })
    .def_property("gsvGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.gsvGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.gsvGnss = value;
    })
    .def_property("hdgIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.hdgIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.hdgIns = value;
    })
    .def_property("hdtIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.hdtIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.hdtIns = value;
    })
    .def_property("thsIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.thsIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.thsIns = value;
    })
    .def_property("vtgGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.vtgGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.vtgGnss = value;
    })
    .def_property("vtgIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.vtgIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.vtgIns = value;
    })
    .def_property("zdaGnss", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.zdaGnss;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.zdaGnss = value;
    })
    .def_property("zdaIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.zdaIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.zdaIns = value;
    })
    .def_property("pashrIns", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.pashrIns;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.pashrIns = value;
    })
    .def_property("tss1Ins", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.tss1Ins;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.tss1Ins = value;
    })
    .def_property("indyn", 
    [](Registers::System::NmeaOutput1::MsgSelection& self) -> uint32_t
    {
        return self.indyn;
    },
    [](Registers::System::NmeaOutput1::MsgSelection& self, const uint32_t value) 
    {
        self.indyn = value;
    })
    ;
    // Register 102 NmeaOutput2
    py::class_<Registers::System::NmeaOutput2, ConfigurationRegister> nmeaOutput2(System, "NmeaOutput2");
    nmeaOutput2.def(py::init<>())
    .def_readwrite("port", &Registers::System::NmeaOutput2::port)
    .def_readwrite("rate", &Registers::System::NmeaOutput2::rate)
    .def_readwrite("mode", &Registers::System::NmeaOutput2::mode)
    .def_readwrite("gnssSelect", &Registers::System::NmeaOutput2::gnssSelect)
    .def_property("msgSelection",
    [](Registers::System::NmeaOutput2 &self) -> Registers::System::NmeaOutput2::MsgSelection& {
        if (!self.msgSelection) {
            self.msgSelection.emplace(0);
        }
        return *self.msgSelection;
    },
    [](Registers::System::NmeaOutput2 &self, const Registers::System::NmeaOutput2::MsgSelection& value) {
        self.msgSelection = value;
    }
    )
    ;
    py::class_<Registers::System::NmeaOutput2::MsgSelection>(nmeaOutput2, "MsgSelection")
    .def(py::init<>())
    .def(py::init<const uint32_t&>())
    .def_property("rmcGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.rmcGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.rmcGnss = value;
    })
    .def_property("rmcIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.rmcIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.rmcIns = value;
    })
    .def_property("ggaGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.ggaGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.ggaGnss = value;
    })
    .def_property("ggaIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.ggaIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.ggaIns = value;
    })
    .def_property("gllGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.gllGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.gllGnss = value;
    })
    .def_property("gllIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.gllIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.gllIns = value;
    })
    .def_property("gsaGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.gsaGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.gsaGnss = value;
    })
    .def_property("gsvGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.gsvGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.gsvGnss = value;
    })
    .def_property("hdgIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.hdgIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.hdgIns = value;
    })
    .def_property("hdtIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.hdtIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.hdtIns = value;
    })
    .def_property("thsIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.thsIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.thsIns = value;
    })
    .def_property("vtgGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.vtgGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.vtgGnss = value;
    })
    .def_property("vtgIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.vtgIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.vtgIns = value;
    })
    .def_property("zdaGnss", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.zdaGnss;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.zdaGnss = value;
    })
    .def_property("zdaIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.zdaIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.zdaIns = value;
    })
    .def_property("pashrIns", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.pashrIns;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.pashrIns = value;
    })
    .def_property("tss1Ins", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.tss1Ins;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.tss1Ins = value;
    })
    .def_property("indyn", 
    [](Registers::System::NmeaOutput2::MsgSelection& self) -> uint32_t
    {
        return self.indyn;
    },
    [](Registers::System::NmeaOutput2::MsgSelection& self, const uint32_t value) 
    {
        self.indyn = value;
    })
    ;
    // Register 206 LegacyCompatibilitySettings
    py::class_<Registers::System::LegacyCompatibilitySettings, ConfigurationRegister> legacyCompatibilitySettings(System, "LegacyCompatibilitySettings");
    legacyCompatibilitySettings.def(py::init<>())
    .def_readwrite("insLegacy", &Registers::System::LegacyCompatibilitySettings::insLegacy)
    .def_property("gnssLegacy",
    [](Registers::System::LegacyCompatibilitySettings &self) -> Registers::System::LegacyCompatibilitySettings::GnssLegacy& {
        if (!self.gnssLegacy) {
            self.gnssLegacy.emplace(0);
        }
        return *self.gnssLegacy;
    },
    [](Registers::System::LegacyCompatibilitySettings &self, const Registers::System::LegacyCompatibilitySettings::GnssLegacy& value) {
        self.gnssLegacy = value;
    }
    )
    .def_readwrite("imuLegacy", &Registers::System::LegacyCompatibilitySettings::imuLegacy)
    .def_readwrite("hwLegacy", &Registers::System::LegacyCompatibilitySettings::hwLegacy)
    ;
    py::class_<Registers::System::LegacyCompatibilitySettings::GnssLegacy>(legacyCompatibilitySettings, "GnssLegacy")
    .def(py::init<>())
    .def(py::init<const uint8_t&>())
    .def_property("legacyGnssFix", 
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self) -> uint8_t
    {
        return self.legacyGnssFix;
    },
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self, const uint8_t value) 
    {
        self.legacyGnssFix = value;
    })
    .def_property("requireReg55Reset", 
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self) -> uint8_t
    {
        return self.requireReg55Reset;
    },
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self, const uint8_t value) 
    {
        self.requireReg55Reset = value;
    })
    .def_property("alwaysPpsPulse", 
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self) -> uint8_t
    {
        return self.alwaysPpsPulse;
    },
    [](Registers::System::LegacyCompatibilitySettings::GnssLegacy& self, const uint8_t value) 
    {
        self.alwaysPpsPulse = value;
    })
    ;
    // Module end System
    
    // Module VelocityAiding
    py::module VelocityAiding = Registers.def_submodule("VelocityAiding", "VelocityAiding Submodule");
    // Register 50 VelAidingMeas
    py::class_<Registers::VelocityAiding::VelAidingMeas, ConfigurationRegister> velAidingMeas(VelocityAiding, "VelAidingMeas");
    velAidingMeas.def(py::init<>())
    .def_readwrite("velocityX", &Registers::VelocityAiding::VelAidingMeas::velocityX)
    .def_readwrite("velocityY", &Registers::VelocityAiding::VelAidingMeas::velocityY)
    .def_readwrite("velocityZ", &Registers::VelocityAiding::VelAidingMeas::velocityZ)
    ;
    // Register 51 VelAidingControl
    py::class_<Registers::VelocityAiding::VelAidingControl, ConfigurationRegister> velAidingControl(VelocityAiding, "VelAidingControl");
    velAidingControl.def(py::init<>())
    .def_readwrite("velAidEnable", &Registers::VelocityAiding::VelAidingControl::velAidEnable)
    .def_readwrite("velUncertTuning", &Registers::VelocityAiding::VelAidingControl::velUncertTuning)
    .def_readwrite("resv", &Registers::VelocityAiding::VelAidingControl::resv)
    ;
    // Module end VelocityAiding
    
    // Module WorldMagGravityModel
    py::module WorldMagGravityModel = Registers.def_submodule("WorldMagGravityModel", "WorldMagGravityModel Submodule");
    // Register 83 RefModelConfig
    py::class_<Registers::WorldMagGravityModel::RefModelConfig, ConfigurationRegister> refModelConfig(WorldMagGravityModel, "RefModelConfig");
    refModelConfig.def(py::init<>())
    .def_readwrite("enableMagModel", &Registers::WorldMagGravityModel::RefModelConfig::enableMagModel)
    .def_readwrite("enableGravityModel", &Registers::WorldMagGravityModel::RefModelConfig::enableGravityModel)
    .def_readwrite("resv1", &Registers::WorldMagGravityModel::RefModelConfig::resv1)
    .def_readwrite("resv2", &Registers::WorldMagGravityModel::RefModelConfig::resv2)
    .def_readwrite("recalcThreshold", &Registers::WorldMagGravityModel::RefModelConfig::recalcThreshold)
    .def_readwrite("year", &Registers::WorldMagGravityModel::RefModelConfig::year)
    .def_readwrite("latitude", &Registers::WorldMagGravityModel::RefModelConfig::latitude)
    .def_readwrite("longitude", &Registers::WorldMagGravityModel::RefModelConfig::longitude)
    .def_readwrite("altitude", &Registers::WorldMagGravityModel::RefModelConfig::altitude)
    ;
    
    // Enumeration for Register 35 VpeBasicControl HeadingMode
    py::enum_<Registers::Attitude::VpeBasicControl::HeadingMode>(vpeBasicControl, "HeadingMode")
    .value("Absolute", Registers::Attitude::VpeBasicControl::HeadingMode::Absolute)
    .value("Relative", Registers::Attitude::VpeBasicControl::HeadingMode::Relative)
    .value("Indoor", Registers::Attitude::VpeBasicControl::HeadingMode::Indoor)
    ;
    // Enumeration for Register 35 VpeBasicControl FilteringMode
    py::enum_<Registers::Attitude::VpeBasicControl::FilteringMode>(vpeBasicControl, "FilteringMode")
    .value("Unfiltered", Registers::Attitude::VpeBasicControl::FilteringMode::Unfiltered)
    .value("AdaptivelyFiltered", Registers::Attitude::VpeBasicControl::FilteringMode::AdaptivelyFiltered)
    ;
    // Enumeration for Register 35 VpeBasicControl TuningMode
    py::enum_<Registers::Attitude::VpeBasicControl::TuningMode>(vpeBasicControl, "TuningMode")
    .value("Static", Registers::Attitude::VpeBasicControl::TuningMode::Static)
    .value("Adaptive", Registers::Attitude::VpeBasicControl::TuningMode::Adaptive)
    ;
    // Enumeration for Register 55 GnssBasicConfig ReceiverEnable
    py::enum_<Registers::GNSS::GnssBasicConfig::ReceiverEnable>(gnssBasicConfig, "ReceiverEnable")
    .value("Internal", Registers::GNSS::GnssBasicConfig::ReceiverEnable::Internal)
    .value("VnWrite", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnWrite)
    .value("VnAdor", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnAdor)
    .value("GnssA", Registers::GNSS::GnssBasicConfig::ReceiverEnable::GnssA)
    .value("VnWriteAndGnssA", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnWriteAndGnssA)
    .value("VnAdorAndGnssA", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnAdorAndGnssA)
    .value("VnWriteAndGnssAB", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnWriteAndGnssAB)
    .value("VnAdorAndGnssAB", Registers::GNSS::GnssBasicConfig::ReceiverEnable::VnAdorAndGnssAB)
    ;
    // Enumeration for Register 55 GnssBasicConfig PpsSource
    py::enum_<Registers::GNSS::GnssBasicConfig::PpsSource>(gnssBasicConfig, "PpsSource")
    .value("PpsPinRising", Registers::GNSS::GnssBasicConfig::PpsSource::PpsPinRising)
    .value("PpsPinFalling", Registers::GNSS::GnssBasicConfig::PpsSource::PpsPinFalling)
    .value("SyncInRising", Registers::GNSS::GnssBasicConfig::PpsSource::SyncInRising)
    .value("SyncInFalling", Registers::GNSS::GnssBasicConfig::PpsSource::SyncInFalling)
    .value("none", Registers::GNSS::GnssBasicConfig::PpsSource::None)
    ;
    // Enumeration for Register 55 GnssBasicConfig Rate
    py::enum_<Registers::GNSS::GnssBasicConfig::Rate>(gnssBasicConfig, "Rate")
    .value("Rate1Hz", Registers::GNSS::GnssBasicConfig::Rate::Rate1Hz)
    .value("Rate5Hz", Registers::GNSS::GnssBasicConfig::Rate::Rate5Hz)
    ;
    // Enumeration for Register 55 GnssBasicConfig AntPower
    py::enum_<Registers::GNSS::GnssBasicConfig::AntPower>(gnssBasicConfig, "AntPower")
    .value("Off", Registers::GNSS::GnssBasicConfig::AntPower::Off)
    .value("Internal", Registers::GNSS::GnssBasicConfig::AntPower::Internal)
    .value("External", Registers::GNSS::GnssBasicConfig::AntPower::External)
    ;
    // Enumeration for Register 58 GnssSolLla Gnss1Fix
    py::enum_<Registers::GNSS::GnssSolLla::Gnss1Fix>(gnssSolLla, "Gnss1Fix")
    .value("NoFix", Registers::GNSS::GnssSolLla::Gnss1Fix::NoFix)
    .value("TimeFix", Registers::GNSS::GnssSolLla::Gnss1Fix::TimeFix)
    .value("Fix2D", Registers::GNSS::GnssSolLla::Gnss1Fix::Fix2D)
    .value("Fix3D", Registers::GNSS::GnssSolLla::Gnss1Fix::Fix3D)
    .value("SBAS", Registers::GNSS::GnssSolLla::Gnss1Fix::SBAS)
    .value("RtkFloat", Registers::GNSS::GnssSolLla::Gnss1Fix::RtkFloat)
    .value("RtkFix", Registers::GNSS::GnssSolLla::Gnss1Fix::RtkFix)
    ;
    // Enumeration for Register 59 GnssSolEcef Gnss1Fix
    py::enum_<Registers::GNSS::GnssSolEcef::Gnss1Fix>(gnssSolEcef, "Gnss1Fix")
    .value("NoFix", Registers::GNSS::GnssSolEcef::Gnss1Fix::NoFix)
    .value("TimeFix", Registers::GNSS::GnssSolEcef::Gnss1Fix::TimeFix)
    .value("Fix2D", Registers::GNSS::GnssSolEcef::Gnss1Fix::Fix2D)
    .value("Fix3D", Registers::GNSS::GnssSolEcef::Gnss1Fix::Fix3D)
    .value("SBAS", Registers::GNSS::GnssSolEcef::Gnss1Fix::SBAS)
    .value("RtkFloat", Registers::GNSS::GnssSolEcef::Gnss1Fix::RtkFloat)
    .value("RtkFix", Registers::GNSS::GnssSolEcef::Gnss1Fix::RtkFix)
    ;
    // Enumeration for Register 99 GnssSystemConfig ReceiverSelect
    py::enum_<Registers::GNSS::GnssSystemConfig::ReceiverSelect>(gnssSystemConfig, "ReceiverSelect")
    .value("GnssAB", Registers::GNSS::GnssSystemConfig::ReceiverSelect::GnssAB)
    .value("GnssA", Registers::GNSS::GnssSystemConfig::ReceiverSelect::GnssA)
    .value("GnssB", Registers::GNSS::GnssSystemConfig::ReceiverSelect::GnssB)
    ;
    // Enumeration for Register 100 GnssSyncConfig GnssSyncEnable
    py::enum_<Registers::GNSS::GnssSyncConfig::GnssSyncEnable>(gnssSyncConfig, "GnssSyncEnable")
    .value("Off", Registers::GNSS::GnssSyncConfig::GnssSyncEnable::Off)
    .value("AlwaysOn", Registers::GNSS::GnssSyncConfig::GnssSyncEnable::AlwaysOn)
    .value("OnWhenLocked", Registers::GNSS::GnssSyncConfig::GnssSyncEnable::OnWhenLocked)
    ;
    // Enumeration for Register 100 GnssSyncConfig Polarity
    py::enum_<Registers::GNSS::GnssSyncConfig::Polarity>(gnssSyncConfig, "Polarity")
    .value("FallingEdge", Registers::GNSS::GnssSyncConfig::Polarity::FallingEdge)
    .value("RisingEdge", Registers::GNSS::GnssSyncConfig::Polarity::RisingEdge)
    ;
    // Enumeration for Register 100 GnssSyncConfig SpecType
    py::enum_<Registers::GNSS::GnssSyncConfig::SpecType>(gnssSyncConfig, "SpecType")
    .value("PeriodPulseWidth", Registers::GNSS::GnssSyncConfig::SpecType::PeriodPulseWidth)
    .value("FreqDutyCycle", Registers::GNSS::GnssSyncConfig::SpecType::FreqDutyCycle)
    ;
    // Enumeration for Register 103 Gnss2SolLla Gnss2Fix
    py::enum_<Registers::GNSS::Gnss2SolLla::Gnss2Fix>(gnss2SolLla, "Gnss2Fix")
    .value("NoFix", Registers::GNSS::Gnss2SolLla::Gnss2Fix::NoFix)
    .value("TimeFix", Registers::GNSS::Gnss2SolLla::Gnss2Fix::TimeFix)
    .value("Fix2D", Registers::GNSS::Gnss2SolLla::Gnss2Fix::Fix2D)
    .value("Fix3D", Registers::GNSS::Gnss2SolLla::Gnss2Fix::Fix3D)
    .value("SBAS", Registers::GNSS::Gnss2SolLla::Gnss2Fix::SBAS)
    .value("RtkFloat", Registers::GNSS::Gnss2SolLla::Gnss2Fix::RtkFloat)
    .value("RtkFix", Registers::GNSS::Gnss2SolLla::Gnss2Fix::RtkFix)
    ;
    // Enumeration for Register 104 Gnss2SolEcef Gnss2Fix
    py::enum_<Registers::GNSS::Gnss2SolEcef::Gnss2Fix>(gnss2SolEcef, "Gnss2Fix")
    .value("NoFix", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::NoFix)
    .value("TimeFix", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::TimeFix)
    .value("Fix2D", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::Fix2D)
    .value("Fix3D", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::Fix3D)
    .value("SBAS", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::SBAS)
    .value("RtkFloat", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::RtkFloat)
    .value("RtkFix", Registers::GNSS::Gnss2SolEcef::Gnss2Fix::RtkFix)
    ;
    // Enumeration for Register 44 RealTimeHsiControl Mode
    py::enum_<Registers::HardSoftIronEstimator::RealTimeHsiControl::Mode>(realTimeHsiControl, "Mode")
    .value("Off", Registers::HardSoftIronEstimator::RealTimeHsiControl::Mode::Off)
    .value("Run", Registers::HardSoftIronEstimator::RealTimeHsiControl::Mode::Run)
    .value("Reset", Registers::HardSoftIronEstimator::RealTimeHsiControl::Mode::Reset)
    ;
    // Enumeration for Register 44 RealTimeHsiControl ApplyCompensation
    py::enum_<Registers::HardSoftIronEstimator::RealTimeHsiControl::ApplyCompensation>(realTimeHsiControl, "ApplyCompensation")
    .value("Disable", Registers::HardSoftIronEstimator::RealTimeHsiControl::ApplyCompensation::Disable)
    .value("Enable", Registers::HardSoftIronEstimator::RealTimeHsiControl::ApplyCompensation::Enable)
    ;
    // Enumeration for Register 82 DeltaThetaVelConfig IntegrationFrame
    py::enum_<Registers::IMU::DeltaThetaVelConfig::IntegrationFrame>(deltaThetaVelConfig, "IntegrationFrame")
    .value("Body", Registers::IMU::DeltaThetaVelConfig::IntegrationFrame::Body)
    .value("NED", Registers::IMU::DeltaThetaVelConfig::IntegrationFrame::NED)
    ;
    // Enumeration for Register 82 DeltaThetaVelConfig GyroCompensation
    py::enum_<Registers::IMU::DeltaThetaVelConfig::GyroCompensation>(deltaThetaVelConfig, "GyroCompensation")
    .value("none", Registers::IMU::DeltaThetaVelConfig::GyroCompensation::None)
    .value("Bias", Registers::IMU::DeltaThetaVelConfig::GyroCompensation::Bias)
    ;
    // Enumeration for Register 82 DeltaThetaVelConfig AccelCompensation
    py::enum_<Registers::IMU::DeltaThetaVelConfig::AccelCompensation>(deltaThetaVelConfig, "AccelCompensation")
    .value("none", Registers::IMU::DeltaThetaVelConfig::AccelCompensation::None)
    .value("Gravity", Registers::IMU::DeltaThetaVelConfig::AccelCompensation::Gravity)
    .value("Bias", Registers::IMU::DeltaThetaVelConfig::AccelCompensation::Bias)
    .value("BiasAndGravity", Registers::IMU::DeltaThetaVelConfig::AccelCompensation::BiasAndGravity)
    ;
    // Enumeration for Register 82 DeltaThetaVelConfig EarthRateCompensation
    py::enum_<Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation>(deltaThetaVelConfig, "EarthRateCompensation")
    .value("none", Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation::None)
    .value("GyroRate", Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation::GyroRate)
    .value("CoriolisAccel", Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation::CoriolisAccel)
    .value("RateAndCoriolis", Registers::IMU::DeltaThetaVelConfig::EarthRateCompensation::RateAndCoriolis)
    ;
    // Enumeration for Register 67 InsBasicConfig Scenario
    py::enum_<Registers::INS::InsBasicConfig::Scenario>(insBasicConfig, "Scenario")
    .value("Ahrs", Registers::INS::InsBasicConfig::Scenario::Ahrs)
    .value("GnssInsWithPressure", Registers::INS::InsBasicConfig::Scenario::GnssInsWithPressure)
    .value("GnssInsNoPressure", Registers::INS::InsBasicConfig::Scenario::GnssInsNoPressure)
    .value("DualGnssNoPressure", Registers::INS::InsBasicConfig::Scenario::DualGnssNoPressure)
    .value("DualGnssWithPressure", Registers::INS::InsBasicConfig::Scenario::DualGnssWithPressure)
    ;
    // Enumeration for Register 67 InsBasicConfig AhrsAiding
    py::enum_<Registers::INS::InsBasicConfig::AhrsAiding>(insBasicConfig, "AhrsAiding")
    .value("Disable", Registers::INS::InsBasicConfig::AhrsAiding::Disable)
    .value("Enable", Registers::INS::InsBasicConfig::AhrsAiding::Enable)
    ;
    // Enumeration for Register 67 InsBasicConfig EstBaseline
    py::enum_<Registers::INS::InsBasicConfig::EstBaseline>(insBasicConfig, "EstBaseline")
    .value("Disable", Registers::INS::InsBasicConfig::EstBaseline::Disable)
    .value("Enable", Registers::INS::InsBasicConfig::EstBaseline::Enable)
    ;
    // Enumeration for Register 144 InsGnssSelect ActiveReceiverSelect
    py::enum_<Registers::INS::InsGnssSelect::ActiveReceiverSelect>(insGnssSelect, "ActiveReceiverSelect")
    .value("none", Registers::INS::InsGnssSelect::ActiveReceiverSelect::None)
    .value("PrimaryReceiver", Registers::INS::InsGnssSelect::ActiveReceiverSelect::PrimaryReceiver)
    .value("SecondaryReceiver", Registers::INS::InsGnssSelect::ActiveReceiverSelect::SecondaryReceiver)
    .value("TertiaryReceiver", Registers::INS::InsGnssSelect::ActiveReceiverSelect::TertiaryReceiver)
    .value("FallbackOnFailure", Registers::INS::InsGnssSelect::ActiveReceiverSelect::FallbackOnFailure)
    ;
    // Enumeration for Register 144 InsGnssSelect UseGnssCompass
    py::enum_<Registers::INS::InsGnssSelect::UseGnssCompass>(insGnssSelect, "UseGnssCompass")
    .value("Off", Registers::INS::InsGnssSelect::UseGnssCompass::Off)
    .value("On", Registers::INS::InsGnssSelect::UseGnssCompass::On)
    ;
    // Enumeration for Register 5 BaudRate BaudRate
    py::enum_<Registers::System::BaudRate::BaudRates>(baudRate, "BaudRates")
    .value("Baud9600", Registers::System::BaudRate::BaudRates::Baud9600)
    .value("Baud19200", Registers::System::BaudRate::BaudRates::Baud19200)
    .value("Baud38400", Registers::System::BaudRate::BaudRates::Baud38400)
    .value("Baud57600", Registers::System::BaudRate::BaudRates::Baud57600)
    .value("Baud115200", Registers::System::BaudRate::BaudRates::Baud115200)
    .value("Baud128000", Registers::System::BaudRate::BaudRates::Baud128000)
    .value("Baud230400", Registers::System::BaudRate::BaudRates::Baud230400)
    .value("Baud460800", Registers::System::BaudRate::BaudRates::Baud460800)
    .value("Baud921600", Registers::System::BaudRate::BaudRates::Baud921600)
    ;
    // Enumeration for Register 5 BaudRate SerialPort
    py::enum_<Registers::System::BaudRate::SerialPort>(baudRate, "SerialPort")
    .value("ActiveSerial", Registers::System::BaudRate::SerialPort::ActiveSerial)
    .value("Serial1", Registers::System::BaudRate::SerialPort::Serial1)
    .value("Serial2", Registers::System::BaudRate::SerialPort::Serial2)
    .value("Poll", Registers::System::BaudRate::SerialPort::Poll)
    ;
    // Enumeration for Register 6 AsyncOutputType Ador
    py::enum_<Registers::System::AsyncOutputType::Ador>(asyncOutputType, "Ador")
    .value("OFF", Registers::System::AsyncOutputType::Ador::OFF)
    .value("YPR", Registers::System::AsyncOutputType::Ador::YPR)
    .value("QTN", Registers::System::AsyncOutputType::Ador::QTN)
    .value("QMR", Registers::System::AsyncOutputType::Ador::QMR)
    .value("MAG", Registers::System::AsyncOutputType::Ador::MAG)
    .value("ACC", Registers::System::AsyncOutputType::Ador::ACC)
    .value("GYR", Registers::System::AsyncOutputType::Ador::GYR)
    .value("MAR", Registers::System::AsyncOutputType::Ador::MAR)
    .value("YMR", Registers::System::AsyncOutputType::Ador::YMR)
    .value("YBA", Registers::System::AsyncOutputType::Ador::YBA)
    .value("YIA", Registers::System::AsyncOutputType::Ador::YIA)
    .value("IMU", Registers::System::AsyncOutputType::Ador::IMU)
    .value("GPS", Registers::System::AsyncOutputType::Ador::GPS)
    .value("GPE", Registers::System::AsyncOutputType::Ador::GPE)
    .value("INS", Registers::System::AsyncOutputType::Ador::INS)
    .value("INE", Registers::System::AsyncOutputType::Ador::INE)
    .value("ISL", Registers::System::AsyncOutputType::Ador::ISL)
    .value("ISE", Registers::System::AsyncOutputType::Ador::ISE)
    .value("DTV", Registers::System::AsyncOutputType::Ador::DTV)
    .value("G2S", Registers::System::AsyncOutputType::Ador::G2S)
    .value("G2E", Registers::System::AsyncOutputType::Ador::G2E)
    .value("HVE", Registers::System::AsyncOutputType::Ador::HVE)
    ;
    // Enumeration for Register 6 AsyncOutputType SerialPort
    py::enum_<Registers::System::AsyncOutputType::SerialPort>(asyncOutputType, "SerialPort")
    .value("ActiveSerial", Registers::System::AsyncOutputType::SerialPort::ActiveSerial)
    .value("Serial1", Registers::System::AsyncOutputType::SerialPort::Serial1)
    .value("Serial2", Registers::System::AsyncOutputType::SerialPort::Serial2)
    .value("Poll", Registers::System::AsyncOutputType::SerialPort::Poll)
    ;
    // Enumeration for Register 7 AsyncOutputFreq Adof
    py::enum_<Registers::System::AsyncOutputFreq::Adof>(asyncOutputFreq, "Adof")
    .value("Rate0Hz", Registers::System::AsyncOutputFreq::Adof::Rate0Hz)
    .value("Rate1Hz", Registers::System::AsyncOutputFreq::Adof::Rate1Hz)
    .value("Rate2Hz", Registers::System::AsyncOutputFreq::Adof::Rate2Hz)
    .value("Rate4Hz", Registers::System::AsyncOutputFreq::Adof::Rate4Hz)
    .value("Rate5Hz", Registers::System::AsyncOutputFreq::Adof::Rate5Hz)
    .value("Rate10Hz", Registers::System::AsyncOutputFreq::Adof::Rate10Hz)
    .value("Rate20Hz", Registers::System::AsyncOutputFreq::Adof::Rate20Hz)
    .value("Rate25Hz", Registers::System::AsyncOutputFreq::Adof::Rate25Hz)
    .value("Rate40Hz", Registers::System::AsyncOutputFreq::Adof::Rate40Hz)
    .value("Rate50Hz", Registers::System::AsyncOutputFreq::Adof::Rate50Hz)
    .value("Rate100Hz", Registers::System::AsyncOutputFreq::Adof::Rate100Hz)
    .value("Rate200Hz", Registers::System::AsyncOutputFreq::Adof::Rate200Hz)
    ;
    // Enumeration for Register 7 AsyncOutputFreq SerialPort
    py::enum_<Registers::System::AsyncOutputFreq::SerialPort>(asyncOutputFreq, "SerialPort")
    .value("ActiveSerial", Registers::System::AsyncOutputFreq::SerialPort::ActiveSerial)
    .value("Serial1", Registers::System::AsyncOutputFreq::SerialPort::Serial1)
    .value("Serial2", Registers::System::AsyncOutputFreq::SerialPort::Serial2)
    .value("Poll", Registers::System::AsyncOutputFreq::SerialPort::Poll)
    ;
    // Enumeration for Register 30 ProtocolControl AsciiAppendCount
    py::enum_<Registers::System::ProtocolControl::AsciiAppendCount>(protocolControl, "AsciiAppendCount")
    .value("none", Registers::System::ProtocolControl::AsciiAppendCount::None)
    .value("SyncInCount", Registers::System::ProtocolControl::AsciiAppendCount::SyncInCount)
    .value("SyncInTime", Registers::System::ProtocolControl::AsciiAppendCount::SyncInTime)
    .value("SyncOutCount", Registers::System::ProtocolControl::AsciiAppendCount::SyncOutCount)
    .value("GpsPps", Registers::System::ProtocolControl::AsciiAppendCount::GpsPps)
    .value("GpsTow", Registers::System::ProtocolControl::AsciiAppendCount::GpsTow)
    ;
    // Enumeration for Register 30 ProtocolControl AsciiAppendStatus
    py::enum_<Registers::System::ProtocolControl::AsciiAppendStatus>(protocolControl, "AsciiAppendStatus")
    .value("none", Registers::System::ProtocolControl::AsciiAppendStatus::None)
    .value("Ahrs", Registers::System::ProtocolControl::AsciiAppendStatus::Ahrs)
    .value("Ins", Registers::System::ProtocolControl::AsciiAppendStatus::Ins)
    .value("Imu", Registers::System::ProtocolControl::AsciiAppendStatus::Imu)
    .value("Gnss1", Registers::System::ProtocolControl::AsciiAppendStatus::Gnss1)
    .value("Gnss2", Registers::System::ProtocolControl::AsciiAppendStatus::Gnss2)
    .value("Gnss3", Registers::System::ProtocolControl::AsciiAppendStatus::Gnss3)
    ;
    // Enumeration for Register 30 ProtocolControl SpiAppendCount
    py::enum_<Registers::System::ProtocolControl::SpiAppendCount>(protocolControl, "SpiAppendCount")
    .value("none", Registers::System::ProtocolControl::SpiAppendCount::None)
    .value("SyncInCount", Registers::System::ProtocolControl::SpiAppendCount::SyncInCount)
    .value("SyncInTime", Registers::System::ProtocolControl::SpiAppendCount::SyncInTime)
    .value("SyncOutCount", Registers::System::ProtocolControl::SpiAppendCount::SyncOutCount)
    .value("GpsPps", Registers::System::ProtocolControl::SpiAppendCount::GpsPps)
    .value("GpsTow", Registers::System::ProtocolControl::SpiAppendCount::GpsTow)
    ;
    // Enumeration for Register 30 ProtocolControl SpiAppendStatus
    py::enum_<Registers::System::ProtocolControl::SpiAppendStatus>(protocolControl, "SpiAppendStatus")
    .value("none", Registers::System::ProtocolControl::SpiAppendStatus::None)
    .value("Ahrs", Registers::System::ProtocolControl::SpiAppendStatus::Ahrs)
    .value("Ins", Registers::System::ProtocolControl::SpiAppendStatus::Ins)
    .value("Imu", Registers::System::ProtocolControl::SpiAppendStatus::Imu)
    .value("Gnss1", Registers::System::ProtocolControl::SpiAppendStatus::Gnss1)
    .value("Gnss2", Registers::System::ProtocolControl::SpiAppendStatus::Gnss2)
    .value("Gnss3", Registers::System::ProtocolControl::SpiAppendStatus::Gnss3)
    ;
    // Enumeration for Register 30 ProtocolControl AsciiChecksum
    py::enum_<Registers::System::ProtocolControl::AsciiChecksum>(protocolControl, "AsciiChecksum")
    .value("Checksum8bit", Registers::System::ProtocolControl::AsciiChecksum::Checksum8bit)
    .value("Crc16bit", Registers::System::ProtocolControl::AsciiChecksum::Crc16bit)
    ;
    // Enumeration for Register 30 ProtocolControl SpiChecksum
    py::enum_<Registers::System::ProtocolControl::SpiChecksum>(protocolControl, "SpiChecksum")
    .value("Off", Registers::System::ProtocolControl::SpiChecksum::Off)
    .value("Checksum8bit", Registers::System::ProtocolControl::SpiChecksum::Checksum8bit)
    .value("Crc16bit", Registers::System::ProtocolControl::SpiChecksum::Crc16bit)
    ;
    // Enumeration for Register 30 ProtocolControl ErrorMode
    py::enum_<Registers::System::ProtocolControl::ErrorMode>(protocolControl, "ErrorMode")
    .value("Ignore", Registers::System::ProtocolControl::ErrorMode::Ignore)
    .value("SendError", Registers::System::ProtocolControl::ErrorMode::SendError)
    .value("AdorOff", Registers::System::ProtocolControl::ErrorMode::AdorOff)
    ;
    // Enumeration for Register 32 SyncControl SyncInMode
    py::enum_<Registers::System::SyncControl::SyncInMode>(syncControl, "SyncInMode")
    .value("Disable", Registers::System::SyncControl::SyncInMode::Disable)
    .value("Count", Registers::System::SyncControl::SyncInMode::Count)
    .value("ImuSample", Registers::System::SyncControl::SyncInMode::ImuSample)
    .value("AsyncAll", Registers::System::SyncControl::SyncInMode::AsyncAll)
    .value("Async0", Registers::System::SyncControl::SyncInMode::Async0)
    ;
    // Enumeration for Register 32 SyncControl SyncInEdge
    py::enum_<Registers::System::SyncControl::SyncInEdge>(syncControl, "SyncInEdge")
    .value("RisingEdge", Registers::System::SyncControl::SyncInEdge::RisingEdge)
    .value("FallingEdge", Registers::System::SyncControl::SyncInEdge::FallingEdge)
    ;
    // Enumeration for Register 32 SyncControl SyncOutMode
    py::enum_<Registers::System::SyncControl::SyncOutMode>(syncControl, "SyncOutMode")
    .value("none", Registers::System::SyncControl::SyncOutMode::None)
    .value("ImuStart", Registers::System::SyncControl::SyncOutMode::ImuStart)
    .value("ImuReady", Registers::System::SyncControl::SyncOutMode::ImuReady)
    .value("NavReady", Registers::System::SyncControl::SyncOutMode::NavReady)
    .value("GpsPps", Registers::System::SyncControl::SyncOutMode::GpsPps)
    ;
    // Enumeration for Register 32 SyncControl SyncOutPolarity
    py::enum_<Registers::System::SyncControl::SyncOutPolarity>(syncControl, "SyncOutPolarity")
    .value("NegativePulse", Registers::System::SyncControl::SyncOutPolarity::NegativePulse)
    .value("PositivePulse", Registers::System::SyncControl::SyncOutPolarity::PositivePulse)
    ;
    // Enumeration for Register 101 NmeaOutput1 Port
    py::enum_<Registers::System::NmeaOutput1::Port>(nmeaOutput1, "Port")
    .value("none", Registers::System::NmeaOutput1::Port::None)
    .value("Serial1", Registers::System::NmeaOutput1::Port::Serial1)
    .value("Serial2", Registers::System::NmeaOutput1::Port::Serial2)
    .value("Serial1And2", Registers::System::NmeaOutput1::Port::Serial1And2)
    ;
    // Enumeration for Register 101 NmeaOutput1 Rate
    py::enum_<Registers::System::NmeaOutput1::Rate>(nmeaOutput1, "Rate")
    .value("Rate0Hz", Registers::System::NmeaOutput1::Rate::Rate0Hz)
    .value("Rate1Hz", Registers::System::NmeaOutput1::Rate::Rate1Hz)
    .value("Rate5Hz", Registers::System::NmeaOutput1::Rate::Rate5Hz)
    .value("Rate10Hz", Registers::System::NmeaOutput1::Rate::Rate10Hz)
    .value("Rate20Hz", Registers::System::NmeaOutput1::Rate::Rate20Hz)
    ;
    // Enumeration for Register 101 NmeaOutput1 Mode
    py::enum_<Registers::System::NmeaOutput1::Mode>(nmeaOutput1, "Mode")
    .value("V41GPID", Registers::System::NmeaOutput1::Mode::V41GPID)
    .value("V23GPID", Registers::System::NmeaOutput1::Mode::V23GPID)
    .value("V41SYSID", Registers::System::NmeaOutput1::Mode::V41SYSID)
    ;
    // Enumeration for Register 101 NmeaOutput1 GnssSelect
    py::enum_<Registers::System::NmeaOutput1::GnssSelect>(nmeaOutput1, "GnssSelect")
    .value("GnssA", Registers::System::NmeaOutput1::GnssSelect::GnssA)
    .value("ActiveReceiver", Registers::System::NmeaOutput1::GnssSelect::ActiveReceiver)
    ;
    // Enumeration for Register 102 NmeaOutput2 Port
    py::enum_<Registers::System::NmeaOutput2::Port>(nmeaOutput2, "Port")
    .value("none", Registers::System::NmeaOutput2::Port::None)
    .value("Serial1", Registers::System::NmeaOutput2::Port::Serial1)
    .value("Serial2", Registers::System::NmeaOutput2::Port::Serial2)
    .value("Serial1And2", Registers::System::NmeaOutput2::Port::Serial1And2)
    ;
    // Enumeration for Register 102 NmeaOutput2 Rate
    py::enum_<Registers::System::NmeaOutput2::Rate>(nmeaOutput2, "Rate")
    .value("Rate0Hz", Registers::System::NmeaOutput2::Rate::Rate0Hz)
    .value("Rate1Hz", Registers::System::NmeaOutput2::Rate::Rate1Hz)
    .value("Rate5Hz", Registers::System::NmeaOutput2::Rate::Rate5Hz)
    .value("Rate10Hz", Registers::System::NmeaOutput2::Rate::Rate10Hz)
    .value("Rate20Hz", Registers::System::NmeaOutput2::Rate::Rate20Hz)
    ;
    // Enumeration for Register 102 NmeaOutput2 Mode
    py::enum_<Registers::System::NmeaOutput2::Mode>(nmeaOutput2, "Mode")
    .value("V41GPID", Registers::System::NmeaOutput2::Mode::V41GPID)
    .value("V23GPID", Registers::System::NmeaOutput2::Mode::V23GPID)
    .value("V41SYSID", Registers::System::NmeaOutput2::Mode::V41SYSID)
    ;
    // Enumeration for Register 102 NmeaOutput2 GnssSelect
    py::enum_<Registers::System::NmeaOutput2::GnssSelect>(nmeaOutput2, "GnssSelect")
    .value("GnssA", Registers::System::NmeaOutput2::GnssSelect::GnssA)
    .value("ActiveReceiver", Registers::System::NmeaOutput2::GnssSelect::ActiveReceiver)
    ;
    // Enumeration for Register 206 LegacyCompatibilitySettings InsLegacy
    py::enum_<Registers::System::LegacyCompatibilitySettings::InsLegacy>(legacyCompatibilitySettings, "InsLegacy")
    .value("Current", Registers::System::LegacyCompatibilitySettings::InsLegacy::Current)
    .value("Legacy", Registers::System::LegacyCompatibilitySettings::InsLegacy::Legacy)
    ;
    // Enumeration for Register 206 LegacyCompatibilitySettings ImuLegacy
    py::enum_<Registers::System::LegacyCompatibilitySettings::ImuLegacy>(legacyCompatibilitySettings, "ImuLegacy")
    .value("Current", Registers::System::LegacyCompatibilitySettings::ImuLegacy::Current)
    .value("Legacy", Registers::System::LegacyCompatibilitySettings::ImuLegacy::Legacy)
    ;
    // Enumeration for Register 206 LegacyCompatibilitySettings HwLegacy
    py::enum_<Registers::System::LegacyCompatibilitySettings::HwLegacy>(legacyCompatibilitySettings, "HwLegacy")
    .value("Current", Registers::System::LegacyCompatibilitySettings::HwLegacy::Current)
    .value("Legacy", Registers::System::LegacyCompatibilitySettings::HwLegacy::Legacy)
    ;
    // Enumeration for Register 51 VelAidingControl VelAidEnable
    py::enum_<Registers::VelocityAiding::VelAidingControl::VelAidEnable>(velAidingControl, "VelAidEnable")
    .value("Disable", Registers::VelocityAiding::VelAidingControl::VelAidEnable::Disable)
    .value("Enable", Registers::VelocityAiding::VelAidingControl::VelAidEnable::Enable)
    ;
    // Enumeration for Register 83 RefModelConfig EnableMagModel
    py::enum_<Registers::WorldMagGravityModel::RefModelConfig::EnableMagModel>(refModelConfig, "EnableMagModel")
    .value("Disabled", Registers::WorldMagGravityModel::RefModelConfig::EnableMagModel::Disabled)
    .value("Enabled", Registers::WorldMagGravityModel::RefModelConfig::EnableMagModel::Enabled)
    ;
    // Enumeration for Register 83 RefModelConfig EnableGravityModel
    py::enum_<Registers::WorldMagGravityModel::RefModelConfig::EnableGravityModel>(refModelConfig, "EnableGravityModel")
    .value("Disabled", Registers::WorldMagGravityModel::RefModelConfig::EnableGravityModel::Disabled)
    .value("Enabled", Registers::WorldMagGravityModel::RefModelConfig::EnableGravityModel::Enabled)
    ;
}
}
// clang-format on

