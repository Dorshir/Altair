#include "idgenerator.hpp"

namespace altair {

IdGenerator& IdGenerator::instance() {
    static IdGenerator inst;
    return inst;
}

IdGenerator::IdGenerator() = default;

int IdGenerator::nextId() {
    return ++current_;
}

} // namespace altair