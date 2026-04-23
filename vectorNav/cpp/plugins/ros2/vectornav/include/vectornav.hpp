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

#ifndef VN_VECTORNAV_HPP_
#define VN_VECTORNAV_HPP_

#include <memory>
#include <string>

// ROS2
#include "rclcpp/rclcpp.hpp"

// VN SDK
#include "vectornav/Config.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/TemplateLibrary/Matrix.hpp"
#include "vectornav_msgs/msg/attitude_group.hpp"
#include "vectornav_msgs/msg/common_group.hpp"
#include "vectornav_msgs/msg/gnss2_group.hpp"
#include "vectornav_msgs/msg/gnss_group.hpp"
#include "vectornav_msgs/msg/imu_group.hpp"
#include "vectornav_msgs/msg/ins_group.hpp"
#include "vectornav_msgs/msg/time_group.hpp"
#include "vectornav_msgs/msg/vector4.hpp"
#include "vectornav_msgs/srv/transaction.hpp"

template <class T>
geometry_msgs::msg::Point toPoint3d(const T& input);

template <>
geometry_msgs::msg::Point toPoint3d(const VN::Vec3d& input)
{
    geometry_msgs::msg::Point lhs;
    lhs.x = input[0];
    lhs.y = input[1];
    lhs.z = input[2];
    return lhs;
}

template <>
geometry_msgs::msg::Point toPoint3d(const VN::Lla& input)
{
    geometry_msgs::msg::Point lhs;
    lhs.x = input.lat;
    lhs.y = input.lon;
    lhs.z = input.alt;
    return lhs;
}

template <class T>
geometry_msgs::msg::Vector3 toVector3(const T& input);

template <>
geometry_msgs::msg::Vector3 toVector3(const VN::Vec3f& input)
{
    geometry_msgs::msg::Vector3 lhs;
    lhs.x = input[0];
    lhs.y = input[1];
    lhs.z = input[2];
    return lhs;
}

template <>
geometry_msgs::msg::Vector3 toVector3(const VN::Ypr& input)
{
    geometry_msgs::msg::Vector3 lhs;
    lhs.x = input.yaw;
    lhs.y = input.pitch;
    lhs.z = input.roll;
    return lhs;
}

geometry_msgs::msg::Quaternion toQuaternion(const VN::Quat& input)
{
    geometry_msgs::msg::Quaternion lhs;
    lhs.x = input.vector[0];
    lhs.y = input.vector[1];
    lhs.z = input.vector[2];
    lhs.w = input.scalar;
    return lhs;
}

