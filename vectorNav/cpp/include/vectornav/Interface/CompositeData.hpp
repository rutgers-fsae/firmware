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

#ifndef VN_COMPOSITEDATA_HPP_
#define VN_COMPOSITEDATA_HPP_

#include <assert.h>

#include <optional>
#include <variant>

#include "vectornav/Config.hpp"
#include "vectornav/Implementation/AsciiHeader.hpp"
#include "vectornav/Implementation/BinaryHeader.hpp"
#include "vectornav/Implementation/BinaryMeasurementDefinitions.hpp"
#include "vectornav/Interface/Registers.hpp"

namespace VN
{
/// @brief The standard user interface for receiving all measurement data. Auto-generated from protocol.
/// While large in memory use by default, this can be pared down with compile-configuration settings.
class CompositeData
{
public:
    /// @brief The time group provides all timing and event counter related outputs.  Some of these outputs (such as the TimeGps, TimePps, and TimeUtc), require
    /// either that the internal GPS to be enabled, or an external GPS must be present.
    struct TimeGroup
    {
#if (TIME_GROUP_ENABLE & TIME_TIMESTARTUP_BIT)
        std::optional<Time> timeStartup;  ///< The system time since startup measured in nano seconds.  The time since startup is based upon the internal TXCO
                                          ///< oscillator for the MCU.  The accuracy of the internal TXCO is plus minus 20ppm (-40C to 85C).
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMEGPS_BIT)
        std::optional<Time> timeGps;  ///< The absolute GPS time since start of GPS epoch 1980 expressed in nano seconds.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMEGPSTOW_BIT)
        std::optional<Time> timeGpsTow;  ///< The time since the start of the current GPS time week expressed in nano seconds.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMEGPSWEEK_BIT)
        std::optional<uint16_t> timeGpsWeek;  ///< The current GPS week.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMESYNCIN_BIT)
        std::optional<Time> timeSyncIn;  ///< The time since the last SyncIn event trigger expressed in nano seconds.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMEGPSPPS_BIT)
        std::optional<Time> timeGpsPps;  ///< The time since the last GPS PPS trigger event expressed in nano seconds.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMEUTC_BIT)
        std::optional<TimeUtc> timeUtc;  ///< The current UTC time.  The year is given as a signed byte year offset from the year 2000.  For example the year
                                         ///< 2013 would be given as year 13.
#endif
#if (TIME_GROUP_ENABLE & TIME_SYNCINCNT_BIT)
        std::optional<uint32_t> syncInCnt;  ///< The number of SyncIn trigger events that have occurred.
#endif
#if (TIME_GROUP_ENABLE & TIME_SYNCOUTCNT_BIT)
        std::optional<uint32_t> syncOutCnt;  ///< The number of SyncOut trigger events that have occurred.
#endif
#if (TIME_GROUP_ENABLE & TIME_TIMESTATUS_BIT)
        std::optional<TimeStatus> timeStatus;  ///< Time valid status flags.
#endif
    };

    /// @brief The IMU group provides all outputs which are dependent upon the measurements collected from the IMU.
    struct ImuGroup
    {
#if (IMU_GROUP_ENABLE & IMU_IMUSTATUS_BIT)
        std::optional<ImuStatus> imuStatus;  ///< Reports various statuses of the IMU sensors.
#endif
#if (IMU_GROUP_ENABLE & IMU_UNCOMPMAG_BIT)
        std::optional<Vec3f>
            uncompMag;  ///< The IMU magnetic field given in the body-frame.  This measurement is compensated by the static calibration (individual factory
                        ///< calibration stored in flash), and the user compensation, however it is not compensated by the onboard Hard/Soft Iron estimator.
#endif
#if (IMU_GROUP_ENABLE & IMU_UNCOMPACCEL_BIT)
        std::optional<Vec3f>
            uncompAccel;  ///< The IMU acceleration given in the body-frame.  This measurement is compensated by the static calibration (individual factory
                          ///< calibration stored in flash), however it is not compensated by any bias compensation from the onboard Kalman filter.
#endif
#if (IMU_GROUP_ENABLE & IMU_UNCOMPGYRO_BIT)
        std::optional<Vec3f>
            uncompGyro;  ///< The IMU angular rate given in the body-frame.  This measurement is compensated by the static calibration (individual factory
                         ///< calibration stored in flash), however it is not compensated by any bias compensation from the onboard Kalman filter.
#endif
#if (IMU_GROUP_ENABLE & IMU_TEMPERATURE_BIT)
        std::optional<float> temperature;  ///< The IMU temperature.
#endif
#if (IMU_GROUP_ENABLE & IMU_PRESSURE_BIT)
        std::optional<float>
            pressure;  ///< The IMU pressure.  This is an absolute pressure measurement.  Typical pressure at sea level would be around 100 kPa.
#endif
#if (IMU_GROUP_ENABLE & IMU_DELTATHETA_BIT)
        std::optional<DeltaTheta> deltaTheta;  ///< The delta theta is the delta rotation angles incurred due to rotation, since the last time the values were
                                               ///< output by the device. The delta angles are calculated based upon the onboard conning and sculling
                                               ///< integration performed onboard the sensor at the IMU sampling rate. The delta time is the time interval that
                                               ///< the delta angle and velocities are integrated over. The integration for the delta angles are reset each time
                                               ///< the values are either polled or sent out due to a scheduled asynchronous ASCII or binary output.
#endif
#if (IMU_GROUP_ENABLE & IMU_DELTAVEL_BIT)
        std::optional<Vec3f> deltaVel;  ///< The delta velocity is the delta velocity incurred due to motion, since the last time the values were output by the
                                        ///< device. The delta velocities are calculated based upon the onboard conning and sculling integration performed
                                        ///< onboard the sensor at the IMU sampling rate. The integration for the delta velocities are reset each time the
                                        ///< values are either polled or sent out due to a scheduled asynchronous ASCII or binary output.
#endif
#if (IMU_GROUP_ENABLE & IMU_MAG_BIT)
        std::optional<Vec3f>
            mag;  ///< The IMU compensated magnetic field given in the body-frame.  This measurement is compensated by the static calibration (individual
                  ///< factory calibration stored in flash), the user compensation, and the dynamic calibration from the onboard Hard/Soft Iron estimator.
#endif
#if (IMU_GROUP_ENABLE & IMU_ACCEL_BIT)
        std::optional<Vec3f> accel;  ///< The bias-compensated acceleration measured in the body-frame. This measurement is compensated by the static
                                     ///< calibration (individual factory calibration stored in flash), the user compensation, and the dynamic bias compensation
                                     ///< from the onboard Kalman filter (if applicable).
#endif
#if (IMU_GROUP_ENABLE & IMU_ANGULARRATE_BIT)
        std::optional<Vec3f> angularRate;  ///< The bias-compensated angular rate measured in the body-frame.  This measurement is compensated by the static
                                           ///< calibration (individual factory calibration stored in flash), the user compensation, and the dynamic bias
                                           ///< compensation from the onboard Kalman filter.
#endif
#if (IMU_GROUP_ENABLE & IMU_SENSSAT_BIT)
        std::optional<uint16_t> sensSat;  ///< This field provides flags identifying whether any of the measurements are currently saturated.
#endif
    };

