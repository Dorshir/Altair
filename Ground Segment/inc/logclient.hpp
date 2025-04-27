#ifndef LOG_CLIENT_HPP
#define LOG_CLIENT_HPP

#include "clientconnection.hpp"
#include "protocol.hpp"
#include "protocol_defs.hpp"

#include <string>
#include <atomic>
#include <memory>

namespace altair {

/// LogClient class that connects to a TCP server and handles log data.
class LogClient {
public:

    LogClient(const std::string& host, uint16_t port);
    ~LogClient();

    /// Starts the client and connects to the server.
    void run();

private:
    /// Handles incoming packets from the server.
    void connectToServer();

    /// Handles the received packet and processes it.
    void handlePacket(const Packet& pkt);

    /// Saves the log data to a file.
    void saveLogData(const std::vector<uint8_t>& data);

    /// Connects to the server and sets up the connection.
    void requestLogs(uint8_t type, const std::string& start_date, const std::string& end_date);

private:

    std::string host_;
    uint16_t port_;
    std::shared_ptr<ClientConnection> connection_;
    std::atomic<bool> running_{false};
    uint8_t current_type_{0};
};

} // namespace altair

#endif // LOG_CLIENT_HPP