class Vectornav : public rclcpp::Node
{
public:
    Vectornav() : Node("vectornav")
    {
        using namespace VN::Registers::System;
        // Device Port
        declare_parameter<std::string>("port", "/dev/ttyUSB0");

        // Baud Rate
        // 9600, 19200 38400 57600 115200 128000 230400 460800 921600
        declare_parameter<int>("BaudRate", static_cast<int>(VN::Sensor::BaudRate::Baud115200));

        // Async Output Type (ASCII)
        declare_parameter<int>("AsyncDataOutputType", static_cast<int>(AsyncOutputType::Ador::YPR));
        declare_parameter<int>("AsyncDataOutputTypeSerialPort", static_cast<int>(AsyncOutputType::SerialPort::Serial1));
        declare_parameter<int>("AsyncDataOutputFrequency", static_cast<int>(AsyncOutputFreq::Adof::Rate20Hz));
        declare_parameter<int>("AsyncDataOutputFrequencySerialPort", static_cast<int>(AsyncOutputFreq::SerialPort::Serial1));

        // Sync control
        declare_parameter<int>("syncInMode", static_cast<int>(SyncControl::SyncInMode::Count));
        declare_parameter<int>("syncInEdge", static_cast<int>(SyncControl::SyncInEdge::RisingEdge));
        declare_parameter<uint16_t>("syncInSkipFactor", 0);
        declare_parameter<int>("syncOutMode", static_cast<int>(SyncControl::SyncOutMode::None));
        declare_parameter<int>("syncOutPolarity", static_cast<int>(SyncControl::SyncOutPolarity::NegativePulse));
        declare_parameter<int>("syncOutSkipFactor", 0);
        declare_parameter<int>("syncOutPulseWidth_ns", 100000000);

        // Communication Protocol Control
        declare_parameter<int>("asciiAppendCount", static_cast<int>(ProtocolControl::AsciiAppendCount::None));
        declare_parameter<int>("asciiAppendStatus", static_cast<int>(ProtocolControl::AsciiAppendStatus::None));
        declare_parameter<int>("spiAppendCount", static_cast<int>(ProtocolControl::SpiAppendCount::None));
        declare_parameter<int>("spiAppendStatus", static_cast<int>(ProtocolControl::SpiAppendStatus::None));
        declare_parameter<int>("asciiChecksum", static_cast<int>(ProtocolControl::AsciiChecksum::Checksum8bit));
        declare_parameter<int>("spiChecksum", static_cast<int>(ProtocolControl::SpiChecksum::Off));
        declare_parameter<int>("errorMode", static_cast<int>(ProtocolControl::ErrorMode::SendError));

        // Binary Output Register 1
        declare_parameter<int>("BO1.asyncMode", 3);
        declare_parameter<int>("BO1.rateDivisor", 400);  // 20Hz
        declare_parameter<int>("BO1.commonField", 0x7FFF);
        declare_parameter<int>("BO1.timeField", 0x0000);
        declare_parameter<int>("BO1.imuField", 0x0000);
        declare_parameter<int>("BO1.gpsField", 0x0010 | 0x0200);
        declare_parameter<int>("BO1.attitudeField", 0x0000);
        declare_parameter<int>("BO1.insField", 0x0004 | 0x0008);
        declare_parameter<int>("BO1.gps2Field", 0x0000);

        // Binary Output Register 2
        declare_parameter<int>("BO2.asyncMode", 0);
        declare_parameter<int>("BO2.rateDivisor", 0);
        declare_parameter<int>("BO2.commonField", 0x0000);
        declare_parameter<int>("BO2.timeField", 0x0000);
        declare_parameter<int>("BO2.imuField", 0x0000);
        declare_parameter<int>("BO2.gpsField", 0x0000);
        declare_parameter<int>("BO2.attitudeField", 0x0000);
        declare_parameter<int>("BO2.insField", 0x0000);
        declare_parameter<int>("BO2.gps2Field", 0x0000);

        // Binary Output Register 3
        declare_parameter<int>("BO3.asyncMode", 0);
        declare_parameter<int>("BO3.rateDivisor", 0);
        declare_parameter<int>("BO3.commonField", 0x0000);
        declare_parameter<int>("BO3.timeField", 0x0000);
        declare_parameter<int>("BO3.imuField", 0x0000);
        declare_parameter<int>("BO3.gpsField", 0x0000);
        declare_parameter<int>("BO3.attitudeField", 0x0000);
        declare_parameter<int>("BO3.insField", 0x0000);
        declare_parameter<int>("BO3.gps2Field", 0x0000);

        // Message Header
        declare_parameter<std::string>("frame_id", "vectornav");
        std::vector<double> tmp;
        declare_parameter<std::vector<double>>("antennaAOffset", tmp);
        declare_parameter<std::vector<double>>("compassBaseline", tmp);
        declare_parameter<std::vector<double>>("compassBaselineU", tmp);

        pubCommon = this->create_publisher<vectornav_msgs::msg::CommonGroup>("vectornav/raw/common", 10);
        pubTime = this->create_publisher<vectornav_msgs::msg::TimeGroup>("vectornav/raw/time", 10);
        pubImu = this->create_publisher<vectornav_msgs::msg::ImuGroup>("vectornav/raw/imu", 10);
        pubGnss = this->create_publisher<vectornav_msgs::msg::GnssGroup>("vectornav/raw/gnss", 10);
        pubAttitude = this->create_publisher<vectornav_msgs::msg::AttitudeGroup>("vectornav/raw/attitude", 10);
        pubIns = this->create_publisher<vectornav_msgs::msg::InsGroup>("vectornav/raw/ins", 10);
        pubGnss2 = this->create_publisher<vectornav_msgs::msg::Gnss2Group>("vectornav/raw/gnss2", 10);
        srvTransaction = this->create_service<vectornav_msgs::srv::Transaction>(
            "vectornav/transaction", std::bind(&Vectornav::vnTransaction, this, std::placeholders::_1, std::placeholders::_2));
    }

