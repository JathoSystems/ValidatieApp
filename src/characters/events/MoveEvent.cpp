#include "characters/events/MoveEvent.hpp"
#include <iostream>

#include "Physics/PhysicsComponent.h"

MoveEvent::MoveEvent(int x, int y)
    : _x(x), _y(y) {}

std::string MoveEvent::getName() const {
    return "move";
}

Package MoveEvent::serialize() const {
    Package p;

    p.push_back(static_cast<int8_t>(_x));
    p.push_back(static_cast<int8_t>(_y));

    return p;
}

Data MoveEvent::deserialize(const Package& package) {
    Data data;

    if (package.size() >= 2) {
        int8_t sx = static_cast<int8_t>(package[0]);
        int8_t sy = static_cast<int8_t>(package[1]);

        data.push_back(static_cast<uint8_t>(sx));
        data.push_back(static_cast<uint8_t>(sy));

        if (_x == 0) _x = sx;
        if (_y == 0) _y = sy;
    }

    return data;
}

void MoveEvent::apply(GameObject* gameObject) {
    PhysicsComponent* physics = gameObject->getComponent<PhysicsComponent>();

    if (!physics) return;

    physics->setVelocity(_x, _y);

    gameObject->getTransform()->getPosition()->setX(_x);
    gameObject->getTransform()->getPosition()->setX(_y);
}