    /// @brief The GNSS group provides all outputs which are dependent upon the measurements collected from the primary GNSS receiver.  All data in this group
    /// is updated at the rate of the GPS receiver (nominally 5Hz for the internal GNSS).
    struct GnssGroup
    {
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUTC_BIT)
        std::optional<TimeUtc> gnss1TimeUtc;  ///< The current UTC time.  The year is given as a signed byte year offset from the year 2000.  For example the
                                              ///< year 2013 would be given as year 13.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GPS1TOW_BIT)
        std::optional<Time> gps1Tow;  ///< The GPS time of week given in nano seconds.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GPS1WEEK_BIT)
        std::optional<uint16_t> gps1Week;  ///< The current GPS week.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1NUMSATS_BIT)
        std::optional<uint8_t> gnss1NumSats;  ///< The number of tracked GNSS satellites.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1FIX_BIT)
        std::optional<uint8_t> gnss1Fix;  ///< The current GNSS fix.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSLLA_BIT)
        std::optional<Lla> gnss1PosLla;  ///< The current GNSS position measurement given as the geodetic latitude, longitude and altitude above the ellipsoid.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSECEF_BIT)
        std::optional<Vec3d> gnss1PosEcef;  ///< The current GNSS position given in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELNED_BIT)
        std::optional<Vec3f> gnss1VelNed;  ///< The current GNSS velocity in the North East Down (NED) reference frame.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELECEF_BIT)
        std::optional<Vec3f> gnss1VelEcef;  ///< The current GNSS velocity in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSUNCERTAINTY_BIT)
        std::optional<Vec3f> gnss1PosUncertainty;  ///< The current GNSS position uncertainty in the North East Down (NED) reference frame.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELUNCERTAINTY_BIT)
        std::optional<float> gnss1VelUncertainty;  ///< The current GNSS velocity uncertainty.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUNCERTAINTY_BIT)
        std::optional<float> gnss1TimeUncertainty;  ///< The current GPS time uncertainty.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEINFO_BIT)
        std::optional<GnssTimeInfo> gnss1TimeInfo;  ///< Flags for valid GPS TOW, week number and UTC and current leap seconds.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1DOP_BIT)
        std::optional<GnssDop> gnss1Dop;  ///< Dilution of precision.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1SATINFO_BIT)
        std::optional<GnssSatInfo> gnss1SatInfo;  ///< Information and measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1RAWMEAS_BIT)
        std::optional<GnssRawMeas> gnss1RawMeas;  ///< Raw measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1STATUS_BIT)
        std::optional<GnssStatus> gnss1Status;  ///< GNSS Status info flags.
#endif
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1ALTMSL_BIT)
        std::optional<double> gnss1AltMsl;  ///< Altitude (Mean Sea Level).