    void vnTransaction(const std::shared_ptr<vectornav_msgs::srv::Transaction::Request> request,
                       const std::shared_ptr<vectornav_msgs::srv::Transaction::Response> response)
    {
        RCLCPP_INFO(get_logger(), "[TX] %s", std::string("$VN" + request->send + "*XX").c_str());
        VN::GenericCommand cmd(request->send.c_str());
        VN::Error vnError = vs_.sendCommand(&cmd, VN::Sensor::SendCommandBlockMode::Block);
        if (VN::Error::None != vnError)
        {
            response->recv = "Error: " + std::string(VN::errorCodeToString(vnError));
            RCLCPP_INFO(get_logger(), "[RX] %s", VN::errorCodeToString(vnError));
        }
        else
        {
            response->recv = "[RX] " + std::string(cmd.getResponse().c_str());
            RCLCPP_INFO(get_logger(), "[RX] %s", cmd.getResponse().c_str());
        }
    }

    bool init()
    {
        if (connect(get_parameter("port").as_string(), get_parameter("BaudRate").as_int())) { return true; }

        if (configureSensor()) { return true; }

        return false;
    }

    bool connect(const std::string port, const int requestedBaudRate)
    {
        RCLCPP_INFO(get_logger(), "Starting Connection...");

        VN::Error vnError = vs_.autoConnect(port);  // **todo: Make try catch
        if (vnError != VN::Error::None)
        {
            RCLCPP_INFO(get_logger(), "Unable to connect to device %s", port.c_str());
            RCLCPP_ERROR(get_logger(), "Error: %s", VN::errorCodeToString(vnError));
            return true;
        }

        RCLCPP_INFO(get_logger(), "AutoConnect Complete..");

        if (requestedBaudRate != static_cast<int>(vs_.connectedBaudRate().value()))
        {
            vnError = vs_.changeBaudRate(static_cast<VN::Sensor::BaudRate>(requestedBaudRate));
            if (VN::Error::None != vnError)
            {
                RCLCPP_INFO(get_logger(), "Failure to change device to requested baud rate.");
                RCLCPP_ERROR(get_logger(), "Error: %s", VN::errorCodeToString(vnError));
                return true;
            }
        }

        VN::Registers::System::Model modelRegister;
        vs_.readRegister(&modelRegister);
        std::string modelNumber = modelRegister.model;

        VN::Registers::System::FwVer firmwareRegister;
        vs_.readRegister(&firmwareRegister);
        std::string firmwareVersion = firmwareRegister.fwVer;

        VN::Registers::System::HwVer hardwareRegister;
        vs_.readRegister(&hardwareRegister);
        uint32_t hardwareVersion = hardwareRegister.hwVer;

        VN::Registers::System::Serial serialRegister;
        vs_.readRegister(&serialRegister);
        uint32_t serialNumber = serialRegister.serialNum;

        VN::Registers::System::UserTag userTagRegister;
        vs_.readRegister(&userTagRegister);
        std::string userTagNumber = userTagRegister.tag.value();

        RCLCPP_INFO(get_logger(), "Connected to %s @ %d baud", port.c_str(), static_cast<uint32_t>(vs_.connectedBaudRate().value()));
        RCLCPP_INFO(get_logger(), "Model: %s", modelNumber.c_str());
        RCLCPP_INFO(get_logger(), "Firmware Version: %s", firmwareVersion.c_str());
        RCLCPP_INFO(get_logger(), "Hardware Version : %d", hardwareVersion);
        RCLCPP_INFO(get_logger(), "Serial Number : %d", serialNumber);
        RCLCPP_INFO(get_logger(), "User Tag : \"%s\"", userTagNumber.c_str());

        return false;
    }

