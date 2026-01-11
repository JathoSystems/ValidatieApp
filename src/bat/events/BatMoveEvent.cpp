#include "bat/events/BatMoveEvent.hpp"
#include "bat/BatAI.h"
#include "GameObjects/Transform/Transform.h"
#include "GameObjects/Transform/Position.h"
#include "GameObjects/ObjectRegistry.hpp"
#include <cmath>
#include <cstring>

BatMoveEvent::BatMoveEvent(int objectId, float x, float y)
    : _objectId(objectId), _x(x), _y(y) {
}

std::string BatMoveEvent::getName() const {
    return "batmove";
}

Package BatMoveEvent::serialize() const {
    Package p;

    const uint8_t* idBytes = reinterpret_cast<const uint8_t*>(&_objectId);
    for (int i = 0; i < sizeof(int); ++i) {
        p.push_back(idBytes[i]);
    }

    const uint8_t* xBytes = reinterpret_cast<const uint8_t*>(&_x);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(xBytes[i]);

    const uint8_t* yBytes = reinterpret_cast<const uint8_t*>(&_y);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(yBytes[i]);
    
    return p;
}

Data BatMoveEvent::deserialize(const Package &package) {
    Data data;
    
    if (package.size() >= 12) {
        // Deserialize object ID as 4 bytes (int)
        std::memcpy(&_objectId, &package[0], sizeof(int));
        
        // Deserialize X position
        std::memcpy(&_x, &package[4], sizeof(float));
        
        // Deserialize Y position
        std::memcpy(&_y, &package[8], sizeof(float));
        
        // Copy back to data
        for (size_t i = 0; i < package.size(); ++i) {
            data.push_back(package[i]);
        }
    }
    
    return data;
}

void BatMoveEvent::apply(GameObject* gameObject) {
    // Now called directly on main thread - safe to access game objects
    GameObject* obj = ObjectRegistry::getInstance().getObject(_objectId);
    if (!obj) return;
    
    BatAI* batAI = obj->getComponent<BatAI>();
    if (!batAI) return;

    batAI->setNetworkPosition(_x, _y);
}
