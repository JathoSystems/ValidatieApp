#include "characters/events/MoveEvent.hpp"
#include <iostream>
#include <cstring> // For std::memcpy

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

        // Don't apply to active player
        if (controller && controller->isActive()) {
            return;
        }

        // CRITICAL: Apply position correction WITH interpolation to reduce jitter
        float currentX = baseChar->getTransform()->getPosition()->getX();
        float currentY = baseChar->getTransform()->getPosition()->getY();

        // Calculate error
        float errorX = _x - currentX;
        float errorY = _y - currentY;
        float errorMagnitude = std::sqrt(errorX * errorX + errorY * errorY);

        // If error is large (> 50 pixels), snap immediately (lag spike or initial spawn)
        // Otherwise, smoothly correct over time
        const float SNAP_THRESHOLD = 50.0f;
        const float CORRECTION_SPEED = 0.3f; // Interpolation factor

        if (errorMagnitude > SNAP_THRESHOLD) {
            // Large desync - snap immediately
            baseChar->getTransform()->getPosition()->setX(_x);
            baseChar->getTransform()->getPosition()->setY(_y);

            if(auto* physics = baseChar->getComponent<PhysicsComponent>()) {
                physics->setPosition(_x, _y);
            }
        } else if (errorMagnitude > 1.0f) {
            // Small desync - interpolate smoothly
            float correctedX = currentX + errorX * CORRECTION_SPEED;
            float correctedY = currentY + errorY * CORRECTION_SPEED;

            baseChar->getTransform()->getPosition()->setX(correctedX);
            baseChar->getTransform()->getPosition()->setY(correctedY);

            if(auto* physics = baseChar->getComponent<PhysicsComponent>()) {
                physics->setPosition(correctedX, correctedY);
            }
        }

        if(auto* physics = baseChar->getComponent<PhysicsComponent>()) {
            float vx, vy;
            physics->getVelocity(vx, vy);

            if (!_toggle) {
                physics->setVelocity(0.0f, vy);
            } else {
                float targetVx = (_direction == Direction::EAST) ? -300.0f : 300.0f;
                physics->setVelocity(targetVx, vy);
            }
        }

        // Update visual movement direction
        baseChar->setMovementDirection(_toggle ? _direction : Direction::NONE);
    }
}