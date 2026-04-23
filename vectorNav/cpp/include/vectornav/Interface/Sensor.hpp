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

#ifndef VN_SENSOR_HPP_
#define VN_SENSOR_HPP_

#include <array>
#include <atomic>
#include <memory>
#include <optional>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/File.hpp"
#include "vectornav/HAL/Serial.hpp"
#include "vectornav/HAL/Serial_Base.hpp"
#include "vectornav/HAL/Thread.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/AsciiHeader.hpp"
#include "vectornav/Implementation/AsciiPacketDispatcher.hpp"
#include "vectornav/Implementation/CommandProcessor.hpp"
#include "vectornav/Implementation/FaPacketDispatcher.hpp"
#include "vectornav/Implementation/FbPacketDispatcher.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/Implementation/PacketSynchronizer.hpp"
#include "vectornav/Implementation/QueueDefinitions.hpp"
#include "vectornav/Interface/Commands.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/GenericCommand.hpp"
#include "vectornav/Interface/Registers.hpp"

namespace VN
{

class SensorTestHarness;

/// @brief This is the top-level class used by the SDK. In nominal operation, the user will instantiate a sensor object and interact directly with it.
/// Consequently, public Sensor methods are all direct API.
class Sensor
{
public:
    /// @brief Enumeration defining measurement queue operation modes.
    /// Specifies how measurements should be handled when adding them to the queue,
    /// with values mapped to corresponding MeasurementQueue::PutMode options.

    enum class MeasQueueMode : uint8_t
    {
        Off = 0,
        Force = static_cast<uint8_t>(MeasurementQueue::PutMode::Force),
        Try = static_cast<uint8_t>(MeasurementQueue::PutMode::Try),
#if THREADING_ENABLE
        Retry = static_cast<uint8_t>(MeasurementQueue::PutMode::Retry)
#endif
    };

    // ------------------------------------------
    /*! \name Constructor and Destructor */
    // ------------------------------------------
    /// @brief Default constructor.
    /// @param mode Measurement queue mode (default: Force).
    Sensor(MeasQueueMode mode = MeasQueueMode::Force);

    /// @brief Constructor to statically allocate Sensor object. For usage, see relevant documentation and examples.
    /// @tparam MainByteBufferCapacity The capacity for the MainByteBuffer.
    /// @tparam FbByteBufferCapacity The capacity for the fbBuffer.
    /// @param mainBuffer The allocated memory for mainBuffer.
    /// @param fbBuffer The allocated memory for fbBuffer.
    /// @param mode Measurement queue mode (default: Force).
    template <size_t MainByteBufferCapacity, size_t FbByteBufferCapacity>
    Sensor(std::array<uint8_t, MainByteBufferCapacity>& mainBuffer, std::array<uint8_t, FbByteBufferCapacity>& fbBuffer,
           MeasQueueMode mode = MeasQueueMode::Force);

    /// @brief Constructor to dynamically allocate Sensor object using ByteBuffer objects.
    /// @param mainBuffer The allocated ByteBuffer object for mainBuffer.
    /// @param fbBuffer The allocated ByteBuffer object for fbBuffer.
    /// @param mode Measurement queue mode (default: Force).
    Sensor(ByteBuffer& mainBuffer, ByteBuffer& fbBuffer, MeasQueueMode mode = MeasQueueMode::Force);

    /// @brief Default destructor.
    ~Sensor();

    // ------------------------------------------
    /*! \name Serial Connectivity */
    // ------------------------------------------

    using BaudRate = Registers::System::BaudRate::BaudRates;

    /// @brief Opens the serial port at the specified baud rate. This does not verify any connectivity, and as such only validates that the serial port is
    /// available for opening. If THREADING_ENABLE, this starts the Listening Thread.
    /// @param portName The name of the port to connect.
    /// @param baudRate The baud rate at which to connect.
    Error connect(const Serial_Base::PortName& portName, const BaudRate baudRate) noexcept;

