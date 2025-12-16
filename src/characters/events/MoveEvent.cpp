#include "characters/events/MoveEvent.hpp"
#include <iostream>

#include "characters/BaseCharacter.hpp"
#include "enums/Direction.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/PhysicsComponent.h"

MoveEvent::MoveEvent(int objectId, Direction direction, bool toggle) : _objectId(objectId), _direction(direction),
                                                                       _toggle(toggle) {
}

std::string MoveEvent::getName() const {
    return "move";
}

Package MoveEvent::serialize() const {
    Package p;

    std::cout << "=== SERIALIZE ===" << std::endl;
    std::cout << "ObjectId: " << _objectId << " (as int: " << static_cast<int>(_objectId) << ")" << std::endl;
    std::cout << "Direction: " << static_cast<int>(_direction) << std::endl;
    std::cout << "Toggle: " << static_cast<int>(_toggle) << std::endl;

    p.push_back(static_cast<uint8_t>(_objectId));
    p.push_back(static_cast<uint8_t>(_toggle));
    p.push_back(static_cast<uint8_t>(_direction));

    std::cout << "Package contents: ";
    for (auto byte : p) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return p;
}

Data MoveEvent::deserialize(const Package &package) {
    Data data;

    std::cout << "=== DESERIALIZE ===" << std::endl;
    std::cout << "Package size: " << package.size() << std::endl;
    std::cout << "Package contents: ";
    for (auto byte : package) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    if (package.size() >= 3) {
        uint8_t objectId = package.at(0);
        uint8_t toggle = package.at(1);
        uint8_t direction = package.at(2);

        std::cout << "Deserialized objectId: " << static_cast<int>(objectId) << std::endl;
        std::cout << "Deserialized toggle: " << static_cast<int>(toggle) << std::endl;
        std::cout << "Deserialized direction: " << static_cast<int>(direction) << std::endl;

        _objectId = objectId;
        _toggle = static_cast<bool>(toggle);
        _direction = static_cast<Direction>(direction);
    }

    return data;
}

void MoveEvent::apply(GameObject *gameObject) {
    if (BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(gameObject)) {
        if (!_toggle) {
            baseChar->setMovementDirection(Direction::NONE);
        } else {
            baseChar->setMovementDirection(_direction);
        }
    }
}
