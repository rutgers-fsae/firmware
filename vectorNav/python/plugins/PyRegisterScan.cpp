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

#include "RegisterScan/include/vectornav/RegisterScan.hpp"
#include "vectornav/Interface/BridgeSensor.hpp"


#include "PyErrors.hpp"
#include "PyTemplates.hpp"

namespace py = pybind11;
namespace VN {
  
void init_register_scan(py::module& m) {

  py::module Plugins = m.def_submodule("Plugins", "Plugins Module");
  py::module RegisterScan = Plugins.def_submodule("RegisterScan", "RegisterScan Module");

  py::class_<VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>>(RegisterScan, "ConfigWriter_Ascii")
    .def("writeConfig", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>::writeConfig)
    .def("close", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>::close);

  py::class_<VN::RegisterScan::AsciiConfigWriter, VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>>(RegisterScan, "AsciiConfigWriter")
    .def(py::init<Filesystem::FilePath&>())
    .def("write", &VN::RegisterScan::AsciiConfigWriter::write)
    .def("finalize", &VN::RegisterScan::AsciiConfigWriter::finalize);  

  py::class_<VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>>(RegisterScan, "ConfigWriter_Generic")
    .def("writeConfig", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>::writeConfig)
    .def("close", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>::close);

  py::class_<VN::RegisterScan::GenericConfigWriter, VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>>(RegisterScan, "GenericConfigWriter")
    .def(py::init<std::function<VN::Error(const AsciiMessage&)>, std::function<void()>>(), 
      py::arg("writeConfig"), py::arg("finalizeConfig") = std::function<void()>([]() {}))  
    .def("write", &VN::RegisterScan::GenericConfigWriter::write)  
    .def("finalize", &VN::RegisterScan::GenericConfigWriter::finalize); 
          
  py::class_<VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>>(RegisterScan, "ConfigWriter_Xml")
    .def("writeConfig", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>::writeConfig)
    .def("close", &VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>::close);

  py::class_<VN::RegisterScan::XmlConfigWriter, VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>>(RegisterScan, "XmlConfigWriter")
    .def(py::init<BridgeSensor&, Filesystem::FilePath>())
    .def("write", &VN::RegisterScan::XmlConfigWriter::write)
    .def("finalize", &VN::RegisterScan::XmlConfigWriter::finalize);

  py::enum_<VN::RegisterScan::SaveConfigurationFilter::Type>(RegisterScan, "SaveConfigFilterType", py::arithmetic())
    .value("Include", VN::RegisterScan::SaveConfigurationFilter::Type::Include)
    .value("Exclude", VN::RegisterScan::SaveConfigurationFilter::Type::Exclude);

  py::class_<VN::RegisterScan::SaveConfigurationFilter>(RegisterScan, "SaveConfigurationFilter")
    .def(py::init<>())
    .def_readwrite("type", &VN::RegisterScan::SaveConfigurationFilter::type)
    .def_readwrite("list", &VN::RegisterScan::SaveConfigurationFilter::list);

    // saveConfiguration
    RegisterScan.def("saveConfiguration", [] (BridgeSensor& sensor,
            VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>& writer,
            VN::RegisterScan::SaveConfigurationFilter filter) {
            VN::Error error = VN::RegisterScan::saveConfiguration<VN::RegisterScan::AsciiConfigWriter>(sensor, writer, filter);
            if (error != VN::Error::None) { throwError(error); } },
        py::arg("sensor"),
        py::arg("configWriter"),
        py::arg("filter") = VN::RegisterScan::SaveConfigurationFilter{ VN::RegisterScan::SaveConfigurationFilter::Type::Include,
            VN::RegisterScan::getDefaultConfigRegisters() },
        "Save config to an Ascii file" );

    RegisterScan.def("saveConfiguration", [] (BridgeSensor& sensor,
            VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>& writer,
            VN::RegisterScan::SaveConfigurationFilter filter) {
            VN::Error error = VN::RegisterScan::saveConfiguration<VN::RegisterScan::XmlConfigWriter>(sensor, writer, filter);
            if (error != VN::Error::None) { throwError(error); } },
        py::arg("sensor"),
        py::arg("configWriter"),
        py::arg("filter") = VN::RegisterScan::SaveConfigurationFilter{ VN::RegisterScan::SaveConfigurationFilter::Type::Include,
            VN::RegisterScan::getDefaultConfigRegisters() },
        "Save config to an Xml file" );

    RegisterScan.def("saveConfiguration", [] (BridgeSensor& sensor,
            VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>& writer,
            VN::RegisterScan::SaveConfigurationFilter filter) {
            VN::Error error = VN::RegisterScan::saveConfiguration<VN::RegisterScan::GenericConfigWriter>(sensor, writer, filter);
            if (error != VN::Error::None) { throwError(error); } },
        py::arg("sensor"),
        py::arg("configWriter"),
        py::arg("filter") = VN::RegisterScan::SaveConfigurationFilter{ VN::RegisterScan::SaveConfigurationFilter::Type::Include,
            VN::RegisterScan::getDefaultConfigRegisters() },
        "Save config generically" );

    // saveNonDefaultConfiguration
    RegisterScan.def("saveNonDefaultConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigWriter<VN::RegisterScan::AsciiConfigWriter>& writer) {
            VN::Error error = VN::RegisterScan::saveNonDefaultConfiguration<VN::RegisterScan::AsciiConfigWriter>(sensor, writer);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("saveNonDefaultConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigWriter<VN::RegisterScan::XmlConfigWriter>& writer) {
            VN::Error error = VN::RegisterScan::saveNonDefaultConfiguration<VN::RegisterScan::XmlConfigWriter>(sensor, writer);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("saveNonDefaultConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigWriter<VN::RegisterScan::GenericConfigWriter>& writer) {
            VN::Error error = VN::RegisterScan::saveNonDefaultConfiguration<VN::RegisterScan::GenericConfigWriter>(sensor, writer);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("getDefaultConfigRegisters", &VN::RegisterScan::getDefaultConfigRegisters);

    py::class_<VN::RegisterScan::ConfigReader<VN::RegisterScan::AsciiConfigReader>>(RegisterScan, "ConfigReader_Ascii")
        .def("getNextConfig", &VN::RegisterScan::ConfigReader<VN::RegisterScan::AsciiConfigReader>::getNextConfig);

    py::class_<VN::RegisterScan::AsciiConfigReader, VN::RegisterScan::ConfigReader<VN::RegisterScan::AsciiConfigReader>>(RegisterScan, "AsciiConfigReader")
        .def(py::init<const Filesystem::FilePath&>())
        .def("next", &VN::RegisterScan::AsciiConfigReader::next);

    py::class_<VN::RegisterScan::ConfigReader<VN::RegisterScan::XmlConfigReader>>(RegisterScan, "ConfigReader_Xml")
        .def("getNextConfig", &VN::RegisterScan::ConfigReader<VN::RegisterScan::XmlConfigReader>::getNextConfig);

    py::class_<VN::RegisterScan::XmlConfigReader, VN::RegisterScan::ConfigReader<VN::RegisterScan::XmlConfigReader>>(RegisterScan, "XmlConfigReader")
        .def(py::init<const Filesystem::FilePath&>())
        .def("next", &VN::RegisterScan::XmlConfigReader::next);

    py::class_<VN::RegisterScan::ConfigReader<VN::RegisterScan::GenericConfigReader>>(RegisterScan, "ConfigReader_Generic")
        .def("getNextConfig", &VN::RegisterScan::ConfigReader<VN::RegisterScan::GenericConfigReader>::getNextConfig);

    py::class_<VN::RegisterScan::GenericConfigReader, VN::RegisterScan::ConfigReader<VN::RegisterScan::GenericConfigReader>>(RegisterScan, "GenericConfigReader")
        .def(py::init<std::function<VN::Error(AsciiMessage&)>>())
        .def("next", &VN::RegisterScan::GenericConfigReader::next);

    // setConfigurationRegisters
    RegisterScan.def("setConfigurationRegisters", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::AsciiConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::setConfigurationRegisters<VN::RegisterScan::AsciiConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});
      
    RegisterScan.def("setConfigurationRegisters", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::XmlConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::setConfigurationRegisters<VN::RegisterScan::XmlConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("setConfigurationRegisters", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::GenericConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::setConfigurationRegisters<VN::RegisterScan::GenericConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});

    // loadConfiguration
    RegisterScan.def("loadConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::AsciiConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::loadConfiguration<VN::RegisterScan::AsciiConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("loadConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::XmlConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::loadConfiguration<VN::RegisterScan::XmlConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});

    RegisterScan.def("loadConfiguration", [] (BridgeSensor& sensor, VN::RegisterScan::ConfigReader<VN::RegisterScan::GenericConfigReader>& reader) {
            VN::Error error = VN::RegisterScan::loadConfiguration<VN::RegisterScan::GenericConfigReader>(sensor, reader);
            if (error != VN::Error::None) { throwError(error);}});

  declare_vector<uint8_t, 256>(m, "RegisterList");
  py::implicitly_convertible<py::list, Vector<uint8_t, 256>>();
          
}} // namespace VN

