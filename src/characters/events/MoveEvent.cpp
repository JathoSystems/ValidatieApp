#include "characters/events/MoveEvent.hpp"
#include <iostream>

#include "enums/Direction.hpp"
#include "Physics/PhysicsComponent.h"

MoveEvent::MoveEvent(int objectId, Direction direction, bool toggle):
    _objectId(objectId), _direction(direction), _toggle(toggle) {}

std::string MoveEvent::getName() const {
    return "move";
}

Package MoveEvent::serialize() const {
    Package p;

    p.push_back(_objectId);
    p.push_back(_toggle);
    p.push_back(static_cast<int8_t>(_direction));

    return p;
}

Data MoveEvent::deserialize(const Package& package) {
    Data data;

    if (package.size() >= 3) {
        int8_t objectId = static_cast<int8_t>(package[0]);
        int8_t toggle = static_cast<int8_t>(package[1]);
        int8_t direction = static_cast<int8_t>(package[2]);

        data.push_back(objectId);
        data.push_back(toggle);
        data.push_back(direction);

        _direction = static_cast<Direction>(direction);
        _toggle = toggle;
        _objectId = objectId;
    }

    return data;
}

void MoveEvent::apply(GameObject* gameObject) {
    std::cout << DirectionToString(_direction);
}
