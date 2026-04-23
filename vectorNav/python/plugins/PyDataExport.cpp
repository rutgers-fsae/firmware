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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <cstdint>
#include <memory>
#include <sstream>

#include "vectornav/Exporter.hpp"
#include "vectornav/ExporterAscii.hpp"
#include "vectornav/ExporterCsv.hpp"
#include "vectornav/ExporterRinex.hpp"
#include "vectornav/ExporterSkippedByte.hpp"
#include "vectornav/HAL/File.hpp"

namespace py = pybind11;

namespace VN
{

void init_data_export(py::module& m)
{
    py::module Plugins = m.def_submodule("Plugins", "Plugins Module");
    py::module DataExport = Plugins.def_submodule("DataExport", "DataExport Module");

    py::class_<VN::DataExport::Exporter>(DataExport, "Exporter")
        .def("getQueuePtr", &VN::DataExport::Exporter::getQueuePtr, py::return_value_policy::reference)
        .def("start", &VN::DataExport::Exporter::start)
        .def("stop", &VN::DataExport::Exporter::stop)
        .def("isLogging", &VN::DataExport::Exporter::isLogging);

    py::class_<VN::DataExport::ExporterCsv, VN::DataExport::Exporter>(DataExport, "ExporterCsv")
        .def(py::init<const Filesystem::FilePath&, VN::DataExport::Exporter::PacketQueueMode, bool>(), 
            py::arg("outputDir"), py::arg("mode") = VN::DataExport::Exporter::PacketQueueMode::Force, py::arg("enableSystemTimeStamps") = false)
        .def("exportToFile", &VN::DataExport::ExporterCsv::exportToFile);

    py::class_<VN::DataExport::ExporterRinex, VN::DataExport::Exporter>(DataExport, "ExporterRinex")
        .def(py::init<const Filesystem::FilePath&, const uint32_t>())
        .def("exportToFile", &VN::DataExport::ExporterRinex::exportToFile);

    py::class_<VN::DataExport::ExporterAscii, VN::DataExport::Exporter>(DataExport, "ExporterAscii")
        .def(py::init<const Filesystem::FilePath&, VN::DataExport::Exporter::PacketQueueMode, bool>(), 
            py::arg("outputDir"), py::arg("mode") = VN::DataExport::Exporter::PacketQueueMode::Force, py::arg("enableSystemTimeStamps") = false)
        .def("exportToFile", &VN::DataExport::ExporterAscii::exportToFile);

    py::class_<VN::DataExport::ExporterSkippedByte, VN::DataExport::Exporter>(DataExport, "ExporterSkippedByte")
        .def(py::init<const Filesystem::FilePath&, VN::DataExport::Exporter::PacketQueueMode>(),
            py::arg("outputDir"), py::arg("mode") = VN::DataExport::Exporter::PacketQueueMode::Force)
        .def("exportToFile", &VN::DataExport::ExporterSkippedByte::exportToFile);

    py::enum_<VN::DataExport::Exporter::PacketQueueMode>(DataExport, "PacketQueueMode", py::module_local())
        .value("Force", VN::DataExport::Exporter::PacketQueueMode::Force)
        .value("Try", VN::DataExport::Exporter::PacketQueueMode::Try)
#if THREADING_ENABLE
        .value("Retry", VN::DataExport::Exporter::PacketQueueMode::Retry)
#endif  
    ;

    py::implicitly_convertible<py::str, Filesystem::FilePath>();
}

}  // namespace VN
