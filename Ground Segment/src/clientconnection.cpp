#include "clientconnection.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <errno.h>

namespace altair {

ClientConnection::ClientConnection(int socket_fd)
  : socket_(socket_fd)
{}

ClientConnection::~ClientConnection() {
    running_ = false;
    if (reader_.joinable()) reader_.join();
    if (socket_ >= 0) ::close(socket_);
}

void ClientConnection::start() {
    running_ = true;
    reader_ = std::thread([self = shared_from_this()]() {
        self->readLoop();
    });
}

void ClientConnection::stop() {
    running_ = false;
}

void ClientConnection::onMessage(PacketCallback cb) {
    callback_ = std::move(cb);
}

void ClientConnection::send(const std::vector<uint8_t>& raw) {
    ssize_t n = ::write(socket_, raw.data(), raw.size());
    if (n != ssize_t(raw.size())) {
        std::cerr << "ClientConnection[" << id_ << "] write error: "
                  << strerror(errno) << "\n";
    }
}

void ClientConnection::setId(int id) {
    id_ = id;
}

int ClientConnection::getId() const {
    return id_;
}

void ClientConnection::readLoop() {

    while (running_) {
        uint8_t byte;
        ssize_t n = ::read(socket_, &byte, 1);
        if (n <= 0) {
            if (n < 0) {
                std::cerr << "[Client " << id_ << "] Read error: " << strerror(errno) << std::endl;
            } 

            running_ = false;

            if (disconnectCallback_) {
                disconnectCallback_(shared_from_this());
            }
            break;
        }

        buffer_.push_back(byte);

        // Attempt to parse one or more complete frames
        while (true) {
            auto opt = Protocol::unpack(buffer_.data(),
                                        static_cast<uint16_t>(buffer_.size()));
            if (!opt) break;

            Packet pkt = *opt;

            if (callback_) {
                callback_(pkt);
            }

            size_t frameLen = 1  /*L*/ + 1 /*ID*/ 
                            + pkt.payload.size() 
                            + 1 /*CRC*/ + 1 /*END*/;
            buffer_.erase(buffer_.begin(),
                          buffer_.begin() + frameLen);
        }
    }
}

void ClientConnection::setDisconnectCallback(DisconnectCallback cb) {
    disconnectCallback_ = std::move(cb);
}

} // namespace altair