#include "gateway.hpp"
#include "protocol.hpp"
#include "protocol_defs.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <cstring>

#define PROTO_PKT_TIME_SYNC 0x04   


namespace altair {

using namespace std::string_literals; 

Gateway::Gateway(uint16_t tcp_port, const std::string& uart_device)
    : tcp_server_(std::make_unique<TCPServer>(tcp_port))
    , uart_comm_(std::make_unique<UartCommunicator>(uart_device))
{
    // Set up callbacks
    tcp_server_->setMessageCallback(
        [this](std::shared_ptr<ClientConnection> client, const Packet& pkt) {
            handleTcpPacket(client, pkt);
        });
    
    tcp_server_->setClientConnectedCallback(
        [this](std::shared_ptr<ClientConnection> client) {
            handleNewClient(client);
        });
    
    tcp_server_->setClientDisconnectedCallback(
        [this](std::shared_ptr<ClientConnection> client) {
            handleClientDisconnect(client);
        });

    uart_comm_->onReceive([this](const Packet& pkt) {
        handleUartPacket(pkt);
    });

}

Gateway::~Gateway() {
    stop();
}


void Gateway::start() {
    if (running_) return;
    
    running_ = true;

    try {
        uart_comm_->start();
        tcp_server_->start();

        sendTimeSync();
    }
    catch (const std::exception& e) {
        stop();
        throw std::runtime_error("Failed to start Gateway: "s + e.what());
    }
}

void Gateway::sendTimeSync() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    struct tm* ltm = std::localtime(&time);
    
    // Format: YYYYMMDDHHMMSS (exactly 14 chars)
    char timestr[15];
    int len = std::snprintf(timestr, sizeof(timestr), 
                          "%04d%02d%02d%02d%02d%02d",
                          ltm->tm_year + 1900,
                          ltm->tm_mon + 1,
                          ltm->tm_mday,
                          ltm->tm_hour,
                          ltm->tm_min,
                          ltm->tm_sec);
    
    if (len != 14) {
        std::cerr << "[Gateway] ERROR: Generated time string wrong length: " 
                  << len << std::endl;
        return;
    }

    // Create and send time sync packet
    Packet timePkt;
    timePkt.packetId = PROTO_PKT_TIME_SYNC;
    timePkt.payload.assign(timestr, timestr + 14);

    uart_comm_->send(timePkt);
}

void Gateway::stop() {
    running_ = false;
    uart_comm_->stop();
    tcp_server_->stop();
}

void Gateway::handleNewClient(std::shared_ptr<ClientConnection> client) {
    try {
        client_manager_.registerClient(client);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to register client: " << e.what() << std::endl;
    }
}

void Gateway::handleClientDisconnect(std::shared_ptr<ClientConnection> client) {
    if (!client) return;
    
    int clientId = client->getId();
    client_manager_.unregisterClient(clientId);
}

void Gateway::handleTcpPacket(std::shared_ptr<ClientConnection> client, 
                             const Packet& pkt) {
    if (!client || !running_) return;

    try {
        std::cout << "[Gateway] Received TCP packet:" << std::endl;
        std::cout << "  ID: 0x" << std::hex << static_cast<int>(pkt.packetId) 
                  << std::dec << std::endl;
        std::cout << "  Payload size: " << pkt.payload.size() << std::endl;
        std::cout << "  Payload hex: ";
        for (uint8_t b : pkt.payload) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(b) << " ";
        }

        std::cout << std::dec << std::endl;
        std::cout << "  Payload ASCII: ";
        for (uint8_t b : pkt.payload) {
            std::cout << (std::isprint(b) ? static_cast<char>(b) : '.');
        }
        std::cout << std::endl;

        switch (pkt.packetId) {
            case PROTO_PKT_SAMPLE:
            case PROTO_PKT_EVENT:
                std::cout << "[Gateway] Forwarding log request to UART" << std::endl;
                uart_comm_->send(pkt);
                break;

            default:
                std::cerr << "[Gateway] Unknown packet type from client: " 
                         << static_cast<int>(pkt.packetId) << std::endl;
                break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to forward TCP packet to UART: " 
                  << e.what() << std::endl;
    }
}

void Gateway::handleUartPacket(const Packet& uart_pkt) {
    if (!running_) {
        return;
    }

    try {
        switch (uart_pkt.packetId) {
            case PROTO_PKT_SAMPLE:
            {
                auto framed_packet = Protocol::pack(uart_pkt);
                client_manager_.broadcastToAll(framed_packet);
                break;
            }
            default:
            {
                std::cout << std::dec << std::endl;
                for (uint8_t b : uart_pkt.payload) {
                    std::cout << (std::isprint(b) ? static_cast<char>(b) : '.');
                }
                std::cout << std::endl;
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to broadcast UART packet: " << e.what() << std::endl;
    }
}

} // namespace altair