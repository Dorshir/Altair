#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>

namespace altair {

class ClientConnection;  // forward declaration

/// Manages mapping from client IDs to ClientConnection pointers.
class ClientManager {
public:
    ClientManager() = default;
    ~ClientManager() = default;

    /// Registers a new client, returns its unique ID.
    int registerClient(std::shared_ptr<ClientConnection> client);

    /// Unregisters (and stops tracking) the client with the given ID.
    void unregisterClient(int clientId);

    /// Returns the shared pointer to the client, or nullptr if not found.
    std::shared_ptr<ClientConnection> getClient(int clientId);

    /// Broadcasts data to all connected clients.
    void broadcastToAll(const std::vector<uint8_t>& data);

private:

    std::mutex mutex_;
    std::unordered_map<int, std::shared_ptr<ClientConnection>> clients_;
    
};

} // namespace altair

#endif // CLIENTMANAGER_HPP
