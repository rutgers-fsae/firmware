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

#include "PyTemplates.hpp"
#include "vectornav/AttitudeMath.hpp"
#include "vectornav/Conversions.hpp"
#include "vectornav/EigenDecomposition.hpp"
#include "vectornav/LinearAlgebra.hpp"
#include "vectornav/LinearLeastSquares.hpp"
#include "vectornav/PosVelMath.hpp"
#include "vectornav/TemplateLibrary/Matrix.hpp"

namespace py = pybind11;

namespace VN {
using namespace Math;

void init_math(py::module& m) {
  py::module Plugins = m.def_submodule("Plugins", "Plugins Module");
  py::module Math = Plugins.def_submodule("Math", "Math Plugin");

  // ----------------
  // AttitudeMath
  // ----------------
  Math.def("normalizeQuat", [](const Quat& quat) -> std::optional<Quat> { return normalizeQuat(quat); }, py::arg("quat"));

  Math.def("ypr2quat", &ypr2quat, py::arg("ypr"));

  Math.def("ypr2dcm", &ypr2dcm, py::arg("ypr"));

  Math.def("quat2dcm", &quat2dcm, py::arg("quat"));

  Math.def("dcm2ypr", &dcm2ypr, py::arg("dcm"));

  Math.def("quat2ypr", &quat2ypr, py::arg("quat"));

  Math.def("dcm2quat", &dcm2quat, py::arg("dcm"));

  Math.def("multiplyQuat", &multiplyQuat, py::arg("q0"), py::arg("q1"));

  Math.def("inverseQuat", &inverseQuat, py::arg("quat"));

  Math.def("propagateQuat", &propagateQuat, py::arg("q0"), py::arg("dTheta"));

  Math.def("mag2yaw", &mag2yaw, py::arg("magBody"), py::arg("pitchAndRoll"), py::arg("declination") = 0.0f);

  Math.def("accel2pitchroll", [](const Vec3f& accelBody) -> std::optional<Vec2f> { return accel2pitchroll(accelBody); }, py::arg("accelBody"));
  
  Math.def("imu2ypr", [](const Vec3f& magBody, const Vec3f& accelBody, float declination) -> std::optional<Ypr> { return imu2ypr(magBody, accelBody, declination); }, py::arg("magBody"), py::arg("accelBody"), py::arg("declination") = 0.0f);

  Math.def("crp2quat", &crp2quat, py::arg("crp"));
  
  Math.def("quat2crp", [](const Quat& quat) -> std::optional<Vec3f> { return quat2crp(quat); }, py::arg("quat"));

  Math.def("crp2dcm", &crp2dcm, py::arg("crp"));

  Math.def("dcm2crp", [](const Mat3f& dcm) -> std::optional<Vec3f> { return dcm2crp(dcm); }, py::arg("dcm"));

  // ----------------
  // Conversions
  // ----------------
  Math.def("rad2deg", static_cast<double(*)(double)>(&rad2deg), py::arg("angleInRads"));
  Math.def("rad2deg", [](const Vec2f& vec) { return rad2deg(vec); }, py::arg("vec"));
  Math.def("rad2deg", [](const Vec3f& vec) { return rad2deg(vec); }, py::arg("vec"));
  Math.def("rad2deg", [](const Vec3d& vec) { return rad2deg(vec); }, py::arg("vec"));
  Math.def("rad2deg", [](const Vec4f& vec) { return rad2deg(vec); }, py::arg("vec"));
  Math.def("rad2deg", [](const Mat3f& mat) { return rad2deg(mat); }, py::arg("mat"));
  Math.def("rad2deg", [](const Ypr& ypr) { return rad2deg(ypr); }, py::arg("ypr"));

  Math.def("deg2rad", static_cast<double(*)(double)>(&deg2rad), py::arg("angleInDegs"));
  Math.def("deg2rad", [](const Vec2f& vec) { return deg2rad(vec); }, py::arg("vec"));
  Math.def("deg2rad", [](const Vec3f& vec) { return deg2rad(vec); }, py::arg("vec"));
  Math.def("deg2rad", [](const Vec3d& vec) { return deg2rad(vec); }, py::arg("vec"));
  Math.def("deg2rad", [](const Vec4f& vec) { return deg2rad(vec); }, py::arg("vec"));
  Math.def("deg2rad", [](const Mat3f& mat) { return deg2rad(mat); }, py::arg("mat"));
  Math.def("deg2rad", [](const Ypr& ypr) { return deg2rad(ypr); }, py::arg("ypr"));

  Math.def("celsius2fahren", py::overload_cast<double>(&celsius2fahren), py::arg("tempInCelsius"));

  Math.def("fahren2celsius", py::overload_cast<double>(&fahren2celsius), py::arg("tempInFahren"));

  Math.def("celsius2kelvin", py::overload_cast<double>(&celsius2kelvin), py::arg("tempInCelsius"));

  Math.def("kelvin2celsius", py::overload_cast<double>(&kelvin2celsius), py::arg("tempInKelvin"));

  Math.def("fahren2kelvin", py::overload_cast<double>(&fahren2kelvin), py::arg("tempInFahren"));

  Math.def("kelvin2fahren", py::overload_cast<double>(&kelvin2fahren), py::arg("tempInKelvin"));

  // ----------------
  // LinearAlgebra
  // ----------------
  Math.def("dot", [](const Vec3f& lhs, const Vec3f& rhs) { return dot(lhs, rhs); }, py::arg("lhs"), py::arg("rhs"));
  Math.def("dot", [](const Vec3d& lhs, const Vec3d& rhs) { return dot(lhs, rhs); }, py::arg("lhs"), py::arg("rhs"));
  Math.def("dot", [](const Vec4f& lhs, const Vec4f& rhs) { return dot(lhs, rhs); }, py::arg("lhs"), py::arg("rhs"));

  Math.def("cross", [](const Vec3f& lhs, const Vec3f& rhs) { return cross(lhs, rhs); }, py::arg("lhs"), py::arg("rhs"));

  Math.def("inverse", [](const Mat3f& mat) { return inverse(mat); }, py::arg("mat"));

  Math.def("transpose", [](const Mat3f& mat) { return transpose(mat); }, py::arg("mat"));

  Math.def("sign", [](float val) { return sign(val); }, py::arg("val"));
  Math.def("sign", [](int val) { return sign(val); }, py::arg("val"));

  Math.def("skew", [](const Vec3f& vec) { return skew(vec); }, py::arg("vec"));

  Math.def("norm", [](const Vec2f& vec) { return norm(vec); }, py::arg("vec"));
  Math.def("norm", [](const Vec3f& vec) { return norm(vec); }, py::arg("vec"));
  Math.def("norm", [](const Vec4f& vec) { return norm(vec); }, py::arg("vec"));
  Math.def("norm", [](const Mat3f& mat) { return norm(mat); }, py::arg("mat"));

  Math.def("maxi", [](const Vec2f& vec) { return maxi(vec); }, py::arg("vec"));
  Math.def("maxi", [](const Vec3f& vec) { return maxi(vec); }, py::arg("vec"));
  Math.def("maxi", [](const Vec4f& vec) { return maxi(vec); }, py::arg("vec"));

  Math.def("mini", [](const Vec2f& vec) { return mini(vec); }, py::arg("vec"));
  Math.def("mini", [](const Vec3f& vec) { return mini(vec); }, py::arg("vec"));
  Math.def("mini", [](const Vec4f& vec) { return mini(vec); }, py::arg("vec"));

  Math.def("condition", [](const Mat3f& A, const Mat3f& Ainv) { return condition(A, Ainv); }, py::arg("A"), py::arg("Ainv"));

  Math.def("normalize", [](Vec2f vec) -> std::optional<Vec2f> { return normalize(vec); }, py::arg("vec"));
  Math.def("normalize", [](Vec3f vec) -> std::optional<Vec3f> { return normalize(vec); }, py::arg("vec"));
  Math.def("normalize", [](Vec4f vec) -> std::optional<Vec4f> { return normalize(vec); }, py::arg("vec"));

  Math.def("outer", [](const Vec3f& lhs) { return outer(lhs); }, py::arg("lhs"));
  Math.def("outer", [](const Vec3f& vec1, const Vec3f& vec2) { return outer(vec1, vec2); }, py::arg("vec1"), py::arg("vec2"));

  // ----------------
  // PosVelMath
  // ----------------
  Math.def("course_over_ground", py::overload_cast<float, float>(&course_over_ground), py::arg("velNedX"), py::arg("velNedY"));
  Math.def("course_over_ground", py::overload_cast<const Vec3f&>(&course_over_ground), py::arg("velNed"));

  Math.def("speed_over_ground", py::overload_cast<float, float>(&speed_over_ground), py::arg("velNedX"), py::arg("velNedY"));
  Math.def("speed_over_ground", py::overload_cast<const Vec3f&>(&speed_over_ground), py::arg("velNed"));

  Math.def("ecef2lla", &ecef2lla, py::arg("ecef"));
  Math.def("lla2ecef", &lla2ecef, py::arg("lla"));

  Plugins.attr("Math") = Math;
}
} // namespace VN
// clang-format on
