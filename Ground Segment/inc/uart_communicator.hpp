#ifndef UART_COMMUNICATOR_HPP
#define UART_COMMUNICATOR_HPP

#include "communicator.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <vector>

namespace altair {

/// UartCommunicator class that handles UART communication with a device.
class UartCommunicator : public Communicator {
public:

    // device: e.g. "/dev/ttyUSB0"; baud_rate: e.g. 115200
    UartCommunicator(std::string const& device, unsigned baud_rate = 115200);
    ~UartCommunicator() override;

    /// Send a fully-formed Packet (will be framed via Protocol::pack)
    void send(Packet const& pkt) override;

    // Register a callback to be called for each received Packet
    void onReceive(ReceiveCallback cb) override;

    /// Open the device and start reading in a background thread
    void start() override;

    /// Stop the background thread and close the device
    void stop() override;

private:
    // Read from the UART device in a loop until stopped
    void readLoop();

private:

    std::string           device_;
    unsigned              baud_rate_;
    int                   fd_ = -1;
    ReceiveCallback       callback_;
    std::thread           reader_;
    std::atomic<bool>     running_{false};
    std::vector<uint8_t>  buffer_;
};

} //namespace altair

#endif // UART_COMMUNICATOR_HPP