    bool configureSensor()
    {
        std::vector<VN::ConfigurationRegister*> configRegisters;

        using namespace VN::Registers::System;
        // Asynchronous Data Output Type Register
        AsyncOutputType asyncDataOutputType;
        asyncDataOutputType.ador = static_cast<AsyncOutputType::Ador>(get_parameter("AsyncDataOutputType").as_int());
        asyncDataOutputType.serialPort = static_cast<AsyncOutputType::SerialPort>(get_parameter("AsyncDataOutputTypeSerialPort").as_int());

        configRegisters.push_back(&asyncDataOutputType);

        // Asynchronous Data Output Frequency Register
        AsyncOutputFreq asyncDataOutputFreq;
        asyncDataOutputFreq.adof = static_cast<AsyncOutputFreq::Adof>(get_parameter("AsyncDataOutputFrequency").as_int());
        asyncDataOutputFreq.serialPort = static_cast<AsyncOutputFreq::SerialPort>(get_parameter("AsyncDataOutputFrequencySerialPort").as_int());

        configRegisters.push_back(&asyncDataOutputFreq);

        // Sync Control Register
        SyncControl syncControlComm;
        syncControlComm.syncInMode = static_cast<SyncControl::SyncInMode>(get_parameter("syncInMode").as_int());
        syncControlComm.syncInEdge = static_cast<SyncControl::SyncInEdge>(get_parameter("syncInEdge").as_int());
        syncControlComm.syncInSkipFactor = get_parameter("syncInSkipFactor").as_int();
        syncControlComm.syncOutMode = static_cast<SyncControl::SyncOutMode>(get_parameter("syncOutMode").as_int());
        syncControlComm.syncOutPolarity = static_cast<SyncControl::SyncOutPolarity>(get_parameter("syncOutPolarity").as_int());
        syncControlComm.syncOutSkipFactor = get_parameter("syncOutSkipFactor").as_int();
        syncControlComm.syncOutPulseWidth = get_parameter("syncOutPulseWidth_ns").as_int();
        syncControlComm.resv1 = 0;
        syncControlComm.resv2 = 0;

        configRegisters.push_back(&syncControlComm);

        // Protocol Control Register
        ProtocolControl protocolConfigComm;
        protocolConfigComm.asciiAppendCount = static_cast<ProtocolControl::AsciiAppendCount>(get_parameter("asciiAppendCount").as_int());
        protocolConfigComm.asciiAppendStatus = static_cast<ProtocolControl::AsciiAppendStatus>(get_parameter("asciiAppendStatus").as_int());
        protocolConfigComm.spiAppendCount = static_cast<ProtocolControl::SpiAppendCount>(get_parameter("spiAppendCount").as_int());
        protocolConfigComm.spiAppendStatus = static_cast<ProtocolControl::SpiAppendStatus>(get_parameter("spiAppendStatus").as_int());
        protocolConfigComm.asciiChecksum = static_cast<ProtocolControl::AsciiChecksum>(get_parameter("asciiChecksum").as_int());
        protocolConfigComm.spiChecksum = static_cast<ProtocolControl::SpiChecksum>(get_parameter("spiChecksum").as_int());
        protocolConfigComm.errorMode = static_cast<ProtocolControl::ErrorMode>(get_parameter("errorMode").as_int());

        configRegisters.push_back(&protocolConfigComm);

        // Binary Output Register 1
        BinaryOutput1 binaryOutput1Comm;
        binaryOutput1Comm.asyncMode = get_parameter("BO1.asyncMode").as_int();
        binaryOutput1Comm.rateDivisor = get_parameter("BO1.rateDivisor").as_int();
        binaryOutput1Comm.common = get_parameter("BO1.commonField").as_int();
        binaryOutput1Comm.time = get_parameter("BO1.timeField").as_int();
        binaryOutput1Comm.imu = get_parameter("BO1.imuField").as_int();
        binaryOutput1Comm.gnss = get_parameter("BO1.gpsField").as_int();
        binaryOutput1Comm.attitude = get_parameter("BO1.attitudeField").as_int();
        binaryOutput1Comm.ins = get_parameter("BO1.insField").as_int();
        binaryOutput1Comm.gnss2 = get_parameter("BO1.gps2Field").as_int();

        configRegisters.push_back(&binaryOutput1Comm);

        // Binary Output Register 2
        BinaryOutput2 binaryOutput2Comm;
        binaryOutput2Comm.asyncMode = get_parameter("BO2.asyncMode").as_int();
        binaryOutput2Comm.rateDivisor = get_parameter("BO2.rateDivisor").as_int();
        binaryOutput2Comm.common = get_parameter("BO2.commonField").as_int();
        binaryOutput2Comm.time = get_parameter("BO2.timeField").as_int();
        binaryOutput2Comm.imu = get_parameter("BO2.imuField").as_int();
        binaryOutput2Comm.gnss = get_parameter("BO2.gpsField").as_int();
        binaryOutput2Comm.attitude = get_parameter("BO2.attitudeField").as_int();
        binaryOutput2Comm.ins = get_parameter("BO2.insField").as_int();
        binaryOutput2Comm.gnss2 = get_parameter("BO2.gps2Field").as_int();

        configRegisters.push_back(&binaryOutput2Comm);

        // Binary Output Register 3
        BinaryOutput3 binaryOutput3Comm;
        binaryOutput3Comm.asyncMode = get_parameter("BO3.asyncMode").as_int();
        binaryOutput3Comm.rateDivisor = get_parameter("BO3.rateDivisor").as_int();
        binaryOutput3Comm.common = get_parameter("BO3.commonField").as_int();
        binaryOutput3Comm.time = get_parameter("BO3.timeField").as_int();
        binaryOutput3Comm.imu = get_parameter("BO3.imuField").as_int();
        binaryOutput3Comm.gnss = get_parameter("BO3.gpsField").as_int();
        binaryOutput3Comm.attitude = get_parameter("BO3.attitudeField").as_int();
        binaryOutput3Comm.ins = get_parameter("BO3.insField").as_int();
        binaryOutput3Comm.gnss2 = get_parameter("BO3.gps2Field").as_int();

        configRegisters.push_back(&binaryOutput3Comm);

        // Identify if sensor is VN-200 or VN-300
        Model modelRegister;
        VN::Error vnError = vs_.readRegister(&modelRegister);
        if (vnError != VN::Error::None)
        {
            RCLCPP_ERROR(get_logger(), "Unable to Determine device family");
            return true;
        }
        std::string modelNumber = modelRegister.model;

        bool configureGnssA = modelNumber[3] == '2' || modelNumber[3] == '3';
        bool configureGnssCompass = modelNumber[3] == '3';

        VN::Registers::GNSS::GnssAOffset antennaAOffsetComm;

        // Configure only if VN-2x0 or VN-3x0
        if (configureGnssA)
        {
            const auto aOffset = get_parameter("antennaAOffset").as_double_array();
            antennaAOffsetComm.positionX = aOffset[0];
            antennaAOffsetComm.positionY = aOffset[1];
            antennaAOffsetComm.positionZ = aOffset[2];
            RCLCPP_INFO(get_logger(), "Antenna A Offset: %f,%f,%f", aOffset[0], aOffset[1], aOffset[2]);

            configRegisters.push_back(&antennaAOffsetComm);
        }

        VN::Registers::GNSSCompass::GnssCompassBaseline compassBaselineComm;
        // Configure only if VN-3x0
        if (configureGnssCompass)
        {
            const auto cBaseline = get_parameter("compassBaseline").as_double_array();
            const auto cBaselineU = get_parameter("compassBaselineU").as_double_array();

            compassBaselineComm.positionX = cBaseline[0];
            compassBaselineComm.positionY = cBaseline[1];
            compassBaselineComm.positionZ = cBaseline[2];
            compassBaselineComm.uncertaintyX = cBaselineU[0];
            compassBaselineComm.uncertaintyY = cBaselineU[1];
            compassBaselineComm.uncertaintyZ = cBaselineU[2];

            configRegisters.push_back(&compassBaselineComm);

            RCLCPP_INFO(get_logger(), "GnssCompassBaseline: %f,%f,%f,%f,%f,%f", cBaseline[0], cBaseline[1], cBaseline[2], cBaselineU[0], cBaselineU[1],
                        cBaselineU[2]);
        }

        for (auto& reg : configRegisters)
        {
            vnError = vs_.writeRegister(reg);
            if (vnError != VN::Error::None)
            {
                // RCLCPP_ERROR(get_logger(), "Unable to configure Register %d: %s -> %s", reg->id(), reg->name(), VN::errorCodeToString(vnError));
                RCLCPP_ERROR(get_logger(), "Unable to configure Register %d -> %s", reg->id(), VN::errorCodeToString(vnError));
                return true;
            }
        }

        return false;
    }