#endif
    };

    /// @brief The attitude group provides all estimated outputs which are dependent upon the estimated attitude solution.  The attitude will be derived from
    /// either the AHRS or the INS, depending upon which filter is currently active and tracking.  All of the fields in this group will only be valid if the
    /// AHRS/INS filter is currently enabled and tracking.
    struct AttitudeGroup
    {
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPR_BIT)
        std::optional<Ypr> ypr;  ///< The estimated attitude describing the body frame with respect to the local North-East-Down (NED) frame given as the
                                 ///< (3-2-1) set of Euler angles corresponding to Yaw-Pitch-Roll.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_QUATERNION_BIT)
        std::optional<Quat>
            quaternion;  ///< The estimated attitude describing the body frame with respect to the local North-East-Down (NED) frame given as the quaternion.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_DCM_BIT)
        std::optional<Mat3f> dcm;  ///< The estimated attitude given as the directional cosine matrix (DCM) in column major order mapping the local
                                   ///< North-East-Down (NED) frame into the body frame.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_MAGNED_BIT)
        std::optional<Vec3f> magNed;  ///< The current estimated magnetic field given in the local North-East-Down (NED) frame.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ACCELNED_BIT)
        std::optional<Vec3f> accelNed;  ///< The estimated acceleration (with gravity) given in the local North-East-Down (NED) frame.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINBODYACC_BIT)
        std::optional<Vec3f> linBodyAcc;  ///< The estimated linear acceleration (without gravity) given in the body frame.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINACCELNED_BIT)
        std::optional<Vec3f> linAccelNed;  ///< The estimated linear acceleration (without gravity) given in the local North-East-Down (NED) frame.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPRU_BIT)
        std::optional<Vec3f> yprU;  ///< The estimated attitude (Yaw, Pitch, Roll) uncertainty (1 Sigma), reported in degrees.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_HEAVE_BIT)
        std::optional<Vec3f> heave;  ///< Real-time heave and heave-rate estimates, plus a delayed-heave estimate.
#endif
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ATTU_BIT)
        std::optional<float> attU;  ///< The estimated uncertainty (1 Sigma) in the current attitude estimate.
#endif
    };

    /// @brief The INS group provides all estimated outputs which are dependent upon the onboard INS state solution.  All of the fields in this group will only
    /// be valid if the INS filter is currently enabled and tracking.
    struct InsGroup
    {
#if (INS_GROUP_ENABLE & INS_INSSTATUS_BIT)
        std::optional<InsStatus> insStatus;  ///< The INS status bitfield.
#endif
#if (INS_GROUP_ENABLE & INS_POSLLA_BIT)
        std::optional<Lla> posLla;  ///< The estimated position given as latitude, longitude, and altitude.
#endif
#if (INS_GROUP_ENABLE & INS_POSECEF_BIT)
        std::optional<Vec3d> posEcef;  ///< The estimate position given in the Earth centered Earth fixed (ECEF) frame.
#endif
#if (INS_GROUP_ENABLE & INS_VELBODY_BIT)
        std::optional<Vec3f> velBody;  ///< The estimated velocity in the body-frame.
#endif
#if (INS_GROUP_ENABLE & INS_VELNED_BIT)
        std::optional<Vec3f> velNed;  ///< The estimated velocity in the North East Down (NED) frame.
#endif
#if (INS_GROUP_ENABLE & INS_VELECEF_BIT)
        std::optional<Vec3f> velEcef;  ///< The estimated velocity in the Earth centered Earth fixed (ECEF) frame.
#endif
#if (INS_GROUP_ENABLE & INS_MAGECEF_BIT)
        std::optional<Vec3f> magEcef;  ///< The compensated magnetic measurement in the Earth centered Earth fixed (ECEF) frame.
#endif
#if (INS_GROUP_ENABLE & INS_ACCELECEF_BIT)
        std::optional<Vec3f>
            accelEcef;  ///< The estimated acceleration (with gravity) given in the Earth centered Earth fixed (ECEF) frame. The acceleration measurement has
                        ///< been bias compensated by the onboard INS filter. This measurement is attitude dependent because the attitude is used to map the
                        ///< measurement from the body-frame into the inertial (ECEF) frame. If the device is stationary and the INS filter is tracking, the
                        ///< measurement should be nominally equivalent to the gravity reference vector in the inertial frame (ECEF).
#endif
#if (INS_GROUP_ENABLE & INS_LINACCELECEF_BIT)
        std::optional<Vec3f>
            linAccelEcef;  ///< The estimated linear acceleration (without gravity) and given in the Earth centered Earth fixed (ECEF) frame. This measurement
                           ///< is attitude dependent as the attitude solution is used to map the measurement from the body-frame into the inertial (ECEF)
                           ///< frame. This acceleration measurement has been bias compensated by the onboard INS filter, and the gravity component has been
                           ///< removed using the current gravity reference vector estimate. If the device is stationary and the onboard INS filter is tracking,
                           ///< the measurement will nominally read 0 on all three axes.
#endif
#if (INS_GROUP_ENABLE & INS_POSU_BIT)
        std::optional<float> posU;  ///< The estimated uncertainty (1 Sigma) in the current position estimate.
#endif
#if (INS_GROUP_ENABLE & INS_VELU_BIT)
        std::optional<float> velU;  ///< The estimated uncertainty (1 Sigma) in the current velocity estimate.
#endif
    };

    /// @brief The GNSS 2 group provides all outputs which are dependent upon the measurements collected from the secondary GNSS receiver.  All data in this
    /// group is updated at the rate of the GPS receiver (nominally 5Hz for the internal GNSS).
    struct Gnss2Group
    {
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUTC_BIT)
        std::optional<TimeUtc> gnss2TimeUtc;  ///< The current UTC time.  The year is given as a signed byte year offset from the year 2000.  For example the
                                              ///< year 2013 would be given as year 13.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2TOW_BIT)
        std::optional<Time> gps2Tow;  ///< The GPS time of week given in nano seconds.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2WEEK_BIT)
        std::optional<uint16_t> gps2Week;  ///< The current GPS week.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2NUMSATS_BIT)
        std::optional<uint8_t> gnss2NumSats;  ///< The number of tracked GNSS satellites.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2FIX_BIT)
        std::optional<uint8_t> gnss2Fix;  ///< The current GNSS fix.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSLLA_BIT)
        std::optional<Lla> gnss2PosLla;  ///< The current GNSS position measurement given as the geodetic latitude, longitude and altitude above the ellipsoid.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSECEF_BIT)
        std::optional<Vec3d> gnss2PosEcef;  ///< The current GNSS position given in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELNED_BIT)
        std::optional<Vec3f> gnss2VelNed;  ///< The current GNSS velocity in the North East Down (NED) reference frame.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELECEF_BIT)
        std::optional<Vec3f> gnss2VelEcef;  ///< The current GNSS velocity in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSUNCERTAINTY_BIT)
        std::optional<Vec3f> gnss2PosUncertainty;  ///< The current GNSS position uncertainty in the North East Down (NED) reference frame.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELUNCERTAINTY_BIT)
        std::optional<float> gnss2VelUncertainty;  ///< The current GNSS velocity uncertainty.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUNCERTAINTY_BIT)
        std::optional<float> gnss2TimeUncertainty;  ///< The current GPS time uncertainty.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEINFO_BIT)
        std::optional<GnssTimeInfo> gnss2TimeInfo;  ///< Flags for valid GPS TOW, week number and UTC and current leap seconds.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2DOP_BIT)
        std::optional<GnssDop> gnss2Dop;  ///< Dilution of precision.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2SATINFO_BIT)
        std::optional<GnssSatInfo> gnss2SatInfo;  ///< Information and measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2RAWMEAS_BIT)
        std::optional<GnssRawMeas> gnss2RawMeas;  ///< Raw measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2STATUS_BIT)
        std::optional<GnssStatus> gnss2Status;  ///< GNSS Status info flags.
