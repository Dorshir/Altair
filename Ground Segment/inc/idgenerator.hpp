#ifndef IDGENERATOR_HPP
#define IDGENERATOR_HPP

#include <atomic>

namespace altair {

/// Generates unique IDs for clients and connections.
class IdGenerator {
public:

    /// Returns the singleton instance.
    static IdGenerator& instance();

    /// Returns the next unique ID.
    int nextId();

private:

    /// Private constructor to prevent instantiation.
    IdGenerator();

private:

    std::atomic<int> current_{0};
    
};

} // namespace altair

#endif // IDGENERATOR_HPP