    void publishMessages();

private:
    void publishCommonGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishTimeGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishImuGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishGnssGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishAttitudeGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishInsGroup(const VN::CompositeData* cd, const rclcpp::Time& rosTime);
    void publishGnss2Group(const VN::CompositeData* cd, const rclcpp::Time& rosTime);

    rclcpp::Publisher<vectornav_msgs::msg::CommonGroup>::SharedPtr pubCommon;
    rclcpp::Publisher<vectornav_msgs::msg::TimeGroup>::SharedPtr pubTime;
    rclcpp::Publisher<vectornav_msgs::msg::ImuGroup>::SharedPtr pubImu;
    rclcpp::Publisher<vectornav_msgs::msg::GnssGroup>::SharedPtr pubGnss;
    rclcpp::Publisher<vectornav_msgs::msg::AttitudeGroup>::SharedPtr pubAttitude;
    rclcpp::Publisher<vectornav_msgs::msg::InsGroup>::SharedPtr pubIns;
    rclcpp::Publisher<vectornav_msgs::msg::Gnss2Group>::SharedPtr pubGnss2;
    rclcpp::Service<vectornav_msgs::srv::Transaction>::SharedPtr srvTransaction;

    VN::Sensor vs_;
};

#define COMMON_GROUP_ENABLE 1
#include <publishing.hpp>
#endif  // VN_VECTORNAV_HPP_
