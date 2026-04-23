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

#include "vectornav/Implementation/BinaryHeader.hpp"
#include "vectornav/Implementation/Packet.hpp"
#if __has_include("vectornav/Implementation/DynamicPacketQueue.hpp")
    #include "vectornav/Implementation/DynamicPacketQueue.hpp"
#endif

#include "PyTemplates.hpp"

namespace py = pybind11;
namespace VN
{

void init_packets(py::module& m)
{
    declare_direct_access_queue_base<VN::Packet>(m, "PacketQueue_Interface");
    declare_direct_access_queue<VN::Packet, 1000>(m, "PacketQueue");

#if __has_include("vectornav/Implementation/DynamicPacketQueue.hpp")
    py::class_<DynamicPacketQueue, PacketQueue_Interface>(m, "DynamicPacketQueue")
    .def(py::init<uint16_t>())
    // .def("put", &DynamicPacketQueue::put)  // Should not be called by user
    .def("get", [](DynamicPacketQueue& queue) -> std::optional<VN::Packet> {
      auto packet = queue.get();
      if (packet) {
        std::optional<VN::Packet> packetOut(packet->length());
        packetOut->details = packet->details;
        std::memcpy(packetOut->buffer, packet->buffer, packet->length());
        return packetOut;
      } else {
        return std::nullopt;
      }
    })
    // .def("getBack", &DynamicPacketQueue::getBack)  // Should not be called by user
    .def("reset", &DynamicPacketQueue::reset)
    .def("size", &DynamicPacketQueue::size)
    .def("isEmpty", &DynamicPacketQueue::isEmpty)
    .def("capacity", &DynamicPacketQueue::capacity)
    ;
#endif

    py::class_<Packet>(m, "Packet")
    .def(py::init<uint16_t>())
    .def("buffer", [](Packet& pkt) -> py::bytes {
        return py::bytes(reinterpret_cast<const char*>(pkt.buffer), pkt.length());
    })
    .def("length", &Packet::length)
    .def("timestamp", &Packet::timestamp)
    .def_readwrite("details", &Packet::details)
    ;

    py::class_<PacketDetails> packetDetails(m, "PacketDetails");
    packetDetails
      .def(py::init<>())  // Default constructor
      .def_readwrite("syncByte", &PacketDetails::syncByte)
      .def_property(
        "metadata",
        [](PacketDetails& self) -> py::object {
          switch (self.syncByte) {
            case PacketDetails::SyncByte::FA:
              return py::cast(self.faMetadata);
            case PacketDetails::SyncByte::Ascii:
              return py::cast(self.asciiMetadata);
            case PacketDetails::SyncByte::FB:
              return py::cast(self.fbMetadata);
            default:
              throw std::runtime_error("Invalid sync byte");
          }
        },
        [](PacketDetails& self, const py::object& metadata) {
            if (self.syncByte == PacketDetails::SyncByte::Ascii) {
                self.asciiMetadata = metadata.cast<AsciiPacketProtocol::Metadata>();
            } else {
                self.faMetadata = metadata.cast<FaPacketProtocol::Metadata>();
            }
          }
      )
    ;


    py::enum_<PacketDetails::SyncByte>(packetDetails, "SyncByte")
      .value("Ascii", PacketDetails::SyncByte::Ascii)
      .value("FA",    PacketDetails::SyncByte::FA)
      .value("FB",    PacketDetails::SyncByte::FB)
      .value("none",  PacketDetails::SyncByte::None)
      ;

    py::class_<FbPacketProtocol::Metadata>(m, "FbMetadata")
      .def(py::init<>())
      .def_readwrite("length",    &FbPacketProtocol::Metadata::length)
      .def_readwrite("timestamp", &FbPacketProtocol::Metadata::timestamp)
      .def_readwrite("header",    &FbPacketProtocol::Metadata::header)
      .def("__repr__",
        [](const FbPacketProtocol::Metadata& metadata) {
          return "<FbMetadata("
                "length=" + std::to_string(metadata.length) + ", "
                //  "timestamp=" + py::repr(metadata.timestamp) + ", "
                //  "header=" + py::repr(metadata.header) +
                ")>";
        }
      )
    ;

    py::class_<FbPacketProtocol::Header>(m, "FbPacketHeader")
      .def(py::init<>())
      .def_readwrite("messageType",        &FbPacketProtocol::Header::messageType)
      .def_readwrite("messageId",          &FbPacketProtocol::Header::messageId)
      .def_readwrite("totalPacketCount",   &FbPacketProtocol::Header::totalPacketCount)
      .def_readwrite("currentPacketCount", &FbPacketProtocol::Header::currentPacketCount)
      .def_readwrite("payloadLength",      &FbPacketProtocol::Header::payloadLength)
      .def("__repr__",
        [](const FbPacketProtocol::Header& h) {
          return "<FbPacketHeader("
                "messageType=" + std::to_string(h.messageType) + ", "
                "messageId=" + std::to_string(h.messageId) + ", "
                "totalPacketCount=" + std::to_string(h.totalPacketCount) + ", "
                "currentPacketCount=" + std::to_string(h.currentPacketCount) + ", "
                "payloadLength=" + std::to_string(h.payloadLength) + ")>";
                }
      )
    ;

}
}
// clang-format on