    /// @brief Opens the file specified. If THREADING_ENABLE, this starts the Listening Thread.
    /// @param fileName The name of the file to connect.
    Error connect(const Filesystem::FilePath& fileName) noexcept;

    /// @brief Opens the serial port, scanning all possible baud rates until the unit is verified to be connected. This performs a verifySensorConnectivity at
    /// each possible baud rate. If THREADING_ENABLE, this starts the Listening Thread.
    /// @param portName The port name to which to connect.
    Error autoConnect(const Serial_Base::PortName& portName) noexcept;

    /// @brief Sends a ReadRegister for the Model register. Returns true if a valid response is received, otherwise returns false.
    bool verifySensorConnectivity() noexcept;

    /// @brief Runs verifySensorConnectivity() at all valid baud rates to re-establish connection with sensor.  Disconnects if no response received.
    Error autoBaud() noexcept;

    /// @brief Gets the port name of the open serial port. If no port is open, will return std::nullopt.
    std::optional<Serial_Base::PortName> connectedPortName() const noexcept { return _serial.connectedPortName(); };

    /// @brief Gets the baud rate at which the serial port is opened. If no port is open, will return std::nullopt.
    std::optional<BaudRate> connectedBaudRate() const noexcept
    {
        auto connectedBaudRate = _serial.connectedBaudRate();
        return connectedBaudRate ? std::make_optional(static_cast<BaudRate>(*connectedBaudRate)) : std::nullopt;
    };

    // @brief Changes the baud rate on a specified serial port of the sensor. If the targeted port is the currently active one, it also updates the host's baud
    // rate to match.  Will retry on failure
    Error changeBaudRate(const BaudRate newBaudRate,
                         Registers::System::BaudRate::SerialPort serialPort = Registers::System::BaudRate::SerialPort::ActiveSerial) noexcept;

    /// @brief Changes the host (computer) baud rate without commanding a change to the VectorNav unit's baud rate. Use with caution.
    Error changeHostBaudRate(const BaudRate newBaudRate) noexcept;

    /// @brief Disconnects from the unit. If THREADING_ENABLE, this closes the Listening Thread.
    void disconnect() noexcept;

    // ------------------------------------------
    /*! \name Accessing Measurements */
    // ------------------------------------------

    using CompositeDataQueueReturn = DirectAccessQueue_Interface<CompositeData>::value_type;

    /// @brief Checks to see if there is a new measurement available on the MeasurementQueue.
    bool hasMeasurement() const noexcept { return !_measurementQueue.isEmpty(); }

    /// @brief Gets (and pops) the front of the MeasurementQueue.
    /// @param block If true, wait a maximum of getMeasurementTimeoutLength for a new measurement.
    CompositeDataQueueReturn getNextMeasurement(const bool block = true) noexcept;

    /// @brief Gets the back (most recent) of the MeasurementQueue, popping every measurement in the queue.
    /// @param block If true, wait a maximum of getMeasurementTimeoutLength for a new measurement.
    CompositeDataQueueReturn getMostRecentMeasurement(const bool block = true) noexcept;

    // ------------------------------------------
    /*! \name Sending Commands */
    // ------------------------------------------

    /// @brief Block mode for sending a command. @see sendCommand()
    enum class SendCommandBlockMode
    {
        None,            ///< Do not wait for a response from the unit before returning.
        Block,           ///< Block upon a response from the unit until the specific timeout; will return ResponseTimeout if timeout is hit.
        BlockWithRetry,  ///< Block upon a response from the unit until the specific timeout; retry sending the command and blocking commandSendRetriesAllowed
                         ///< times before returning ResponseTimeout.
    };

    /// @brief Sends a Read Register command to the unit to poll register values and blocks on the unit's response.
    /// @param registerToRead The register object to be populated by the unit's response.
    /// @param retryOnFailure Whether to retry sending the Read Register command to the unit if no confirmation is received within commandSendTimeoutLength.
    Error readRegister(Register* registerToRead, const bool retryOnFailure = true) noexcept;

