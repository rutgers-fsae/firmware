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

#ifndef VN_CLI_MATH_HPP_
#define VN_CLI_MATH_HPP_

#include "vectornav/CLI_MeasurementDataTypes.hpp"
#include "vectornav/CLI_Matrix.hpp"

#pragma managed(push, off)
#include "vectornav/AttitudeMath.hpp"
#include "vectornav/Conversions.hpp"
#include "vectornav/LinearAlgebra.hpp"
#include "vectornav/PosVelMath.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#pragma managed(pop)

namespace VNSDK 
{

public ref class Math
{
    public:
    
    // ******
    // AttitudeMath
    // ******
    
    // NormalizeQuat
    static Nullable<Quaternion> NormalizeQuat(Quaternion quat)
    {
        std::optional<VN::Quat> nativeQuatOut = VN::Math::normalizeQuat(quat.GetNative());
        if (nativeQuatOut.has_value())
            return Quaternion(nativeQuatOut.value());
        return Nullable<Quaternion>();
    }
    
    // Ypr2Quat
    static Quaternion Ypr2Quat(Ypr ypr)
    {
        return Quaternion(VN::Math::ypr2quat(ypr.GetNative()));
    }
    
    // Ypr2Dcm
    static Mat3f Ypr2Dcm(Ypr ypr)
    {
        return Mat3f(VN::Math::ypr2dcm(ypr.GetNative()));
    }
    
    // Quat2Dcm
    static Mat3f Quat2Dcm(Quaternion quat)
    {
        return Mat3f(VN::Math::quat2dcm(quat.GetNative()));
    }

    // Dcm2Ypr
    static Ypr Dcm2Ypr(Mat3f dcm)
    {
        return Ypr(VN::Math::dcm2ypr(dcm.GetNative()));
    }        
    // Quat2Ypr
    static Ypr Quat2Ypr(Quaternion quat)
    {
        return Ypr(VN::Math::quat2ypr(quat.GetNative()));
    }

    // Dcm2Quat
    static Quaternion Dcm2Quat(Mat3f dcm)
    {
        return Quaternion(VN::Math::dcm2quat(dcm.GetNative()));
    }
    
    // MultiplyQuat
    static Quaternion MultiplyQuat(Quaternion q0, Quaternion q1)
    {
        return Quaternion(VN::Math::multiplyQuat(q0.GetNative(), q1.GetNative()));
    }

    // InverseQuat
    static Quaternion InverseQuat(Quaternion quat)
    {
        return Quaternion(VN::Math::inverseQuat(quat.GetNative()));
    }
    
    // PropagateQuat
    static Quaternion PropagateQuat(Quaternion q0, Vec3f dTheta)
    {
        return Quaternion(VN::Math::propagateQuat(q0.GetNative(), dTheta.GetNative()));
    }
    
    // Mag2Yaw
    static float Mag2Yaw(Vec3f magBody, Vec2f pitchAndRoll)
    {
        return VN::Math::mag2yaw(magBody.GetNative(), pitchAndRoll.GetNative(), 0.0f);
    }

    static float Mag2Yaw(Vec3f magBody, Vec2f pitchAndRoll, float declination)
    {
        return VN::Math::mag2yaw(magBody.GetNative(), pitchAndRoll.GetNative(), declination);
    }
    
    // Accel2PitchRoll
    static Nullable<Vec2f> Accel2PitchRoll(Vec3f accelBody)
    {
        std::optional<VN::Vec2f> nativeVec = VN::Math::accel2pitchroll(accelBody.GetNative());
        if (nativeVec.has_value())
            return Vec2f(nativeVec.value());
        return Nullable<Vec2f>();
    }
    
    // Imu2Ypr
    static Nullable<Ypr> Imu2Ypr(Vec3f magBody, Vec3f accelBody)
    {
        std::optional<VN::Ypr> nativeYpr = VN::Math::imu2ypr(magBody.GetNative(), accelBody.GetNative());
        if (nativeYpr.has_value())
            return Ypr(nativeYpr.value());
        return Nullable<Ypr>();
    }
    
    static Nullable<Ypr> Imu2Ypr(Vec3f magBody, Vec3f accelBody, float declination)
    {
        std::optional<VN::Ypr> nativeYpr = VN::Math::imu2ypr(magBody.GetNative(), accelBody.GetNative(), declination);
        if (nativeYpr.has_value())
            return Ypr(nativeYpr.value());
        return Nullable<Ypr>();
    }

    // Crp2Quat
    static Quaternion Crp2Quat(Vec3f crp)
    {
        return Quaternion(VN::Math::crp2quat(crp.GetNative()));
    }

    // Quat2Crp
    static Nullable<Vec3f> Quat2Crp(Quaternion quat)
    {
        std::optional<VN::Vec3f> nativeVec = VN::Math::quat2crp(quat.GetNative());
        if (nativeVec.has_value())
            return Vec3f(nativeVec.value());
        return Nullable<Vec3f>();
    }
    
    // Crp2Dcm
    static Mat3f Crp2Dcm(Vec3f crp)
    {
        return Mat3f(VN::Math::crp2dcm(crp.GetNative()));
    } 

    // Dcm2Crp
    static Nullable<Vec3f> Dcm2Crp(Mat3f dcm)
    {
        std::optional<VN::Vec3f> nativeVec = VN::Math::dcm2crp(dcm.GetNative());
        if (nativeVec.has_value())
            return Vec3f(nativeVec.value());
        return Nullable<Vec3f>();
    }
    
    // ******
    // Conversions
    // ******

    // Rad2Deg
    static float Rad2Deg(float angleInRads)
    {
        return VN::Math::rad2deg(angleInRads);
    }

    static double Rad2Deg(double angleInRads)
    {
        return VN::Math::rad2deg(angleInRads);
    }

    static Vec2f Rad2Deg(Vec2f vec)
    {
        return Vec2f(VN::Math::rad2deg(vec.GetNative()));
    }

    static Vec3f Rad2Deg(Vec3f vec)
    {
        return Vec3f(VN::Math::rad2deg(vec.GetNative()));
    }

    static Vec4f Rad2Deg(Vec4f vec)
    {
        return Vec4f(VN::Math::rad2deg(vec.GetNative()));
    }

    static Vec8f Rad2Deg(Vec8f vec)
    {
        return Vec8f(VN::Math::rad2deg(vec.GetNative()));
    }

    static Vec3d Rad2Deg(Vec3d vec)
    {
        return Vec3d(VN::Math::rad2deg(vec.GetNative()));
    }

    static Mat3f Rad2Deg(Mat3f mat)
    {
        return Mat3f(VN::Math::rad2deg(mat.GetNative()));
    }

    static Ypr Rad2Deg(Ypr ypr)
    {
        return Ypr(VN::Math::rad2deg(ypr.GetNative()));
    }
    
    // Deg2Rad
    static float Deg2Rad(float angleInRads)
    {
        return VN::Math::deg2rad(angleInRads);
    }

    static double Deg2Rad(double angleInRads)
    {
        return VN::Math::deg2rad(angleInRads);
    }

    static Vec2f Deg2Rad(Vec2f vec)
    {
        return Vec2f(VN::Math::deg2rad(vec.GetNative()));
    }
    
    static Vec3f Deg2Rad(Vec3f vec)
    {
        return Vec3f(VN::Math::deg2rad(vec.GetNative()));
    }

    static Vec4f Deg2Rad(Vec4f vec)
    {
        return Vec4f(VN::Math::deg2rad(vec.GetNative()));
    }

    static Vec8f Deg2Rad(Vec8f vec)
    {
        return Vec8f(VN::Math::deg2rad(vec.GetNative()));
    }

    static Vec3d Deg2Rad(Vec3d vec)
    {
        return Vec3d(VN::Math::deg2rad(vec.GetNative()));
    }

    static Mat3f Deg2Rad(Mat3f mat)
    {
        return Mat3f(VN::Math::deg2rad(mat.GetNative()));
    }

    static Ypr Deg2Rad(Ypr ypr)
    {
        return Ypr(VN::Math::deg2rad(ypr.GetNative()));
    }


    // Celsius2Fahrenheit
    static float Celsius2Fahrenheit(float tempInCelsius) 
    { 
        return VN::Math::celsius2fahren(tempInCelsius); 
    }

    static double Celsius2Fahrenheit(double tempInCelsius) 
    { 
        return VN::Math::celsius2fahren(tempInCelsius); 
    }

    // Fahrenheit2Celsius
    static float Fahrenheit2Celsius(float tempInFahren) 
    { 
        return VN::Math::fahren2celsius(tempInFahren); 
    }

    static double Fahrenheit2Celsius(double tempInFahren) 
    { 
        return VN::Math::fahren2celsius(tempInFahren); 
    }

    // Celsius2Kelvin
    static float Celsius2Kelvin(float tempInCelsius) 
    { 
        return VN::Math::celsius2kelvin(tempInCelsius); 
    }

    static double Celsius2Kelvin(double tempInCelsius) 
    { 
        return VN::Math::celsius2kelvin(tempInCelsius); 
    }

    // Kelvin2Celsius
    static float Kelvin2Celsius(float tempInKelvin) 
    { 
        return VN::Math::kelvin2celsius(tempInKelvin); 
    }

    static double Kelvin2Celsius(double tempInKelvin) 
    { 
        return VN::Math::kelvin2celsius(tempInKelvin); 
    }

    // Fahrenheit2Kelvin
    static float Fahrenheit2Kelvin(float tempInFahren) 
    { 
        return VN::Math::fahren2kelvin(tempInFahren); 
    }

    static double Fahrenheit2Kelvin(double tempInFahren) 
    { 
        return VN::Math::fahren2kelvin(tempInFahren); 
    }

    // Kelvin2Fahrenheit
    static float Kelvin2Fahrenheit(float tempInKelvin) 
    { 
        return VN::Math::kelvin2fahren(tempInKelvin); 
    }

    static double Kelvin2Fahrenheit(double tempInKelvin) 
    { 
        return VN::Math::kelvin2fahren(tempInKelvin); 
    }

    // ******
    // LinearAlgebra
    // ******

    // Dot Product
    
    static float Dot(Vec2f lhs, Vec2f rhs)
    {
        return VN::Math::dot(lhs.GetNative(), rhs.GetNative());
    }
    
    static float Dot(Vec3f lhs, Vec3f rhs)
    {
        return VN::Math::dot(lhs.GetNative(), rhs.GetNative());
    }
    
    static float Dot(Vec4f lhs, Vec4f rhs)
    {
        return VN::Math::dot(lhs.GetNative(), rhs.GetNative());
    }
    
    static float Dot(Vec8f lhs, Vec8f rhs)
    {
        return VN::Math::dot(lhs.GetNative(), rhs.GetNative());
    }
    
    static double Dot(Vec3d lhs, Vec3d rhs)
    {
        return VN::Math::dot(lhs.GetNative(), rhs.GetNative());
    }    

    // Cross Product
    static Vec3f Cross(Vec3f lhs, Vec3f rhs)
    {
        return Vec3f(VN::Math::cross(lhs.GetNative(), rhs.GetNative()));
    }

    // Inverse
    static Mat3f Inverse(Mat3f mat)
    {
        return Mat3f(VN::Math::inverse(mat.GetNative()));
    }

    // Transpose
    static Mat3f Transpose(Mat3f mat)
    {
        return Mat3f(VN::Math::transpose(mat.GetNative()));
    }

    // Sign
    static int Sign(int val)
    {
        return VN::Math::sign(val);
    }

    static float Sign(float val)
    {
        return VN::Math::sign(val);
    }
    
    static double Sign(double val)
    {
        return VN::Math::sign(val);
    }

    // Skew
    static Mat3f Skew(Vec3f vec)
    {
        return Mat3f(VN::Math::skew(vec.GetNative()));
    }
    
    // Norm
    static float Norm(Vec2f vec)
    {
        return VN::Math::norm(vec.GetNative());
    }
    
    static float Norm(Vec3f vec)
    {
        return VN::Math::norm(vec.GetNative());
    }

    static float Norm(Vec4f vec)
    {
        return VN::Math::norm(vec.GetNative());
    }

    static float Norm(Vec8f vec)
    {
        return VN::Math::norm(vec.GetNative());
    }

    static double Norm(Vec3d vec)
    {
        return VN::Math::norm(vec.GetNative());
    }

    static float Norm(Mat3f mat)
    {
        return VN::Math::norm(mat.GetNative());
    }

    // Maxi
    static uint16_t Maxi(Vec2f vec)
    {
        return VN::Math::maxi(vec.GetNative());
    }
    
    static uint16_t Maxi(Vec3f vec)
    {
        return VN::Math::maxi(vec.GetNative());
    }

    static uint16_t Maxi(Vec4f vec)
    {
        return VN::Math::maxi(vec.GetNative());
    }

    static uint16_t Maxi(Vec8f vec)
    {
        return VN::Math::maxi(vec.GetNative());
    }

    static uint16_t Maxi(Vec3d vec)
    {
        return VN::Math::maxi(vec.GetNative());
    }
    
    // Mini 
    static uint16_t Mini(Vec2f vec)
    {
        return VN::Math::mini(vec.GetNative());
   }
    
    static uint16_t Mini(Vec3f vec)
    {
        return VN::Math::mini(vec.GetNative());
    }

    static uint16_t Mini(Vec4f vec)
    {
        return VN::Math::mini(vec.GetNative());
    }

    static uint16_t Mini(Vec8f vec)
    {
        return VN::Math::mini(vec.GetNative());
    }

    static uint16_t Mini(Vec3d vec)
    {
        return VN::Math::mini(vec.GetNative());
    }

    // Condition
    static float Condition(Mat3f A, Mat3f Ainv)
    {
        return VN::Math::condition(A.GetNative(), Ainv.GetNative());        
    }

    // Normalize
    static Nullable<Vec2f> Normalize(Vec2f vec)
    {
        std::optional<VN::Vec2f> nativeVecOut = VN::Math::normalize(vec.GetNative());
        if (nativeVecOut.has_value())
            return Vec2f(nativeVecOut.value());
        return Nullable<Vec2f>();
    }

    static Nullable<Vec3f> Normalize(Vec3f vec)
    {
        std::optional<VN::Vec3f> nativeVecOut = VN::Math::normalize(vec.GetNative());
        if (nativeVecOut.has_value())
            return Vec3f(nativeVecOut.value());
        return Nullable<Vec3f>();
    }
    
    static Nullable<Vec4f> Normalize(Vec4f vec)
    {
        std::optional<VN::Vec4f> nativeVecOut = VN::Math::normalize(vec.GetNative());
        if (nativeVecOut.has_value())
            return Vec4f(nativeVecOut.value());
        return Nullable<Vec4f>();
    }
    
    static Nullable<Vec8f> Normalize(Vec8f vec)
    {
        std::optional<VN::Matrix<8, 1, float>> nativeVecOut = VN::Math::normalize(vec.GetNative());
        if (nativeVecOut.has_value())
            return Vec8f(nativeVecOut.value());
        return Nullable<Vec8f>();
    }
    
    static Nullable<Vec3d> Normalize(Vec3d vec)
    {
        std::optional<VN::Vec3d> nativeVecOut = VN::Math::normalize(vec.GetNative());
        if (nativeVecOut.has_value())
            return Vec3d(nativeVecOut.value());
        return Nullable<Vec3d>();
    }

    // Outer
    static Mat3f Outer(Vec3f vec)
    {
        return Mat3f(VN::Math::outer(vec.GetNative()));
    }
    
    static Mat3f Outer(Vec3f vec1, Vec3f vec2)
    {
        return Mat3f(VN::Math::outer(vec1.GetNative(), vec2.GetNative()));
    }
    
    // ******
    // PosVel Conversions
    // ******
    
    // CourseOverGround
    static float CourseOverGround(float velNedX, float velNedY)
    {
        return VN::Math::course_over_ground(velNedX, velNedY);
    }
    
    static float CourseOverGround(Vec3f velNed)
    {
        return VN::Math::course_over_ground(velNed.GetNative());
    }

    // SpeedOverGround
    static float SpeedOverGround(const float velNedX, const float velNedY)
    {
        return VN::Math::speed_over_ground(velNedX, velNedY);
    }

    static float SpeedOverGround(Vec3f velNed)
    {
        return VN::Math::speed_over_ground(velNed.GetNative());
    }

    // Ecef2Lla
    static Lla Ecef2Lla(Vec3d ecef)
    {
        return Lla(VN::Math::ecef2lla(ecef.GetNative()));
    }
    
    // Lla2Ecef
    static Vec3d Lla2Ecef(Lla lla)
    {
        return Vec3d(VN::Math::lla2ecef(lla.GetNative()));
    }
    
};  // public ref class Math
}  // namespace VNSDK
#endif  // VN_CLI_MATH_HPP_
// clang-format on
