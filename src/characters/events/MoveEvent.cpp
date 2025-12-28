#include "characters/events/MoveEvent.hpp"
#include <iostream>
#include <cstring>
#include <cmath>

#include "characters/BaseCharacter.hpp"
#include "enums/Direction.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/PhysicsComponent.h"

// Update constructor
MoveEvent::MoveEvent(int objectId, Direction direction, bool toggle, float x, float y)
    : _objectId(objectId), _direction(direction), _toggle(toggle), _x(x), _y(y) {
}

std::string MoveEvent::getName() const {
    return "move";
}

Package MoveEvent::serialize() const {
    Package p;

    p.push_back(static_cast<uint8_t>(_objectId));
    p.push_back(static_cast<uint8_t>(_toggle));
    p.push_back(static_cast<uint8_t>(_direction));

    // Serialize Float X (4 bytes)
    const uint8_t* xBytes = reinterpret_cast<const uint8_t*>(&_x);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(xBytes[i]);

    // Serialize Float Y (4 bytes)
    const uint8_t* yBytes = reinterpret_cast<const uint8_t*>(&_y);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(yBytes[i]);

    return p;
}

Data MoveEvent::deserialize(const Package &package) {
    Data data;

    // We now expect at least 3 + 4 + 4 = 11 bytes
    if (package.size() >= 11) {
        _objectId = package.at(0);
        _toggle = static_cast<bool>(package.at(1));
        _direction = static_cast<Direction>(package.at(2));

        // Deserialize X
        std::memcpy(&_x, &package[3], sizeof(float));

        // Deserialize Y
        std::memcpy(&_y, &package[7], sizeof(float));
    }

    return data;
}

void MoveEvent::apply(GameObject *gameObject) {
    if (BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(gameObject)) {
        BaseCharacterController* controller = baseChar->getController();

        // DEBUG: Log what's happening
        bool isActive = controller && controller->isActive();
        std::cout << "[MoveEvent] ObjectID: " << _objectId
                  << " Active: " << (isActive ? "YES" : "NO")
                  << " Pos: (" << _x << ", " << _y << ")"
                  << " Dir: " << static_cast<int>(_direction)
                  << " Toggle: " << _toggle << std::endl;

        // Don't apply to active player - they control themselves
        if (isActive) {
            std::cout << "[MoveEvent] Skipping - this is the active player" << std::endl;
            return;
        }

        // Store the pending physics update data
        baseChar->setPendingNetworkUpdate(_x, _y, _direction, _toggle);
    }
}