    /// @brief Sends a Write Register command to the unit to set register values and blocks on the unit's response.  Will return a NotEnoughParameters error
    /// if not all register fields have been set by user.
    /// @param registerToWrite The register object to write to the unit.
    /// @param retryOnFailure Whether to retry sending the write register command to the unit if no confirmation is received within commandSendTimeoutLength.
    Error writeRegister(ConfigurationRegister* registerToWrite, const bool retryOnFailure = true) noexcept;

    /// @brief Sends a Write Settings command to the unit and blocks on the unit's confirmation.
    Error writeSettings() noexcept;

    /// @brief Sends a Reset command to the unit and blocks on the unit's confirmation. After confirmation, it sleeps for resetSleepDuration and verifies sensor
    /// connectivity, calling autoConnect if failed.
    Error reset() noexcept;

    /// @brief Sends a Restore Factory Settings command to the unit, blocks on the unit's confirmation, reopens serial at the unit's default baud rate, sleeps
    /// for resetSleepDuration, then verifies unit connectivity. If THREADING_ENABLE, resets the Listening Thread.
    Error restoreFactorySettings() noexcept;

    /// @brief Sends a Known Magnetic Disturbance command to the sensor and blocks on the unit's message confirmation.
    Error knownMagneticDisturbance(const KnownMagneticDisturbance::State state) noexcept;

    /// @brief Sends a Known Acceleration Disturbance command to the sensor and blocks on the unit's message confirmation.
    Error knownAccelerationDisturbance(const KnownAccelerationDisturbance::State state) noexcept;

    /// @brief Sends a Set Initial Heading command to the sensor and blocks on the unit's message confirmation.
    Error setInitialHeading(const float heading) noexcept;
    Error setInitialHeading(const Ypr& ypr) noexcept;
    Error setInitialHeading(const Quat& quat) noexcept;

    /// @brief Sends an Async Output Enable command to the sensor and blocks on the unit's message confirmation.
    Error asyncOutputEnable(const AsyncOutputEnable::State state) noexcept;

    /// @brief Sends a Set Filter Bias command to the sensor and blocks on the unit's message confirmation.
    Error setFilterBias() noexcept;

    /// @brief Sends a Set Bootloader command to the sensor and blocks on the unit's message confirmation.
    Error setBootloader(const SetBootLoader::Processor processorId) noexcept;

    /// @brief Sends an arbitrary command to the unit. The direct command API should be preferred unless it necessary to send without blocking.
    /// @param commandToSend The command object to send to the unit.
    /// @param waitMode How to handle waiting and retrying.
    /// @param waitLength Duration to wait before retrying or returning a ResponseTimeout. Only valid if waitMode is not None.
    /// @param timeoutThreshold Duration after which the message will be popped from the queue
    Error sendCommand(GenericCommand* commandToSend, SendCommandBlockMode waitMode, const Microseconds waitLength = Config::Sensor::commandSendTimeoutLength,
                      const Microseconds timeoutThreshold = Config::CommandProcessor::commandRemovalTimeoutLength) noexcept;

    /// @brief Sends an arbitrary message to the unit without any message modification or response validation. Not recommended for use.
    Error serialSend(const char* buffer, size_t len) noexcept;

    // ------------------------------------------
    /*! \name Additional Logging */
    // ------------------------------------------

    /// @brief Registers a ByteBuffer to be populated (write only) with every byte received by the serial port. Only one can be registered at a time.
    /// @param receivedByteBuffer The ByteBuffer to be populated with all received bytes.
    void registerReceivedByteBuffer(ByteBuffer* const receivedByteBuffer) noexcept;

    /// @brief Deregisters the ByteBuffer previously registered to be populated with received bytes, if applicable. If not, it has no effect.
    void deregisterReceivedByteBuffer() noexcept;

