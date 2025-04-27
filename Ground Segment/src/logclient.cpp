#include "logclient.hpp"
#include "protocol_defs.hpp"

#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace altair {

LogClient::LogClient(const std::string& host, uint16_t port) 
    : host_(host), port_(port) {
    connectToServer();
}

LogClient::~LogClient() {
    if (connection_) {
        connection_->setDisconnectCallback(nullptr);
    }
}

void LogClient::connectToServer() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address");
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Connection failed");
    }

    connection_ = std::make_shared<ClientConnection>(sock);
    connection_->onMessage([this](const Packet& pkt) {
        handlePacket(pkt);
    });
    
    connection_->setDisconnectCallback([this](std::shared_ptr<ClientConnection>) {
        std::cout << "Disconnected from server\n";
        running_ = false;
    });

    connection_->start();
}

void LogClient::handlePacket(const Packet& pkt) {
    switch (pkt.packetId) {
        case PROTO_PKT_SAMPLE:
            saveLogData(pkt.payload);
            break;
        default:
            std::cout << "Received unknown packet type: " 
                      << static_cast<int>(pkt.packetId) << std::endl;
            break;
    }
}

void LogClient::saveLogData(const std::vector<uint8_t>& data) {
    std::string filename = current_type_ == PROTO_PKT_SAMPLE ? 
                          "samples.txt" : "events.txt";
    
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

void LogClient::requestLogs(uint8_t type, const std::string& start_date, 
                          const std::string& end_date) {
    current_type_ = type;
    
    // Format dates as YYYYMMDDYYYYMMDD
    std::string payload = start_date + end_date;
    payload.erase(std::remove(payload.begin(), payload.end(), '-'), payload.end());
    
    if (payload.length() != 16) {
        std::cerr << "Invalid date format\n";
        return;
    }

    Packet request;
    request.packetId = type;
    request.payload.assign(payload.begin(), payload.end());
    
    auto framed = Protocol::pack(request);
    connection_->send(framed);

    std::cout << "\nRetrieved data." << std::endl;

    std::cout << "Data saved to " << (current_type_ == PROTO_PKT_SAMPLE ? 
                "samples.txt" : "events.txt")
     << std::endl;
}

void LogClient::run() {
    running_ = true;
    std::cout << "Connected to server at " << host_ << ":" << port_ << "\n";
    std::cout << "Hello! You can now retrieve logs.\n";
    
    while (running_) {
        std::cout << "\nLog Retrieval Menu:\n"
                  << "1. Get Samples\n"
                  << "2. Get Events\n"
                  << "3. Exit\n"
                  << "Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 3) break;

        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice\n";
            continue;
        }

        std::string start_date, end_date;
        std::cout << "Enter start date (YYYY-MM-DD): ";
        std::getline(std::cin, start_date);
        
        std::cout << "Enter end date (YYYY-MM-DD): ";
        std::getline(std::cin, end_date);

        uint8_t type = (choice == 1) ? PROTO_PKT_SAMPLE : PROTO_PKT_EVENT;
        
        requestLogs(type, start_date, end_date);
    }
}

} // namespace altair