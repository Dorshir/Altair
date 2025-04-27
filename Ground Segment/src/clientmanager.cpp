#include "clientmanager.hpp"
#include "idgenerator.hpp"
#include "clientconnection.hpp"

#include <iostream>
#include <vector>

namespace altair {

int ClientManager::registerClient(std::shared_ptr<ClientConnection> client) {
    if (!client) {
        throw std::invalid_argument("Client cannot be null");
    }

    int id = IdGenerator::instance().nextId();
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.emplace(id, std::move(client));
    clients_[id]->setId(id); 
    return id;
}

void ClientManager::unregisterClient(int clientId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clients_.find(clientId);
    if (it != clients_.end()) {
        clients_.erase(it);
    }
}

std::shared_ptr<ClientConnection> ClientManager::getClient(int clientId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clients_.find(clientId);
    return (it != clients_.end()) ? it->second : nullptr;
}

void ClientManager::broadcastToAll(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [id, client] : clients_) {
        client->send(data);
    }
}

} // namespace altair