#endif
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2ALTMSL_BIT)
        std::optional<double> gnss2AltMsl;  ///< Altitude (Mean Sea Level).
#endif
    };

    /// @brief The GNSS 3 group provides all outputs which are dependent upon the measurements collected from the secondary onboard GNSS, if external GNSS is
    /// enabled.  All data in this group is updated at the rate of the GNSS receiver (nominally 5Hz for the internal GNSS).
    struct Gnss3Group
    {
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEUTC_BIT)
        std::optional<TimeUtc> gnss3TimeUtc;  ///< The current UTC time.  The year is given as a signed byte year offset from the year 2000.  For example the
                                              ///< year 2013 would be given as year 13.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GPS3TOW_BIT)
        std::optional<Time> gps3Tow;  ///< The GPS time of week given in nano seconds.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GPS3WEEK_BIT)
        std::optional<uint16_t> gps3Week;  ///< The current GPS week.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3NUMSATS_BIT)
        std::optional<uint8_t> gnss3NumSats;  ///< The number of tracked GNSS satellites.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3FIX_BIT)
        std::optional<uint8_t> gnss3Fix;  ///< The current GNSS fix.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSLLA_BIT)
        std::optional<Lla> gnss3PosLla;  ///< The current GNSS position measurement given as the geodetic latitude, longitude and altitude above the ellipsoid.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSECEF_BIT)
        std::optional<Vec3d> gnss3PosEcef;  ///< The current GNSS position given in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELNED_BIT)
        std::optional<Vec3f> gnss3VelNed;  ///< The current GNSS velocity in the North East Down (NED) reference frame.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELECEF_BIT)
        std::optional<Vec3f> gnss3VelEcef;  ///< The current GNSS velocity in the Earth centered Earth fixed (ECEF) reference frame.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSUNCERTAINTY_BIT)
        std::optional<Vec3f> gnss3PosUncertainty;  ///< The current GNSS position uncertainty in the North East Down (NED) reference frame.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELUNCERTAINTY_BIT)
        std::optional<float> gnss3VelUncertainty;  ///< The current GNSS velocity uncertainty.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEUNCERTAINTY_BIT)
        std::optional<float> gnss3TimeUncertainty;  ///< The current GPS time uncertainty.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEINFO_BIT)
        std::optional<GnssTimeInfo> gnss3TimeInfo;  ///< Flags for valid GPS TOW, week number and UTC and current leap seconds.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3DOP_BIT)
        std::optional<GnssDop> gnss3Dop;  ///< Dilution of precision.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3SATINFO_BIT)
        std::optional<GnssSatInfo> gnss3SatInfo;  ///< Information and measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3RAWMEAS_BIT)
        std::optional<GnssRawMeas> gnss3RawMeas;  ///< Raw measurements pertaining to each GNSS satellite in view.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3STATUS_BIT)
        std::optional<GnssStatus> gnss3Status;  ///< GNSS Status info flags.
#endif
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3ALTMSL_BIT)
        std::optional<double> gnss3AltMsl;  ///< Altitude (Mean Sea Level).
