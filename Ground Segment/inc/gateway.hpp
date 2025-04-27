#ifndef GATEWAY_HPP
#define GATEWAY_HPP

#include "tcpserver.hpp"
#include "uart_communicator.hpp"
#include "clientmanager.hpp"

#include <memory>
#include <string>

namespace altair {

/// Gateway class that manages TCP and UART communication.
class Gateway {
public:

    Gateway(uint16_t tcp_port, const std::string& uart_device);
    ~Gateway();

    /// Starts the TCP server and UART communicator.
    void start();

    /// Stops the TCP server and UART communicator.
    void stop();

private:

    /// Handles a new TCP client connection.
    void handleNewClient(std::shared_ptr<ClientConnection> client);

    /// Handles a TCP client disconnection.
    void handleClientDisconnect(std::shared_ptr<ClientConnection> client);

    /// Handles a received Packet from a TCP client.
    void handleTcpPacket(std::shared_ptr<ClientConnection> client, const Packet& pkt);

    /// Handles a received Packet from the UART device.
    void handleUartPacket(const Packet& pkt);

    /// Sends a time synchronization packet to the UART device.
    void sendTimeSync();

private:

    std::unique_ptr<TCPServer> tcp_server_;
    std::unique_ptr<UartCommunicator> uart_comm_;
    ClientManager client_manager_;
    bool running_{false};
};

} // namespace altair
#endif