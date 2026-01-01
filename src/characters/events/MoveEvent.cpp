#include "characters/events/MoveEvent.hpp"
#include <iostream>
#include <cstring>
#include <cmath>

#include "characters/BaseCharacter.hpp"
#include "enums/Direction.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/PhysicsComponent.h"

MoveEvent::MoveEvent(int objectId, Direction direction, bool toggle, float x, float y, float vx, float vy)
    : _objectId(objectId), _direction(direction), _toggle(toggle), _x(x), _y(y), _vx(vx), _vy(vy) {
}

std::string MoveEvent::getName() const {
    return "move";
}

Package MoveEvent::serialize() const {
    Package p;

    p.push_back(static_cast<uint8_t>(_objectId));
    p.push_back(static_cast<uint8_t>(_toggle));
    p.push_back(static_cast<uint8_t>(_direction));

    // Serialize X
    const uint8_t* xBytes = reinterpret_cast<const uint8_t*>(&_x);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(xBytes[i]);

    // Serialize Y
    const uint8_t* yBytes = reinterpret_cast<const uint8_t*>(&_y);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(yBytes[i]);

    // Serialize Velocity X
    const uint8_t* vxBytes = reinterpret_cast<const uint8_t*>(&_vx);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(vxBytes[i]);

    // Serialize Velocity Y
    const uint8_t* vyBytes = reinterpret_cast<const uint8_t*>(&_vy);
    for (int i = 0; i < sizeof(float); ++i) p.push_back(vyBytes[i]);

    return p;
}

Data MoveEvent::deserialize(const Package &package) {
    Data data;

    // Size check: 3 header + 4(x) + 4(y) + 4(vx) + 4(vy) = 19 bytes
    if (package.size() >= 19) {
        _objectId = package.at(0);
        _toggle = static_cast<bool>(package.at(1));
        _direction = static_cast<Direction>(package.at(2));

        std::memcpy(&_x, &package[3], sizeof(float));
        std::memcpy(&_y, &package[7], sizeof(float));

        // Deserialize Velocities
        std::memcpy(&_vx, &package[11], sizeof(float));
        std::memcpy(&_vy, &package[15], sizeof(float));
    }

    return data;
}

void MoveEvent::apply(GameObject *gameObject) {
    if (BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(gameObject)) {
        BaseCharacterController* controller = baseChar->getController();

        bool isActive = controller && controller->isActive();
        if (isActive) return;

        // Pass velocity to the character
        baseChar->setPendingNetworkUpdate(_x, _y, _vx, _vy, _direction, _toggle);
    }
}