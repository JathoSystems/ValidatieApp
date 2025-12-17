#include "SpawnEvent.hpp"

std::unordered_map<int, int> SpawnEvent::idMapping;

int SpawnEvent::getMappedId(int originalId) {
    auto it = idMapping.find(originalId);
    if (it != idMapping.end()) {
        return it->second;
    }
    return originalId;
}