#endif
    };

    /// @brief Default constructor that initializes an empty CompositeData object.
    CompositeData() {};

    /// @brief Constructor that initializes CompositeData with ASCII message header information.
    /// @param asciiHeader The ASCII header that describes the source message format and fields.
    CompositeData(AsciiHeader asciiHeader) : _asciiHeader(asciiHeader) {};

    /// @brief Constructor that initializes CompositeData with binary message header information.
    /// @param binaryHeader The binary header that describes the source message format and fields.
    CompositeData(BinaryHeader binaryHeader) : _binaryHeader(binaryHeader) {};

    /// @brief Checks whether the passed header matches the header of the message which populated this object.
    /// @param asciiHeader The header to check.
    /// @return Whether the passed header matches.
    bool matchesMessage(const AsciiHeader& asciiHeader) const noexcept
    {
        if (_asciiHeader.has_value()) { return asciiHeader == _asciiHeader; }
        else { return false; }
    }

    /// @brief Checks whether the passed header matches the header of the message which populated this object.
    /// @param binaryHeader The header to check.
    /// @return Whether the passed header matches.
    bool matchesMessage(const BinaryHeader& binaryHeader) const noexcept
    {
        if (_binaryHeader.has_value()) { return binaryHeader == _binaryHeader; }
        else { return false; }
    }

    /// @brief Checks whether the passed register matches the header of the message which populated this object.
    /// @param binaryOutputRegister The regsiter to check.
    /// @return Whether the passed register matches.
    bool matchesMessage(const Registers::System::BinaryOutput& binaryOutputRegister) const noexcept
    {
        return matchesMessage(binaryOutputRegister.toBinaryHeader());
    }

    std::variant<AsciiHeader, BinaryHeader> header() const noexcept
    {
        if (_asciiHeader.has_value()) { return _asciiHeader.value(); }
        else if (_binaryHeader.has_value()) { return _binaryHeader.value(); }
        else { VN_ABORT(); }
    }

    time_point timestamp;

#if (TIME_GROUP_ENABLE)
    TimeGroup time;
#endif
#if (IMU_GROUP_ENABLE)
    ImuGroup imu;
#endif
#if (GNSS_GROUP_ENABLE)
    GnssGroup gnss;
#endif
#if (ATTITUDE_GROUP_ENABLE)
    AttitudeGroup attitude;
#endif
#if (INS_GROUP_ENABLE)
    InsGroup ins;
#endif
#if (GNSS2_GROUP_ENABLE)
    Gnss2Group gnss2;
#endif
#if (GNSS3_GROUP_ENABLE)
    Gnss3Group gnss3;
#endif

    std::optional<uint32_t> asciiAppendCount;
    std::optional<uint16_t> asciiAppendStatus;

    template <class Extractor>
    Errored copyFromBuffer(Extractor& extractor, const uint8_t measGroupIndex, const uint8_t measTypeIndex);

private:
    std::optional<AsciiHeader> _asciiHeader = std::nullopt;
    std::optional<BinaryHeader> _binaryHeader = std::nullopt;

};  // class CompositeData

