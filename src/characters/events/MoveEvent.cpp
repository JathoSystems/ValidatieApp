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

    p.push_back(static_cast<uint8_t>(_objectId));
    p.push_back(static_cast<uint8_t>(_toggle));
    p.push_back(static_cast<uint8_t>(_direction));

    return p;
}

Data MoveEvent::deserialize(const Package &package) {
    Data data;

    if (package.size() >= 3) {
        uint8_t objectId = package.at(0);
        uint8_t toggle = package.at(1);
        uint8_t direction = package.at(2);

        std::cout << "Deserialized MoveEvent: objectId=" << static_cast<int>(objectId) << "\n";

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
