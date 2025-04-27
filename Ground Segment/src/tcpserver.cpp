#include "tcpserver.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <errno.h>

namespace altair {

TCPServer::TCPServer(uint16_t port)
  : port_{port}
{}

TCPServer::~TCPServer() {
    stop();
    if (accept_thread_.joinable()) accept_thread_.join();
    if (server_fd_ >= 0) ::close(server_fd_);
}

void TCPServer::start() {
    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
        throw std::runtime_error("TCPServer: socket failed");

    int opt = 1;
    ::setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR,
                 &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (::bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("TCPServer: bind failed");

    if (::listen(server_fd_, SOMAXCONN) < 0)
        throw std::runtime_error("TCPServer: listen failed");

    running_ = true;
    accept_thread_ = std::thread(&TCPServer::acceptLoop, this);
}

void TCPServer::setMessageCallback(MessageCallback cb) {
    messageCb_ = std::move(cb);
}

void TCPServer::acceptLoop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t   len = sizeof(client_addr);
        int client_fd = ::accept(server_fd_,
                                  (sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            if (running_)
                std::cerr << "TCPServer accept error: "
                          << strerror(errno) << "\n";
            continue;
        }

        auto conn = std::make_shared<ClientConnection>(client_fd);
        conn->onMessage([this, conn](const Packet& p){
            if (messageCb_) messageCb_(conn, p);
        });

        conn->setDisconnectCallback([this](std::shared_ptr<ClientConnection> client) {
            if (clientDisconnectedCb_) {
                clientDisconnectedCb_(client);
            }
        });
        
        conn->start();
        
        // Notify Gateway of new client
        if (clientConnectedCb_) {
            clientConnectedCb_(conn);
        }
    }
}

void TCPServer::setClientConnectedCallback(ClientCallback cb) {
    clientConnectedCb_ = std::move(cb);
}

void TCPServer::setClientDisconnectedCallback(ClientCallback cb) {
    clientDisconnectedCb_ = std::move(cb);
}

void TCPServer::stop() {
    running_ = false;
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto& client : clients_) {
            client->stop();
        }
    }
}

} // namespace altair