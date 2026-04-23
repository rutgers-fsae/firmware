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

#ifndef VN_PUBLISHING_HPP_
#define VN_PUBLISHING_HPP_

void Vectornav::publishMessages()
{
    const auto cd = vs_.getNextMeasurement();
    if (!cd) { return; }
    if (std::holds_alternative<VN::AsciiHeader>(cd->header())) { return; }
    const rclcpp::Time rosTime = now();
    const VN::BinaryHeader header = std::get<VN::BinaryHeader>(cd->header());

#if (COMMON_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(COMMON_BIT)) { publishCommonGroup(cd.get(), rosTime); }
#endif  // COMMON_GROUP_ENABLE

#if (TIME_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(TIME_BIT)) { publishTimeGroup(cd.get(), rosTime); }
#endif  // TIME_GROUP_ENABLE

#if (IMU_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(IMU_BIT)) { publishImuGroup(cd.get(), rosTime); }
#endif  // IMU_GROUP_ENABLE

#if (GNSS_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(GNSS_BIT)) { publishGnssGroup(cd.get(), rosTime); }
#endif  // GNSS_GROUP_ENABLE

#if (ATTITUDE_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(ATTITUDE_BIT)) { publishAttitudeGroup(cd.get(), rosTime); }
#endif  // ATTITUDE_GROUP_ENABLE

#if (INS_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(INS_BIT)) { publishInsGroup(cd.get(), rosTime); }
#endif  // INS_GROUP_ENABLE

#if (GNSS2_GROUP_ENABLE)
    if (header.isOutputGroupEnabled(GNSS2_BIT)) { publishGnss2Group(cd.get(), rosTime); }
#endif  // GNSS2_GROUP_ENABLE

}  // Vectornav::publishMessages()
void Vectornav::publishCommonGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto common_msg = vectornav_msgs::msg::CommonGroup();
    common_msg.header.stamp = rosTime;
    common_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (TIME_GROUP_ENABLE & TIME_TIMESTARTUP_BIT)
    // COMMON GROUP : TIMESTARTUP
    if (cd->time.timeStartup.has_value()) { common_msg.timestartup = cd->time.timeStartup->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPS_BIT)
    // COMMON GROUP : TIMEGPS
    if (cd->time.timeGps.has_value()) { common_msg.timegps = cd->time.timeGps->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESYNCIN_BIT)
    // COMMON GROUP : TIMESYNCIN
    if (cd->time.timeSyncIn.has_value()) { common_msg.timesyncin = cd->time.timeSyncIn->nanoseconds(); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPR_BIT)
    // COMMON GROUP : YPR
    if (cd->attitude.ypr.has_value()) { common_msg.ypr = toVector3(cd->attitude.ypr.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_QUATERNION_BIT)
    // COMMON GROUP : QUATERNION
    if (cd->attitude.quaternion.has_value()) { common_msg.quaternion = toQuaternion(cd->attitude.quaternion.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_ANGULARRATE_BIT)
    // COMMON GROUP : ANGULARRATE
    if (cd->imu.angularRate.has_value()) { common_msg.angularrate = toVector3(cd->imu.angularRate.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_POSLLA_BIT)
    // COMMON GROUP : POSLLA
    if (cd->ins.posLla.has_value()) { common_msg.poslla = toPoint3d(cd->ins.posLla.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_VELNED_BIT)
    // COMMON GROUP : VELNED
    if (cd->ins.velNed.has_value()) { common_msg.velned = toVector3(cd->ins.velNed.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_ACCEL_BIT)
    // COMMON GROUP : ACCEL
    if (cd->imu.accel.has_value()) { common_msg.accel = toVector3(cd->imu.accel.value()); }
#endif

#if ((IMU_GROUP_ENABLE & IMU_UNCOMPGYRO_BIT) && (IMU_GROUP_ENABLE & IMU_UNCOMPACCEL_BIT))
    // COMMON GROUP : IMU
    if (cd->imu.uncompGyro.has_value() && cd->imu.uncompAccel.has_value())
    {
        common_msg.imu.uncompaccel = toVector3(cd->imu.uncompAccel.value());
        common_msg.imu.uncompgyro = toVector3(cd->imu.uncompGyro.value());
    }
#endif

#if ((IMU_GROUP_ENABLE & IMU_MAG_BIT) && (IMU_GROUP_ENABLE & IMU_TEMPERATURE_BIT) && (IMU_GROUP_ENABLE & IMU_PRESSURE_BIT))
    // COMMON GROUP : MAGPRES
    if (cd->imu.mag.has_value())
    {
        common_msg.magpres.mag = toVector3(cd->imu.mag.value());
        common_msg.magpres.temperature = cd->imu.temperature.value();
        common_msg.magpres.pressure = cd->imu.pressure.value();
    }
#endif

#if ((IMU_GROUP_ENABLE & IMU_DELTATHETA_BIT) && (IMU_GROUP_ENABLE & IMU_DELTAVEL_BIT))
    // COMMON GROUP : DELTAS
    if (cd->imu.deltaTheta.has_value() && cd->imu.deltaVel.has_value())
    {
        common_msg.deltas.deltatime = cd->imu.deltaTheta->deltaTime;
        common_msg.deltas.deltatheta = toVector3(cd->imu.deltaTheta->deltaTheta);
        common_msg.deltas.deltavel = toVector3(cd->imu.deltaVel.value());
    }
#endif

#if (INS_GROUP_ENABLE & INS_INSSTATUS_BIT)
    // COMMON GROUP : INSSTATUS
    if (cd->ins.insStatus.has_value())
    {
        common_msg.insstatus.bitfield = uint16_t(cd->ins.insStatus.value());
        common_msg.insstatus.mode = cd->ins.insStatus->mode;
        common_msg.insstatus.gnssfix = cd->ins.insStatus->gnssFix;
        common_msg.insstatus.resv1 = cd->ins.insStatus->resv1;
        common_msg.insstatus.imuerr = cd->ins.insStatus->imuErr;
        common_msg.insstatus.magpreserr = cd->ins.insStatus->magPresErr;
        common_msg.insstatus.gnsserr = cd->ins.insStatus->gnssErr;
        common_msg.insstatus.resv2 = cd->ins.insStatus->resv2;
        common_msg.insstatus.gnssheadingins = cd->ins.insStatus->gnssCompassFix & 0x01;
        common_msg.insstatus.gnsscompass = cd->ins.insStatus->gnssCompassFix & 0x02;
    }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCINCNT_BIT)
    // COMMON GROUP : SYNCINCNT
    if (cd->time.syncInCnt.has_value()) { common_msg.syncincnt = cd->time.syncInCnt.value(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSPPS_BIT)
    // COMMON GROUP : TIMEGPSPPS
    if (cd->time.timeGpsPps.has_value()) { common_msg.timegpspps = cd->time.timeGpsPps->nanoseconds(); }
#endif

    this->pubCommon->publish(common_msg);

}  // publishCommonGroup

void Vectornav::publishTimeGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto time_msg = vectornav_msgs::msg::TimeGroup();
    time_msg.header.stamp = rosTime;
    time_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (TIME_GROUP_ENABLE & TIME_TIMESTARTUP_BIT)
    // TIME GROUP : TIMESTARTUP
    if (cd->time.timeStartup.has_value()) { time_msg.timestartup = cd->time.timeStartup->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPS_BIT)
    // TIME GROUP : TIMEGPS
    if (cd->time.timeGps.has_value()) { time_msg.timegps = cd->time.timeGps->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSTOW_BIT)
    // TIME GROUP : TIMEGPSTOW
    if (cd->time.timeGpsTow.has_value()) { time_msg.timegpstow = cd->time.timeGpsTow->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSWEEK_BIT)
    // TIME GROUP : TIMEGPSWEEK
    if (cd->time.timeGpsWeek.has_value()) { time_msg.timegpsweek = cd->time.timeGpsWeek.value(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESYNCIN_BIT)
    // TIME GROUP : TIMESYNCIN
    if (cd->time.timeSyncIn.has_value()) { time_msg.timesyncin = cd->time.timeSyncIn->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSPPS_BIT)
    // TIME GROUP : TIMEGPSPPS
    if (cd->time.timeGpsPps.has_value()) { time_msg.timegpspps = cd->time.timeGpsPps->nanoseconds(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEUTC_BIT)
    // TIME GROUP : TIMEUTC
    if (cd->time.timeUtc.has_value())
    {
        time_msg.timeutc.year = cd->time.timeUtc->year;
        time_msg.timeutc.month = cd->time.timeUtc->month;
        time_msg.timeutc.day = cd->time.timeUtc->day;
        time_msg.timeutc.hour = cd->time.timeUtc->hour;
        time_msg.timeutc.minute = cd->time.timeUtc->minute;
        time_msg.timeutc.second = cd->time.timeUtc->second;
        time_msg.timeutc.fracsec = cd->time.timeUtc->fracSec;
    }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCINCNT_BIT)
    // TIME GROUP : SYNCINCNT
    if (cd->time.syncInCnt.has_value()) { time_msg.syncincnt = cd->time.syncInCnt.value(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCOUTCNT_BIT)
    // TIME GROUP : SYNCOUTCNT
    if (cd->time.syncOutCnt.has_value()) { time_msg.syncoutcnt = cd->time.syncOutCnt.value(); }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESTATUS_BIT)
    // TIME GROUP : TIMESTATUS
    if (cd->time.timeStatus.has_value())
    {
        time_msg.timestatus.bitfield = uint8_t(cd->time.timeStatus.value());
        time_msg.timestatus.towvalid = (time_msg.timestatus.bitfield & (0x01 << 0));
        time_msg.timestatus.datevalid = (time_msg.timestatus.bitfield & (0x01 << 1));
        time_msg.timestatus.utcvalid = (time_msg.timestatus.bitfield & (0x01 << 2));
    }
#endif

    this->pubTime->publish(time_msg);

}  // publishTimeGroup

void Vectornav::publishImuGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto imu_msg = vectornav_msgs::msg::ImuGroup();
    imu_msg.header.stamp = rosTime;
    imu_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (IMU_GROUP_ENABLE & IMU_IMUSTATUS_BIT)
    // IMU GROUP : IMUSTATUS
    if (cd->imu.imuStatus.has_value())
    {
        imu_msg.imustatus.bitfield = uint16_t(cd->imu.imuStatus.value());
        imu_msg.imustatus.gyrostatus = (imu_msg.imustatus.bitfield & (0x03 << 0)) >> 0;
        imu_msg.imustatus.accelstatus = (imu_msg.imustatus.bitfield & (0x03 << 2)) >> 2;
        imu_msg.imustatus.magstatus = (imu_msg.imustatus.bitfield & (0x03 << 4)) >> 4;
        imu_msg.imustatus.prestempstatus = (imu_msg.imustatus.bitfield & (0x03 << 6)) >> 6;
        imu_msg.imustatus.resv = (imu_msg.imustatus.bitfield & (0x03 << 8)) >> 8;
    }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPMAG_BIT)
    // IMU GROUP : UNCOMPMAG
    if (cd->imu.uncompMag.has_value()) { imu_msg.uncompmag = toVector3(cd->imu.uncompMag.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPACCEL_BIT)
    // IMU GROUP : UNCOMPACCEL
    if (cd->imu.uncompAccel.has_value()) { imu_msg.uncompaccel = toVector3(cd->imu.uncompAccel.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPGYRO_BIT)
    // IMU GROUP : UNCOMPGYRO
    if (cd->imu.uncompGyro.has_value()) { imu_msg.uncompgyro = toVector3(cd->imu.uncompGyro.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_TEMPERATURE_BIT)
    // IMU GROUP : TEMPERATURE
    if (cd->imu.temperature.has_value()) { imu_msg.temperature = cd->imu.temperature.value(); }
#endif

#if (IMU_GROUP_ENABLE & IMU_PRESSURE_BIT)
    // IMU GROUP : PRESSURE
    if (cd->imu.pressure.has_value()) { imu_msg.pressure = cd->imu.pressure.value(); }
#endif

#if (IMU_GROUP_ENABLE & IMU_DELTATHETA_BIT)
    // IMU GROUP : DELTATHETA
    if (cd->imu.deltaTheta.has_value())
    {
        imu_msg.deltatheta.deltatheta = toVector3(cd->imu.deltaTheta->deltaTheta);
        imu_msg.deltatheta.deltatime = cd->imu.deltaTheta->deltaTime;
    }
#endif

#if (IMU_GROUP_ENABLE & IMU_DELTAVEL_BIT)
    // IMU GROUP : DELTAVEL
    if (cd->imu.deltaVel.has_value()) { imu_msg.deltavel = toVector3(cd->imu.deltaVel.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_MAG_BIT)
    // IMU GROUP : MAG
    if (cd->imu.mag.has_value()) { imu_msg.mag = toVector3(cd->imu.mag.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_ACCEL_BIT)
    // IMU GROUP : ACCEL
    if (cd->imu.accel.has_value()) { imu_msg.accel = toVector3(cd->imu.accel.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_ANGULARRATE_BIT)
    // IMU GROUP : ANGULARRATE
    if (cd->imu.angularRate.has_value()) { imu_msg.angularrate = toVector3(cd->imu.angularRate.value()); }
#endif

#if (IMU_GROUP_ENABLE & IMU_SENSSAT_BIT)
    // IMU GROUP : SENSSAT
    if (cd->imu.sensSat.has_value())
    {
        imu_msg.senssat.bitfield = cd->imu.sensSat.value();
        imu_msg.senssat.magx = (imu_msg.senssat.bitfield & (0x01 << 0));
        imu_msg.senssat.magy = (imu_msg.senssat.bitfield & (0x01 << 1));
        imu_msg.senssat.magz = (imu_msg.senssat.bitfield & (0x01 << 2));
        imu_msg.senssat.accelx = (imu_msg.senssat.bitfield & (0x01 << 3));
        imu_msg.senssat.accely = (imu_msg.senssat.bitfield & (0x01 << 4));
        imu_msg.senssat.accelz = (imu_msg.senssat.bitfield & (0x01 << 5));
        imu_msg.senssat.gyrox = (imu_msg.senssat.bitfield & (0x01 << 6));
        imu_msg.senssat.gyroy = (imu_msg.senssat.bitfield & (0x01 << 7));
        imu_msg.senssat.gyroz = (imu_msg.senssat.bitfield & (0x01 << 8));
        imu_msg.senssat.pres = (imu_msg.senssat.bitfield & (0x01 << 9));
    }
#endif

    this->pubImu->publish(imu_msg);

}  // publishImuGroup

void Vectornav::publishGnssGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto gnss_msg = vectornav_msgs::msg::GnssGroup();
    gnss_msg.header.stamp = rosTime;
    gnss_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUTC_BIT)
    // GNSS GROUP : GNSS1TIMEUTC
    if (cd->gnss.gnss1TimeUtc.has_value())
    {
        gnss_msg.gnss1timeutc.year = cd->gnss.gnss1TimeUtc->year;
        gnss_msg.gnss1timeutc.month = cd->gnss.gnss1TimeUtc->month;
        gnss_msg.gnss1timeutc.day = cd->gnss.gnss1TimeUtc->day;
        gnss_msg.gnss1timeutc.hour = cd->gnss.gnss1TimeUtc->hour;
        gnss_msg.gnss1timeutc.minute = cd->gnss.gnss1TimeUtc->minute;
        gnss_msg.gnss1timeutc.second = cd->gnss.gnss1TimeUtc->second;
        gnss_msg.gnss1timeutc.fracsec = cd->gnss.gnss1TimeUtc->fracSec;
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GPS1TOW_BIT)
    // GNSS GROUP : GPS1TOW
    if (cd->gnss.gps1Tow.has_value()) { gnss_msg.gps1tow = cd->gnss.gps1Tow->nanoseconds(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GPS1WEEK_BIT)
    // GNSS GROUP : GPS1WEEK
    if (cd->gnss.gps1Week.has_value()) { gnss_msg.gps1week = cd->gnss.gps1Week.value(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1NUMSATS_BIT)
    // GNSS GROUP : GNSS1NUMSATS
    if (cd->gnss.gnss1NumSats.has_value()) { gnss_msg.gnss1numsats = cd->gnss.gnss1NumSats.value(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1FIX_BIT)
    // GNSS GROUP : GNSS1FIX
    if (cd->gnss.gnss1Fix.has_value()) { gnss_msg.gnss1fix = cd->gnss.gnss1Fix.value(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSLLA_BIT)
    // GNSS GROUP : GNSS1POSLLA
    if (cd->gnss.gnss1PosLla.has_value()) { gnss_msg.gnss1poslla = toPoint3d(cd->gnss.gnss1PosLla.value()); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSECEF_BIT)
    // GNSS GROUP : GNSS1POSECEF
    if (cd->gnss.gnss1PosEcef.has_value()) { gnss_msg.gnss1posecef = toPoint3d(cd->gnss.gnss1PosEcef.value()); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELNED_BIT)
    // GNSS GROUP : GNSS1VELNED
    if (cd->gnss.gnss1VelNed.has_value()) { gnss_msg.gnss1velned = toVector3(cd->gnss.gnss1VelNed.value()); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELECEF_BIT)
    // GNSS GROUP : GNSS1VELECEF
    if (cd->gnss.gnss1VelEcef.has_value()) { gnss_msg.gnss1velecef = toVector3(cd->gnss.gnss1VelEcef.value()); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSUNCERTAINTY_BIT)
    // GNSS GROUP : GNSS1POSUNCERTAINTY
    if (cd->gnss.gnss1PosUncertainty.has_value()) { gnss_msg.gnss1posuncertainty = toVector3(cd->gnss.gnss1PosUncertainty.value()); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELUNCERTAINTY_BIT)
    // GNSS GROUP : GNSS1VELUNCERTAINTY
    if (cd->gnss.gnss1VelUncertainty.has_value()) { gnss_msg.gnss1veluncertainty = cd->gnss.gnss1VelUncertainty.value(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUNCERTAINTY_BIT)
    // GNSS GROUP : GNSS1TIMEUNCERTAINTY
    if (cd->gnss.gnss1TimeUncertainty.has_value()) { gnss_msg.gnss1timeuncertainty = cd->gnss.gnss1TimeUncertainty.value(); }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEINFO_BIT)
    // GNSS GROUP : GNSS1TIMEINFO
    if (cd->gnss.gnss1TimeInfo.has_value())
    {
        gnss_msg.gnss1timeinfo.gnsstimestatus = cd->gnss.gnss1TimeInfo->gnssTimeStatus;
        gnss_msg.gnss1timeinfo.leapseconds = cd->gnss.gnss1TimeInfo->leapSeconds;
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1DOP_BIT)
    // GNSS GROUP : GNSS1DOP
    if (cd->gnss.gnss1Dop.has_value())
    {
        gnss_msg.gnss1dop.gdop = cd->gnss.gnss1Dop->gDop;
        gnss_msg.gnss1dop.pdop = cd->gnss.gnss1Dop->pDop;
        gnss_msg.gnss1dop.tdop = cd->gnss.gnss1Dop->tDop;
        gnss_msg.gnss1dop.vdop = cd->gnss.gnss1Dop->vDop;
        gnss_msg.gnss1dop.hdop = cd->gnss.gnss1Dop->hDop;
        gnss_msg.gnss1dop.ndop = cd->gnss.gnss1Dop->nDop;
        gnss_msg.gnss1dop.edop = cd->gnss.gnss1Dop->eDop;
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1SATINFO_BIT)
    // GNSS GROUP : GNSS1SATINFO
    if (cd->gnss.gnss1SatInfo.has_value())
    {
        gnss_msg.gnss1satinfo.numsats = cd->gnss.gnss1SatInfo->numSats;
        gnss_msg.gnss1satinfo.resv = cd->gnss.gnss1SatInfo->resv;
        gnss_msg.gnss1satinfo.sys = cd->gnss.gnss1SatInfo->sys;
        gnss_msg.gnss1satinfo.svid = cd->gnss.gnss1SatInfo->svId;
        gnss_msg.gnss1satinfo.flags = cd->gnss.gnss1SatInfo->flags;
        gnss_msg.gnss1satinfo.cno = cd->gnss.gnss1SatInfo->cno;
        gnss_msg.gnss1satinfo.qi = cd->gnss.gnss1SatInfo->qi;
        gnss_msg.gnss1satinfo.el = cd->gnss.gnss1SatInfo->el;
        gnss_msg.gnss1satinfo.az = cd->gnss.gnss1SatInfo->az;
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1RAWMEAS_BIT)
    // GNSS GROUP : GNSS1RAWMEAS
    if (cd->gnss.gnss1RawMeas.has_value())
    {
        gnss_msg.gnss1rawmeas.tow = cd->gnss.gnss1RawMeas->tow;
        gnss_msg.gnss1rawmeas.week = cd->gnss.gnss1RawMeas->week;
        gnss_msg.gnss1rawmeas.nummeas = cd->gnss.gnss1RawMeas->numMeas;
        gnss_msg.gnss1rawmeas.resv = cd->gnss.gnss1RawMeas->resv;
        gnss_msg.gnss1rawmeas.sys = cd->gnss.gnss1RawMeas->sys;
        gnss_msg.gnss1rawmeas.svid = cd->gnss.gnss1RawMeas->svId;
        gnss_msg.gnss1rawmeas.band = cd->gnss.gnss1RawMeas->band;
        gnss_msg.gnss1rawmeas.chan = cd->gnss.gnss1RawMeas->chan;
        gnss_msg.gnss1rawmeas.freqnum = cd->gnss.gnss1RawMeas->freqNum;
        gnss_msg.gnss1rawmeas.cno = cd->gnss.gnss1RawMeas->cno;
        gnss_msg.gnss1rawmeas.flags = cd->gnss.gnss1RawMeas->flags;
        gnss_msg.gnss1rawmeas.pr = cd->gnss.gnss1RawMeas->pr;
        gnss_msg.gnss1rawmeas.cp = cd->gnss.gnss1RawMeas->cp;
        gnss_msg.gnss1rawmeas.dp = cd->gnss.gnss1RawMeas->dp;
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1STATUS_BIT)
    // GNSS GROUP : GNSS1STATUS
    if (cd->gnss.gnss1Status.has_value())
    {
        gnss_msg.gnss1status.bitfield = uint16_t(cd->gnss.gnss1Status.value());
        gnss_msg.gnss1status.enabled = (gnss_msg.gnss1status.bitfield & (0x01 << 0));
        gnss_msg.gnss1status.operational = (gnss_msg.gnss1status.bitfield & (0x01 << 1));
        gnss_msg.gnss1status.fix = (gnss_msg.gnss1status.bitfield & (0x01 << 2));
        gnss_msg.gnss1status.antennasignalerror = (gnss_msg.gnss1status.bitfield & (0x01 << 3));
        gnss_msg.gnss1status.usedfornav = (gnss_msg.gnss1status.bitfield & (0x01 << 4));
        gnss_msg.gnss1status.usedforcompass = (gnss_msg.gnss1status.bitfield & (0x01 << 5));
        gnss_msg.gnss1status.spoofingdetection = (gnss_msg.gnss1status.bitfield & (0x03 << 6)) >> 6;
        gnss_msg.gnss1status.datasource = (gnss_msg.gnss1status.bitfield & (0x07 << 8)) >> 8;
        gnss_msg.gnss1status.usedfornavcurr = (gnss_msg.gnss1status.bitfield & (0x01 << 11));
        gnss_msg.gnss1status.ppsusedfortime = (gnss_msg.gnss1status.bitfield & (0x01 << 12));
    }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1ALTMSL_BIT)
    // GNSS GROUP : GNSS1ALTMSL
    if (cd->gnss.gnss1AltMsl.has_value()) { gnss_msg.gnss1altmsl = cd->gnss.gnss1AltMsl.value(); }
#endif

    this->pubGnss->publish(gnss_msg);

}  // publishGnssGroup

void Vectornav::publishAttitudeGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto attitude_msg = vectornav_msgs::msg::AttitudeGroup();
    attitude_msg.header.stamp = rosTime;
    attitude_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPR_BIT)
    // ATTITUDE GROUP : YPR
    if (cd->attitude.ypr.has_value()) { attitude_msg.ypr = toVector3(cd->attitude.ypr.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_QUATERNION_BIT)
    // ATTITUDE GROUP : QUATERNION
    if (cd->attitude.quaternion.has_value()) { attitude_msg.quaternion = toQuaternion(cd->attitude.quaternion.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_DCM_BIT)
    // ATTITUDE GROUP : DCM
    if (cd->attitude.dcm.has_value())
    {
        attitude_msg.dcm.dcm00 = cd->attitude.dcm.value()(0, 0);
        attitude_msg.dcm.dcm01 = cd->attitude.dcm.value()(0, 1);
        attitude_msg.dcm.dcm02 = cd->attitude.dcm.value()(0, 2);
        attitude_msg.dcm.dcm10 = cd->attitude.dcm.value()(1, 0);
        attitude_msg.dcm.dcm11 = cd->attitude.dcm.value()(1, 1);
        attitude_msg.dcm.dcm12 = cd->attitude.dcm.value()(1, 2);
        attitude_msg.dcm.dcm20 = cd->attitude.dcm.value()(2, 0);
        attitude_msg.dcm.dcm21 = cd->attitude.dcm.value()(2, 1);
        attitude_msg.dcm.dcm22 = cd->attitude.dcm.value()(2, 2);
    }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_MAGNED_BIT)
    // ATTITUDE GROUP : MAGNED
    if (cd->attitude.magNed.has_value()) { attitude_msg.magned = toVector3(cd->attitude.magNed.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ACCELNED_BIT)
    // ATTITUDE GROUP : ACCELNED
    if (cd->attitude.accelNed.has_value()) { attitude_msg.accelned = toVector3(cd->attitude.accelNed.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINBODYACC_BIT)
    // ATTITUDE GROUP : LINBODYACC
    if (cd->attitude.linBodyAcc.has_value()) { attitude_msg.linbodyacc = toVector3(cd->attitude.linBodyAcc.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINACCELNED_BIT)
    // ATTITUDE GROUP : LINACCELNED
    if (cd->attitude.linAccelNed.has_value()) { attitude_msg.linaccelned = toVector3(cd->attitude.linAccelNed.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPRU_BIT)
    // ATTITUDE GROUP : YPRU
    if (cd->attitude.yprU.has_value()) { attitude_msg.ypru = toVector3(cd->attitude.yprU.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_HEAVE_BIT)
    // ATTITUDE GROUP : HEAVE
    if (cd->attitude.heave.has_value()) { attitude_msg.heave = toVector3(cd->attitude.heave.value()); }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ATTU_BIT)
    // ATTITUDE GROUP : ATTU
    if (cd->attitude.attU.has_value()) { attitude_msg.attu = cd->attitude.attU.value(); }
#endif

    this->pubAttitude->publish(attitude_msg);

}  // publishAttitudeGroup

void Vectornav::publishInsGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto ins_msg = vectornav_msgs::msg::InsGroup();
    ins_msg.header.stamp = rosTime;
    ins_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (INS_GROUP_ENABLE & INS_INSSTATUS_BIT)
    // INS GROUP : INSSTATUS
    if (cd->ins.insStatus.has_value())
    {
        ins_msg.insstatus.bitfield = uint16_t(cd->ins.insStatus.value());
        ins_msg.insstatus.mode = cd->ins.insStatus->mode;
        ins_msg.insstatus.gnssfix = cd->ins.insStatus->gnssFix;
        ins_msg.insstatus.resv1 = cd->ins.insStatus->resv1;
        ins_msg.insstatus.imuerr = cd->ins.insStatus->imuErr;
        ins_msg.insstatus.magpreserr = cd->ins.insStatus->magPresErr;
        ins_msg.insstatus.gnsserr = cd->ins.insStatus->gnssErr;
        ins_msg.insstatus.resv2 = cd->ins.insStatus->resv2;
        ins_msg.insstatus.gnssheadingins = cd->ins.insStatus->gnssCompassFix & 0x01;
        ins_msg.insstatus.gnsscompass = cd->ins.insStatus->gnssCompassFix & 0x02;
    }
#endif

#if (INS_GROUP_ENABLE & INS_POSLLA_BIT)
    // INS GROUP : POSLLA
    if (cd->ins.posLla.has_value()) { ins_msg.poslla = toPoint3d(cd->ins.posLla.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_POSECEF_BIT)
    // INS GROUP : POSECEF
    if (cd->ins.posEcef.has_value()) { ins_msg.posecef = toPoint3d(cd->ins.posEcef.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_VELBODY_BIT)
    // INS GROUP : VELBODY
    if (cd->ins.velBody.has_value()) { ins_msg.velbody = toVector3(cd->ins.velBody.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_VELNED_BIT)
    // INS GROUP : VELNED
    if (cd->ins.velNed.has_value()) { ins_msg.velned = toVector3(cd->ins.velNed.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_VELECEF_BIT)
    // INS GROUP : VELECEF
    if (cd->ins.velEcef.has_value()) { ins_msg.velecef = toVector3(cd->ins.velEcef.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_MAGECEF_BIT)
    // INS GROUP : MAGECEF
    if (cd->ins.magEcef.has_value()) { ins_msg.magecef = toVector3(cd->ins.magEcef.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_ACCELECEF_BIT)
    // INS GROUP : ACCELECEF
    if (cd->ins.accelEcef.has_value()) { ins_msg.accelecef = toVector3(cd->ins.accelEcef.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_LINACCELECEF_BIT)
    // INS GROUP : LINACCELECEF
    if (cd->ins.linAccelEcef.has_value()) { ins_msg.linaccelecef = toVector3(cd->ins.linAccelEcef.value()); }
#endif

#if (INS_GROUP_ENABLE & INS_POSU_BIT)
    // INS GROUP : POSU
    if (cd->ins.posU.has_value()) { ins_msg.posu = cd->ins.posU.value(); }
#endif

#if (INS_GROUP_ENABLE & INS_VELU_BIT)
    // INS GROUP : VELU
    if (cd->ins.velU.has_value()) { ins_msg.velu = cd->ins.velU.value(); }
#endif

    this->pubIns->publish(ins_msg);

}  // publishInsGroup

void Vectornav::publishGnss2Group(const VN::CompositeData* cd, const rclcpp::Time& rosTime)
{
    auto gnss2_msg = vectornav_msgs::msg::Gnss2Group();
    gnss2_msg.header.stamp = rosTime;
    gnss2_msg.header.frame_id = this->get_parameter("frame_id").as_string();

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUTC_BIT)
    // GNSS2 GROUP : GNSS2TIMEUTC
    if (cd->gnss2.gnss2TimeUtc.has_value())
    {
        gnss2_msg.gnss2timeutc.year = cd->gnss2.gnss2TimeUtc->year;
        gnss2_msg.gnss2timeutc.month = cd->gnss2.gnss2TimeUtc->month;
        gnss2_msg.gnss2timeutc.day = cd->gnss2.gnss2TimeUtc->day;
        gnss2_msg.gnss2timeutc.hour = cd->gnss2.gnss2TimeUtc->hour;
        gnss2_msg.gnss2timeutc.minute = cd->gnss2.gnss2TimeUtc->minute;
        gnss2_msg.gnss2timeutc.second = cd->gnss2.gnss2TimeUtc->second;
        gnss2_msg.gnss2timeutc.fracsec = cd->gnss2.gnss2TimeUtc->fracSec;
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2TOW_BIT)
    // GNSS2 GROUP : GPS2TOW
    if (cd->gnss2.gps2Tow.has_value()) { gnss2_msg.gps2tow = cd->gnss2.gps2Tow->nanoseconds(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2WEEK_BIT)
    // GNSS2 GROUP : GPS2WEEK
    if (cd->gnss2.gps2Week.has_value()) { gnss2_msg.gps2week = cd->gnss2.gps2Week.value(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2NUMSATS_BIT)
    // GNSS2 GROUP : GNSS2NUMSATS
    if (cd->gnss2.gnss2NumSats.has_value()) { gnss2_msg.gnss2numsats = cd->gnss2.gnss2NumSats.value(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2FIX_BIT)
    // GNSS2 GROUP : GNSS2FIX
    if (cd->gnss2.gnss2Fix.has_value()) { gnss2_msg.gnss2fix = cd->gnss2.gnss2Fix.value(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSLLA_BIT)
    // GNSS2 GROUP : GNSS2POSLLA
    if (cd->gnss2.gnss2PosLla.has_value()) { gnss2_msg.gnss2poslla = toPoint3d(cd->gnss2.gnss2PosLla.value()); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSECEF_BIT)
    // GNSS2 GROUP : GNSS2POSECEF
    if (cd->gnss2.gnss2PosEcef.has_value()) { gnss2_msg.gnss2posecef = toPoint3d(cd->gnss2.gnss2PosEcef.value()); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELNED_BIT)
    // GNSS2 GROUP : GNSS2VELNED
    if (cd->gnss2.gnss2VelNed.has_value()) { gnss2_msg.gnss2velned = toVector3(cd->gnss2.gnss2VelNed.value()); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELECEF_BIT)
    // GNSS2 GROUP : GNSS2VELECEF
    if (cd->gnss2.gnss2VelEcef.has_value()) { gnss2_msg.gnss2velecef = toVector3(cd->gnss2.gnss2VelEcef.value()); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSUNCERTAINTY_BIT)
    // GNSS2 GROUP : GNSS2POSUNCERTAINTY
    if (cd->gnss2.gnss2PosUncertainty.has_value()) { gnss2_msg.gnss2posuncertainty = toVector3(cd->gnss2.gnss2PosUncertainty.value()); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELUNCERTAINTY_BIT)
    // GNSS2 GROUP : GNSS2VELUNCERTAINTY
    if (cd->gnss2.gnss2VelUncertainty.has_value()) { gnss2_msg.gnss2veluncertainty = cd->gnss2.gnss2VelUncertainty.value(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUNCERTAINTY_BIT)
    // GNSS2 GROUP : GNSS2TIMEUNCERTAINTY
    if (cd->gnss2.gnss2TimeUncertainty.has_value()) { gnss2_msg.gnss2timeuncertainty = cd->gnss2.gnss2TimeUncertainty.value(); }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEINFO_BIT)
    // GNSS2 GROUP : GNSS2TIMEINFO
    if (cd->gnss2.gnss2TimeInfo.has_value())
    {
        gnss2_msg.gnss2timeinfo.gnsstimestatus = cd->gnss2.gnss2TimeInfo->gnssTimeStatus;
        gnss2_msg.gnss2timeinfo.leapseconds = cd->gnss2.gnss2TimeInfo->leapSeconds;
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2DOP_BIT)
    // GNSS2 GROUP : GNSS2DOP
    if (cd->gnss2.gnss2Dop.has_value())
    {
        gnss2_msg.gnss2dop.gdop = cd->gnss2.gnss2Dop->gDop;
        gnss2_msg.gnss2dop.pdop = cd->gnss2.gnss2Dop->pDop;
        gnss2_msg.gnss2dop.tdop = cd->gnss2.gnss2Dop->tDop;
        gnss2_msg.gnss2dop.vdop = cd->gnss2.gnss2Dop->vDop;
        gnss2_msg.gnss2dop.hdop = cd->gnss2.gnss2Dop->hDop;
        gnss2_msg.gnss2dop.ndop = cd->gnss2.gnss2Dop->nDop;
        gnss2_msg.gnss2dop.edop = cd->gnss2.gnss2Dop->eDop;
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2SATINFO_BIT)
    // GNSS2 GROUP : GNSS2SATINFO
    if (cd->gnss2.gnss2SatInfo.has_value())
    {
        gnss2_msg.gnss2satinfo.numsats = cd->gnss2.gnss2SatInfo->numSats;
        gnss2_msg.gnss2satinfo.resv = cd->gnss2.gnss2SatInfo->resv;
        gnss2_msg.gnss2satinfo.sys = cd->gnss2.gnss2SatInfo->sys;
        gnss2_msg.gnss2satinfo.svid = cd->gnss2.gnss2SatInfo->svId;
        gnss2_msg.gnss2satinfo.flags = cd->gnss2.gnss2SatInfo->flags;
        gnss2_msg.gnss2satinfo.cno = cd->gnss2.gnss2SatInfo->cno;
        gnss2_msg.gnss2satinfo.qi = cd->gnss2.gnss2SatInfo->qi;
        gnss2_msg.gnss2satinfo.el = cd->gnss2.gnss2SatInfo->el;
        gnss2_msg.gnss2satinfo.az = cd->gnss2.gnss2SatInfo->az;
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2RAWMEAS_BIT)
    // GNSS2 GROUP : GNSS2RAWMEAS
    if (cd->gnss2.gnss2RawMeas.has_value())
    {
        gnss2_msg.gnss2rawmeas.tow = cd->gnss2.gnss2RawMeas->tow;
        gnss2_msg.gnss2rawmeas.week = cd->gnss2.gnss2RawMeas->week;
        gnss2_msg.gnss2rawmeas.nummeas = cd->gnss2.gnss2RawMeas->numMeas;
        gnss2_msg.gnss2rawmeas.resv = cd->gnss2.gnss2RawMeas->resv;
        gnss2_msg.gnss2rawmeas.sys = cd->gnss2.gnss2RawMeas->sys;
        gnss2_msg.gnss2rawmeas.svid = cd->gnss2.gnss2RawMeas->svId;
        gnss2_msg.gnss2rawmeas.band = cd->gnss2.gnss2RawMeas->band;
        gnss2_msg.gnss2rawmeas.chan = cd->gnss2.gnss2RawMeas->chan;
        gnss2_msg.gnss2rawmeas.freqnum = cd->gnss2.gnss2RawMeas->freqNum;
        gnss2_msg.gnss2rawmeas.cno = cd->gnss2.gnss2RawMeas->cno;
        gnss2_msg.gnss2rawmeas.flags = cd->gnss2.gnss2RawMeas->flags;
        gnss2_msg.gnss2rawmeas.pr = cd->gnss2.gnss2RawMeas->pr;
        gnss2_msg.gnss2rawmeas.cp = cd->gnss2.gnss2RawMeas->cp;
        gnss2_msg.gnss2rawmeas.dp = cd->gnss2.gnss2RawMeas->dp;
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2STATUS_BIT)
    // GNSS2 GROUP : GNSS2STATUS
    if (cd->gnss2.gnss2Status.has_value())
    {
        gnss2_msg.gnss2status.bitfield = uint16_t(cd->gnss2.gnss2Status.value());
        gnss2_msg.gnss2status.enabled = (gnss2_msg.gnss2status.bitfield & (0x01 << 0));
        gnss2_msg.gnss2status.operational = (gnss2_msg.gnss2status.bitfield & (0x01 << 1));
        gnss2_msg.gnss2status.fix = (gnss2_msg.gnss2status.bitfield & (0x01 << 2));
        gnss2_msg.gnss2status.antennasignalerror = (gnss2_msg.gnss2status.bitfield & (0x01 << 3));
        gnss2_msg.gnss2status.usedfornav = (gnss2_msg.gnss2status.bitfield & (0x01 << 4));
        gnss2_msg.gnss2status.usedforcompass = (gnss2_msg.gnss2status.bitfield & (0x01 << 5));
        gnss2_msg.gnss2status.spoofingdetection = (gnss2_msg.gnss2status.bitfield & (0x03 << 6)) >> 6;
        gnss2_msg.gnss2status.datasource = (gnss2_msg.gnss2status.bitfield & (0x07 << 8)) >> 8;
        gnss2_msg.gnss2status.usedfornavcurr = (gnss2_msg.gnss2status.bitfield & (0x01 << 11));
        gnss2_msg.gnss2status.ppsusedfortime = (gnss2_msg.gnss2status.bitfield & (0x01 << 12));
    }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2ALTMSL_BIT)
    // GNSS2 GROUP : GNSS2ALTMSL
    if (cd->gnss2.gnss2AltMsl.has_value()) { gnss2_msg.gnss2altmsl = cd->gnss2.gnss2AltMsl.value(); }
#endif

    this->pubGnss2->publish(gnss2_msg);

}  // publishGnss2Group

#endif  // VN_PUBLISHING_HPP_
