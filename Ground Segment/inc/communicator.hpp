#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include "packet.hpp"
#include <functional>
#include <optional>


namespace altair {

/// Abstract base class for communication interfaces (UART, TCP, etc.).
class Communicator {
public:
    using ReceiveCallback = std::function<void(const Packet&)>;

    virtual ~Communicator() = default;

    // Send a fully‑formed Packet (will be framed via Protocol::pack)
    virtual void send(Packet const& pkt) = 0;

    // Called once to register a callback for incoming Packets
    virtual void onReceive(ReceiveCallback cb) = 0;

    // Opens the device and begins background reading
    virtual void start() = 0;

    // Stops the background thread and closes the device
    virtual void stop() = 0;
};

} //namespace altair

#endif // COMMUNICATOR_HPP
