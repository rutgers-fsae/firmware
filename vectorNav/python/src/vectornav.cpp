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

#include "vectornav/Config.hpp"

namespace py = pybind11;

namespace VN {

void init_registers(py::module& m);
void init_sensor(py::module& m);
void init_composite_data(py::module& m);
void init_commands(py::module& m);
void init_vntypes(py::module& m);
void init_errors(py::module& m);
void init_packets(py::module& m);
void init_byte_buffer(py::module& m);
void init_utils(py::module& m);
  
// PLUGIN INIT FUNCTIONS
void init_register_scan(py::module& m);
void init_firmware_programmer(py::module& m);
void init_logger(py::module& m);
void init_data_export(py::module& m);
void init_calibration(py::module& m);
void init_math(py::module& m);

PYBIND11_MODULE(vectornav, m) {
  m.doc() = "VectorNav Python SDK";

  init_registers(m);
  init_composite_data(m);
  init_commands(m);
  init_vntypes(m);
  init_errors(m);
  init_packets(m);
  init_byte_buffer(m);
  init_utils(m);
  init_sensor(m);
  
#ifdef __REGSCAN__
  init_register_scan(m);
#endif
    
#ifdef __FIRMWARE_PROGRAMMER__
  init_firmware_programmer(m);
#endif

#ifdef __LOGGER__
  init_logger(m);
#endif

#ifdef __DATAEXPORT__
  init_data_export(m);
#endif

#ifdef __CALIBRATION__
  init_calibration(m);
#endif

#ifdef __MATH__
  init_math(m);
#endif
}
}
// clang-format on