template <class Extractor>
Errored CompositeData::copyFromBuffer(Extractor& extractor, const uint8_t measGroupIndex, const uint8_t measTypeIndex)
{
    switch (measGroupIndex)
    {
        case 0:
        {
            switch (1 << measTypeIndex)
            {
#if (TIME_GROUP_ENABLE & TIME_TIMESTARTUP_BIT)
                case COMMON_TIMESTARTUP_BIT:
                {
                    return extractor.extract(time.timeStartup);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPS_BIT)
                case COMMON_TIMEGPS_BIT:
                {
                    return extractor.extract(time.timeGps);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESYNCIN_BIT)
                case COMMON_TIMESYNCIN_BIT:
                {
                    return extractor.extract(time.timeSyncIn);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPR_BIT)
                case COMMON_YPR_BIT:
                {
                    return extractor.extract(attitude.ypr);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_QUATERNION_BIT)
                case COMMON_QUATERNION_BIT:
                {
                    return extractor.extract(attitude.quaternion);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_ANGULARRATE_BIT)
                case COMMON_ANGULARRATE_BIT:
                {
                    return extractor.extract(imu.angularRate);
                }
#endif

#if (INS_GROUP_ENABLE & INS_POSLLA_BIT)
                case COMMON_POSLLA_BIT:
                {
                    return extractor.extract(ins.posLla);
                }
#endif

#if (INS_GROUP_ENABLE & INS_VELNED_BIT)
                case COMMON_VELNED_BIT:
                {
                    return extractor.extract(ins.velNed);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_ACCEL_BIT)
                case COMMON_ACCEL_BIT:
                {
                    return extractor.extract(imu.accel);
                }
#endif

#if ((IMU_GROUP_ENABLE & IMU_UNCOMPACCEL_BIT) && (IMU_GROUP_ENABLE & IMU_UNCOMPGYRO_BIT))
                case COMMON_IMU_BIT:
                {
                    return !(!extractor.extract(imu.uncompAccel) && !extractor.extract(imu.uncompGyro));
                }
#endif

#if ((IMU_GROUP_ENABLE & IMU_MAG_BIT) && (IMU_GROUP_ENABLE & IMU_PRESSURE_BIT) && (IMU_GROUP_ENABLE & IMU_TEMPERATURE_BIT))
                case COMMON_MAGPRES_BIT:
                {
                    return !(!extractor.extract(imu.mag) && !extractor.extract(imu.temperature) && !extractor.extract(imu.pressure));
                }
#endif

#if ((IMU_GROUP_ENABLE & IMU_DELTATHETA_BIT) && (IMU_GROUP_ENABLE & IMU_DELTAVEL_BIT))
                case COMMON_DELTAS_BIT:
                {
                    return !(!extractor.extract(imu.deltaTheta) && !extractor.extract(imu.deltaVel));
                }
#endif

#if (INS_GROUP_ENABLE & INS_INSSTATUS_BIT)
                case COMMON_INSSTATUS_BIT:
                {
                    return extractor.extract(ins.insStatus);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCINCNT_BIT)
                case COMMON_SYNCINCNT_BIT:
                {
                    return extractor.extract(time.syncInCnt);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSPPS_BIT)
                case COMMON_TIMEGPSPPS_BIT:
                {
                    return extractor.extract(time.timeGpsPps);
                }
#endif

                default:
                {
                    return true;
                }
            }  // case 0:
        }  // switch (measGroupIndex)

        case 1:
        {
            switch (1 << measTypeIndex)
            {
#if (TIME_GROUP_ENABLE & TIME_TIMESTARTUP_BIT)
                case TIME_TIMESTARTUP_BIT:
                {
                    return extractor.extract(time.timeStartup);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPS_BIT)
                case TIME_TIMEGPS_BIT:
                {
                    return extractor.extract(time.timeGps);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSTOW_BIT)
                case TIME_TIMEGPSTOW_BIT:
                {
                    return extractor.extract(time.timeGpsTow);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSWEEK_BIT)
                case TIME_TIMEGPSWEEK_BIT:
                {
                    return extractor.extract(time.timeGpsWeek);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESYNCIN_BIT)
                case TIME_TIMESYNCIN_BIT:
                {
                    return extractor.extract(time.timeSyncIn);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEGPSPPS_BIT)
                case TIME_TIMEGPSPPS_BIT:
                {
                    return extractor.extract(time.timeGpsPps);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMEUTC_BIT)
                case TIME_TIMEUTC_BIT:
                {
                    return extractor.extract(time.timeUtc);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCINCNT_BIT)
                case TIME_SYNCINCNT_BIT:
                {
                    return extractor.extract(time.syncInCnt);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_SYNCOUTCNT_BIT)
                case TIME_SYNCOUTCNT_BIT:
                {
                    return extractor.extract(time.syncOutCnt);
                }
#endif

#if (TIME_GROUP_ENABLE & TIME_TIMESTATUS_BIT)
                case TIME_TIMESTATUS_BIT:
                {
                    return extractor.extract(time.timeStatus);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 1:

        case 2:
        {
            switch (1 << measTypeIndex)
            {
#if (IMU_GROUP_ENABLE & IMU_IMUSTATUS_BIT)
                case IMU_IMUSTATUS_BIT:
                {
                    return extractor.extract(imu.imuStatus);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPMAG_BIT)
                case IMU_UNCOMPMAG_BIT:
                {
                    return extractor.extract(imu.uncompMag);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPACCEL_BIT)
                case IMU_UNCOMPACCEL_BIT:
                {
                    return extractor.extract(imu.uncompAccel);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_UNCOMPGYRO_BIT)
                case IMU_UNCOMPGYRO_BIT:
                {
                    return extractor.extract(imu.uncompGyro);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_TEMPERATURE_BIT)
                case IMU_TEMPERATURE_BIT:
                {
                    return extractor.extract(imu.temperature);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_PRESSURE_BIT)
                case IMU_PRESSURE_BIT:
                {
                    return extractor.extract(imu.pressure);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_DELTATHETA_BIT)
                case IMU_DELTATHETA_BIT:
                {
                    return extractor.extract(imu.deltaTheta);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_DELTAVEL_BIT)
                case IMU_DELTAVEL_BIT:
                {
                    return extractor.extract(imu.deltaVel);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_MAG_BIT)
                case IMU_MAG_BIT:
                {
                    return extractor.extract(imu.mag);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_ACCEL_BIT)
                case IMU_ACCEL_BIT:
                {
                    return extractor.extract(imu.accel);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_ANGULARRATE_BIT)
                case IMU_ANGULARRATE_BIT:
                {
                    return extractor.extract(imu.angularRate);
                }
#endif

#if (IMU_GROUP_ENABLE & IMU_SENSSAT_BIT)
                case IMU_SENSSAT_BIT:
                {
                    return extractor.extract(imu.sensSat);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 2:

        case 3:
        {
            switch (1 << measTypeIndex)
            {
#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUTC_BIT)
                case GNSS_GNSS1TIMEUTC_BIT:
                {
                    return extractor.extract(gnss.gnss1TimeUtc);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GPS1TOW_BIT)
                case GNSS_GPS1TOW_BIT:
                {
                    return extractor.extract(gnss.gps1Tow);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GPS1WEEK_BIT)
                case GNSS_GPS1WEEK_BIT:
                {
                    return extractor.extract(gnss.gps1Week);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1NUMSATS_BIT)
                case GNSS_GNSS1NUMSATS_BIT:
                {
                    return extractor.extract(gnss.gnss1NumSats);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1FIX_BIT)
                case GNSS_GNSS1FIX_BIT:
                {
                    return extractor.extract(gnss.gnss1Fix);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSLLA_BIT)
                case GNSS_GNSS1POSLLA_BIT:
                {
                    return extractor.extract(gnss.gnss1PosLla);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSECEF_BIT)
                case GNSS_GNSS1POSECEF_BIT:
                {
                    return extractor.extract(gnss.gnss1PosEcef);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELNED_BIT)
                case GNSS_GNSS1VELNED_BIT:
                {
                    return extractor.extract(gnss.gnss1VelNed);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELECEF_BIT)
                case GNSS_GNSS1VELECEF_BIT:
                {
                    return extractor.extract(gnss.gnss1VelEcef);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1POSUNCERTAINTY_BIT)
                case GNSS_GNSS1POSUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss.gnss1PosUncertainty);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1VELUNCERTAINTY_BIT)
                case GNSS_GNSS1VELUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss.gnss1VelUncertainty);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEUNCERTAINTY_BIT)
                case GNSS_GNSS1TIMEUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss.gnss1TimeUncertainty);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1TIMEINFO_BIT)
                case GNSS_GNSS1TIMEINFO_BIT:
                {
                    return extractor.extract(gnss.gnss1TimeInfo);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1DOP_BIT)
                case GNSS_GNSS1DOP_BIT:
                {
                    return extractor.extract(gnss.gnss1Dop);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1SATINFO_BIT)
                case GNSS_GNSS1SATINFO_BIT:
                {
                    return extractor.extract(gnss.gnss1SatInfo);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1RAWMEAS_BIT)
                case GNSS_GNSS1RAWMEAS_BIT:
                {
                    return extractor.extract(gnss.gnss1RawMeas);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1STATUS_BIT)
                case GNSS_GNSS1STATUS_BIT:
                {
                    return extractor.extract(gnss.gnss1Status);
                }
#endif

#if (GNSS_GROUP_ENABLE & GNSS_GNSS1ALTMSL_BIT)
                case GNSS_GNSS1ALTMSL_BIT:
                {
                    return extractor.extract(gnss.gnss1AltMsl);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 3:

        case 4:
        {
            switch (1 << measTypeIndex)
            {
#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPR_BIT)
                case ATTITUDE_YPR_BIT:
                {
                    return extractor.extract(attitude.ypr);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_QUATERNION_BIT)
                case ATTITUDE_QUATERNION_BIT:
                {
                    return extractor.extract(attitude.quaternion);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_DCM_BIT)
                case ATTITUDE_DCM_BIT:
                {
                    return extractor.extract(attitude.dcm);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_MAGNED_BIT)
                case ATTITUDE_MAGNED_BIT:
                {
                    return extractor.extract(attitude.magNed);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ACCELNED_BIT)
                case ATTITUDE_ACCELNED_BIT:
                {
                    return extractor.extract(attitude.accelNed);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINBODYACC_BIT)
                case ATTITUDE_LINBODYACC_BIT:
                {
                    return extractor.extract(attitude.linBodyAcc);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_LINACCELNED_BIT)
                case ATTITUDE_LINACCELNED_BIT:
                {
                    return extractor.extract(attitude.linAccelNed);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_YPRU_BIT)
                case ATTITUDE_YPRU_BIT:
                {
                    return extractor.extract(attitude.yprU);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_HEAVE_BIT)
                case ATTITUDE_HEAVE_BIT:
                {
                    return extractor.extract(attitude.heave);
                }
#endif

#if (ATTITUDE_GROUP_ENABLE & ATTITUDE_ATTU_BIT)
                case ATTITUDE_ATTU_BIT:
                {
                    return extractor.extract(attitude.attU);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 4:

        case 5:
        {
            switch (1 << measTypeIndex)
            {
#if (INS_GROUP_ENABLE & INS_INSSTATUS_BIT)
                case INS_INSSTATUS_BIT:
                {
                    return extractor.extract(ins.insStatus);
                }
#endif

#if (INS_GROUP_ENABLE & INS_POSLLA_BIT)
                case INS_POSLLA_BIT:
                {
                    return extractor.extract(ins.posLla);
                }
#endif

#if (INS_GROUP_ENABLE & INS_POSECEF_BIT)
                case INS_POSECEF_BIT:
                {
                    return extractor.extract(ins.posEcef);
                }
#endif

#if (INS_GROUP_ENABLE & INS_VELBODY_BIT)
                case INS_VELBODY_BIT:
                {
                    return extractor.extract(ins.velBody);
                }
#endif

#if (INS_GROUP_ENABLE & INS_VELNED_BIT)
                case INS_VELNED_BIT:
                {
                    return extractor.extract(ins.velNed);
                }
#endif

#if (INS_GROUP_ENABLE & INS_VELECEF_BIT)
                case INS_VELECEF_BIT:
                {
                    return extractor.extract(ins.velEcef);
                }
#endif

#if (INS_GROUP_ENABLE & INS_MAGECEF_BIT)
                case INS_MAGECEF_BIT:
                {
                    return extractor.extract(ins.magEcef);
                }
#endif

#if (INS_GROUP_ENABLE & INS_ACCELECEF_BIT)
                case INS_ACCELECEF_BIT:
                {
                    return extractor.extract(ins.accelEcef);
                }
#endif

#if (INS_GROUP_ENABLE & INS_LINACCELECEF_BIT)
                case INS_LINACCELECEF_BIT:
                {
                    return extractor.extract(ins.linAccelEcef);
                }
#endif

#if (INS_GROUP_ENABLE & INS_POSU_BIT)
                case INS_POSU_BIT:
                {
                    return extractor.extract(ins.posU);
                }
#endif

#if (INS_GROUP_ENABLE & INS_VELU_BIT)
                case INS_VELU_BIT:
                {
                    return extractor.extract(ins.velU);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 5:

        case 6:
        {
            switch (1 << measTypeIndex)
            {
#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUTC_BIT)
                case GNSS2_GNSS2TIMEUTC_BIT:
                {
                    return extractor.extract(gnss2.gnss2TimeUtc);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2TOW_BIT)
                case GNSS2_GPS2TOW_BIT:
                {
                    return extractor.extract(gnss2.gps2Tow);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GPS2WEEK_BIT)
                case GNSS2_GPS2WEEK_BIT:
                {
                    return extractor.extract(gnss2.gps2Week);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2NUMSATS_BIT)
                case GNSS2_GNSS2NUMSATS_BIT:
                {
                    return extractor.extract(gnss2.gnss2NumSats);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2FIX_BIT)
                case GNSS2_GNSS2FIX_BIT:
                {
                    return extractor.extract(gnss2.gnss2Fix);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSLLA_BIT)
                case GNSS2_GNSS2POSLLA_BIT:
                {
                    return extractor.extract(gnss2.gnss2PosLla);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSECEF_BIT)
                case GNSS2_GNSS2POSECEF_BIT:
                {
                    return extractor.extract(gnss2.gnss2PosEcef);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELNED_BIT)
                case GNSS2_GNSS2VELNED_BIT:
                {
                    return extractor.extract(gnss2.gnss2VelNed);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELECEF_BIT)
                case GNSS2_GNSS2VELECEF_BIT:
                {
                    return extractor.extract(gnss2.gnss2VelEcef);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2POSUNCERTAINTY_BIT)
                case GNSS2_GNSS2POSUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss2.gnss2PosUncertainty);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2VELUNCERTAINTY_BIT)
                case GNSS2_GNSS2VELUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss2.gnss2VelUncertainty);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEUNCERTAINTY_BIT)
                case GNSS2_GNSS2TIMEUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss2.gnss2TimeUncertainty);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2TIMEINFO_BIT)
                case GNSS2_GNSS2TIMEINFO_BIT:
                {
                    return extractor.extract(gnss2.gnss2TimeInfo);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2DOP_BIT)
                case GNSS2_GNSS2DOP_BIT:
                {
                    return extractor.extract(gnss2.gnss2Dop);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2SATINFO_BIT)
                case GNSS2_GNSS2SATINFO_BIT:
                {
                    return extractor.extract(gnss2.gnss2SatInfo);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2RAWMEAS_BIT)
                case GNSS2_GNSS2RAWMEAS_BIT:
                {
                    return extractor.extract(gnss2.gnss2RawMeas);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2STATUS_BIT)
                case GNSS2_GNSS2STATUS_BIT:
                {
                    return extractor.extract(gnss2.gnss2Status);
                }
#endif

#if (GNSS2_GROUP_ENABLE & GNSS2_GNSS2ALTMSL_BIT)
                case GNSS2_GNSS2ALTMSL_BIT:
                {
                    return extractor.extract(gnss2.gnss2AltMsl);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 6:

        case 12:
        {
            switch (1 << measTypeIndex)
            {
#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEUTC_BIT)
                case GNSS3_GNSS3TIMEUTC_BIT:
                {
                    return extractor.extract(gnss3.gnss3TimeUtc);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GPS3TOW_BIT)
                case GNSS3_GPS3TOW_BIT:
                {
                    return extractor.extract(gnss3.gps3Tow);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GPS3WEEK_BIT)
                case GNSS3_GPS3WEEK_BIT:
                {
                    return extractor.extract(gnss3.gps3Week);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3NUMSATS_BIT)
                case GNSS3_GNSS3NUMSATS_BIT:
                {
                    return extractor.extract(gnss3.gnss3NumSats);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3FIX_BIT)
                case GNSS3_GNSS3FIX_BIT:
                {
                    return extractor.extract(gnss3.gnss3Fix);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSLLA_BIT)
                case GNSS3_GNSS3POSLLA_BIT:
                {
                    return extractor.extract(gnss3.gnss3PosLla);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSECEF_BIT)
                case GNSS3_GNSS3POSECEF_BIT:
                {
                    return extractor.extract(gnss3.gnss3PosEcef);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELNED_BIT)
                case GNSS3_GNSS3VELNED_BIT:
                {
                    return extractor.extract(gnss3.gnss3VelNed);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELECEF_BIT)
                case GNSS3_GNSS3VELECEF_BIT:
                {
                    return extractor.extract(gnss3.gnss3VelEcef);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3POSUNCERTAINTY_BIT)
                case GNSS3_GNSS3POSUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss3.gnss3PosUncertainty);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3VELUNCERTAINTY_BIT)
                case GNSS3_GNSS3VELUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss3.gnss3VelUncertainty);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEUNCERTAINTY_BIT)
                case GNSS3_GNSS3TIMEUNCERTAINTY_BIT:
                {
                    return extractor.extract(gnss3.gnss3TimeUncertainty);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3TIMEINFO_BIT)
                case GNSS3_GNSS3TIMEINFO_BIT:
                {
                    return extractor.extract(gnss3.gnss3TimeInfo);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3DOP_BIT)
                case GNSS3_GNSS3DOP_BIT:
                {
                    return extractor.extract(gnss3.gnss3Dop);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3SATINFO_BIT)
                case GNSS3_GNSS3SATINFO_BIT:
                {
                    return extractor.extract(gnss3.gnss3SatInfo);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3RAWMEAS_BIT)
                case GNSS3_GNSS3RAWMEAS_BIT:
                {
                    return extractor.extract(gnss3.gnss3RawMeas);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3STATUS_BIT)
                case GNSS3_GNSS3STATUS_BIT:
                {
                    return extractor.extract(gnss3.gnss3Status);
                }
#endif

#if (GNSS3_GROUP_ENABLE & GNSS3_GNSS3ALTMSL_BIT)
                case GNSS3_GNSS3ALTMSL_BIT:
                {
                    return extractor.extract(gnss3.gnss3AltMsl);
                }
#endif

                default:
                {
                    return true;
                }
            }  // switch (1 << measTypeIndex)
            break;
        }  // case 12:

        default:
        {
            return true;
        }
    }  // switch (measGroupIndex)
    return true;
}  // CompositeData::copyFromBuffer
}  // namespace VN

#endif  // VN_COMPOSITEDATA_HPP_
