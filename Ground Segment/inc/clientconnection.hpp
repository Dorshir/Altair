#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include "packet.hpp"
#include "protocol.hpp"

#include <vector>
#include <cstdint>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>


namespace altair {

/// Represents one TCP client; reads framed Packets and invokes a callback.
class ClientConnection : public std::enable_shared_from_this<ClientConnection> {
public:
    using PacketCallback = std::function<void(const Packet&)>;
    using DisconnectCallback = std::function<void(std::shared_ptr<ClientConnection>)>;

    explicit ClientConnection(int socket_fd);
    ~ClientConnection();

    /// Begin the background read loop.
    void start();

    /// Stop the background read loop and close the socket.
    void stop();

    /// Register to be called on each received Packet.
    void onMessage(PacketCallback cb);

    /// Send a ready‑framed packet (i.e. output of Protocol::pack).
    void send(const std::vector<uint8_t>& raw);

    /// Assign and retrieve its unique ID.
    void setId(int id);
    int  getId() const;

    /// Set a callback to be called when the connection is closed.
    void setDisconnectCallback(DisconnectCallback cb);

private:

    /// Read from the socket in a loop until stopped.
    void readLoop();

private:

    DisconnectCallback        disconnectCallback_;
    int                       socket_;
    int                       id_{0};
    PacketCallback            callback_;
    std::thread               reader_;
    std::atomic<bool>         running_{false};
    std::vector<uint8_t>      buffer_;
};

} // namespace altair

#endif // CLIENTCONNECTION_HPP