    using SyncByte = PacketDetails::SyncByte;
    using AsciiSubscriberFilterType = AsciiPacketDispatcher::SubscriberFilterType;
    using FaSubscriberFilterType = FaPacketDispatcher::SubscriberFilterType;
    using Fb00SubscriberFilter = FbPacketDispatcher::SubscriberFilter;
    using BinaryOutputMeasurements = Registers::System::BinaryOutputMeasurements;

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    /// @param queueToSubscribe The queue to be populated with measurement messages.
    /// @param syncByte The sync byte for which to subscribe the passed queue.  Use SyncByte:None for skipped bytes.
    Error subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const SyncByte syncByte) noexcept;

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    /// @param queueToSubscribe The queue to be populated with measurement messages.
    /// @param binaryOutputMeasurementFilter The filter to determine which measurement messages are populated in the respective queue.
    /// @param filterType How to interpret the respective binaryOutputMeasurementFilter.  Defaults to ExactMatch if not used.
    Error subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const BinaryOutputMeasurements& binaryOutputMeasurementFilter,
                             const FaSubscriberFilterType filterType = FaSubscriberFilterType::ExactMatch) noexcept;

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    /// @param queueToSubscribe The queue to be populated with measurement messages.
    /// @param asciiHeaderFilter The filter to determine which measurement messages are populated in the respective queue.
    /// @param filterType How to interpret the respective asciiHeaderFilter.  Defaults to StartsWith if not used.
    Error subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const AsciiHeader& asciiHeaderFilter,
                             const AsciiSubscriberFilterType filterType = AsciiSubscriberFilterType::StartsWith) noexcept;

    /// @brief Subscribes a queue to be populated (write only) with every matching measurement message as received. Multiple can be simultaneously registered.
    /// @param queueToSubscribe The queue to be populated with measurement messages.
    /// @param fb00Filter The filter to determine which measurement messages are populated in the respective queue.
    Error subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const Fb00SubscriberFilter fb00Filter) noexcept;

    /// @brief Unsubscribes the passed queue from all messages matching the passed sync byte, regardless of filter.
    /// @param queueToUnsubscribe The queue to unsubscribe.
    /// @param syncByte The sync byte from which to unsubscribe the passed queue.
    void unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const SyncByte syncByte) noexcept;

    /// @brief Unsubscribes the passed queue from all messages matching the passed filter.
    /// @param queueToUnsubscribe The queue to unsubscribe.
    /// @param filter The filter from which to unsubscribe the passed queue.
    void unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const BinaryOutputMeasurements& filter) noexcept;

    /// @brief Unsubscribes the passed queue from all messages matching the passed filter.
    /// @param queueToUnsubscribe The sync byte from which to unsubscribe the passed queue.
    /// @param filter The filter from which to unsubscribe the passed queue.
    void unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const AsciiHeader& filter) noexcept;

    // ------------------------------------------
    /*! @name Unthreaded Packet Processing */
    // ------------------------------------------
#if (!THREADING_ENABLE)

    /// @brief Loads the contents of the serial buffer into the mainBuffer. If THREADING_ENABLE, this is called in loop by the listening thread.
    Error loadMainBufferFromSerial() noexcept;

    /// @brief Loads the contents of the file into the mainBuffer. If THREADING_ENABLE, this is called in loop by the listening thread.
    Error loadMainBufferFromFile() noexcept;

    /// @brief Triggers a single iteration of checking for, parsing, and forwarding packet to necessary queue. If THREADING_ENABLE, this is called in loop by
    /// the listening thread.
    /// @return Returns error (true) if no packet found in current buffer.  Indicates more data is needed.
    Errored processNextPacket() noexcept;
