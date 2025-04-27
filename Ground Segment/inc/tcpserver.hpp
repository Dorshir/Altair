#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <set>

#include "clientconnection.hpp"
#include "packet.hpp"

namespace altair {

/// Accepts TCP clients on a port and forwards their Packets via a callback.
class TCPServer {
public:
    /// Called for every Packet from any client: (clientPtr, packet)
    using MessageCallback =
      std::function<void(std::shared_ptr<ClientConnection>, const Packet&)>;

    /// Called when a client connects or disconnects: (clientPtr)
    using ClientCallback = std::function<void(std::shared_ptr<ClientConnection>)>;

    explicit TCPServer(uint16_t port);
    ~TCPServer();

    /// Begin listening + accepting in background.
    void start();
    
    /// Stop accepting clients and close the server socket.
    void stop();

    /// Set the global handler for incoming Packets :contentReference[oaicite:0]{index=0}.
    void setMessageCallback(MessageCallback cb);

    /// Set the handler for when a client connects.
    void setClientConnectedCallback(ClientCallback cb);

    /// Set the handler for when a client disconnects.
    void setClientDisconnectedCallback(ClientCallback cb);

private:

    /// Accept clients in a loop until stopped.
    void acceptLoop();

private:

    int                                             server_fd_{-1};
    uint16_t                                        port_;
    std::atomic<bool>                               running_{false};
    std::thread                                     accept_thread_;
    MessageCallback                                 messageCb_;
    std::mutex                                      clients_mutex_;
    ClientCallback                                  clientConnectedCb_;
    ClientCallback                                  clientDisconnectedCb_;
    std::set<std::shared_ptr<ClientConnection>>     clients_;

};

} // namespace altair

#endif // TCPSERVER_HPP
