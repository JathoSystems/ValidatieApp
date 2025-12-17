#include "bat/events/BatMoveEvent.hpp"
#include "bat/BatAI.h"
#include "GameObjects/Transform/Transform.h"
#include "GameObjects/Transform/Position.h"
#include <cmath>

BatMoveEvent::BatMoveEvent(int objectId, float directionX, float directionY)
    : _objectId(objectId), _directionX(directionX), _directionY(directionY) {
}

std::string BatMoveEvent::getName() const {
    return "batmove";
}

Package BatMoveEvent::serialize() const {
    Package p;

    p.push_back(static_cast<uint8_t>(_objectId));

    int8_t dirXScaled = static_cast<int8_t>(_directionX * 100.0f);
    int8_t dirYScaled = static_cast<int8_t>(_directionY * 100.0f);
    
    p.push_back(static_cast<uint8_t>(dirXScaled));
    p.push_back(static_cast<uint8_t>(dirYScaled));
    
    return p;
}

Data BatMoveEvent::deserialize(const Package &package) {
    Data data;
    
    if (package.size() >= 3) {
        // Deserialize object ID
        _objectId = static_cast<int>(package[0]);
        
        // Deserialize directionX and directionY (scaled back from int)
        int8_t dirXScaled = static_cast<int8_t>(package[1]);
        int8_t dirYScaled = static_cast<int8_t>(package[2]);
        
        _directionX = static_cast<float>(dirXScaled) / 100.0f;
        _directionY = static_cast<float>(dirYScaled) / 100.0f;
        
        // Copy back to data
        for (size_t i = 0; i < package.size(); ++i) {
            data.push_back(package[i]);
        }
    }
    
    return data;
}

void BatMoveEvent::apply(GameObject* gameObject) {
    if (!gameObject) return;
    
    // Get BatAI component
    BatAI* batAI = gameObject->getComponent<BatAI>();
    if (!batAI) return;

    batAI->setDirection(_directionX, _directionY);
}