#endif

    /// @brief This is only used for software integration testing.
    friend class SensorTestHarness;

    // -------------------------------
    /*! @name Error Handling */
    // -------------------------------

    /// @brief The amount of asynchronous errors in the queue.
    uint16_t asyncErrorQueueSize() const noexcept { return _asyncErrorQueue.size(); }

    /// @brief Gets the next asynchronous error in the queue.
    std::optional<AsyncError> getNextAsyncError() noexcept { return _asyncErrorQueue.get(); }

    /// @brief Clears the asynchronous errors queue.
    void clearAsyncErrorQueue() noexcept { _asyncErrorQueue.reset(); }

private:
    //-------------------------------
    // Connectivity
    //-------------------------------
    ByteBuffer _mainByteBuffer{Config::PacketFinders::mainBufferCapacity};
    Serial _serial{_mainByteBuffer};
    InputFile _file{false};

    enum class ConnectionType
    {
        None,
        Serial,
        File
    };
    ConnectionType _connectionType = ConnectionType::None;

#if (THREADING_ENABLE)
    std::atomic<bool> _listening = false;
    std::unique_ptr<Thread> _listeningThread = nullptr;
    void _listen() noexcept;
    Error loadMainBufferFromSerial() noexcept;
    Error loadMainBufferFromFile() noexcept;
    Errored processNextPacket() noexcept;
    void _startListening() noexcept;
    void _stopListening() noexcept;
    mutable Mutex _sensorMutex;
#endif

    // -------------------------------
    // Measurement Operators
    // -------------------------------
    MeasurementQueue _measurementQueue{MeasurementQueue::PutMode::Force, Config::PacketDispatchers::compositeDataQueueCapacity};
    Sensor::CompositeDataQueueReturn _blockOnMeasurement(Timer& timer, const Microseconds sleepLength) noexcept;

    //-------------------------------
    // Command Operators
    //-------------------------------
    CommandProcessor _commandProcessor{[this](AsyncError&& error) { _asyncErrorQueue.put(std::move(error)); }};
    Error _blockOnCommand(GenericCommand* commandToWait, Timer& timer) noexcept;

    // -------------------------------
    // Packet Processing
    // -------------------------------
    MeasQueueMode _measQueueMode;
    bool _parseToCD;
    FaPacketDispatcher _faPacketDispatcher{&_measurementQueue, Config::PacketDispatchers::cdEnabledMeasTypes, _parseToCD};
    AsciiPacketDispatcher _asciiPacketDispatcher{&_measurementQueue, Config::PacketDispatchers::cdEnabledMeasTypes, &_commandProcessor, _parseToCD};
    FbPacketDispatcher _fbPacketDispatcher{&_faPacketDispatcher, Config::PacketFinders::fbBufferCapacity};

    PacketSynchronizer _packetSynchronizer{_mainByteBuffer, [this](AsyncError&& error) { _asyncErrorQueue.put(std::move(error)); }};

    ByteBuffer* _receivedByteBuffer = nullptr;
    void _copyToReceivedByteBuffer(size_t mainBufferIndex) noexcept;
    // -------------------------------
    // Error handling
    // -------------------------------

    Queue_Mutexed<AsyncError, Config::Errors::asyncErrorQueueCapacity> _asyncErrorQueue{};
};

template <size_t MainByteBufferCapacity, size_t FbBufferCapacity>
Sensor::Sensor(std::array<uint8_t, MainByteBufferCapacity>& mainBuffer, std::array<uint8_t, FbBufferCapacity>& fbBuffer, MeasQueueMode mode)
    : _mainByteBuffer(mainBuffer.data(), mainBuffer.size()),
      _measQueueMode{mode},
      _parseToCD{mode != MeasQueueMode::Off},
      _fbPacketDispatcher(&_faPacketDispatcher, fbBuffer.data(), fbBuffer.size())
{
    // Set up packet synchronizer
    _packetSynchronizer.addDispatcher(&_faPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_asciiPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_fbPacketDispatcher);
    if (_parseToCD) { _measurementQueue.setPutMode(static_cast<MeasurementQueue::PutMode>(_measQueueMode)); }
}
}  // namespace VN

#endif  // VN_SENSOR_HPP_
