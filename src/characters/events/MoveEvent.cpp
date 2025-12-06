#include "characters/events/MoveEvent.hpp"
#include <iostream>

MoveEvent::MoveEvent(int x, int y)
    : _x(x), _y(y) {}

std::string MoveEvent::getName() const {
    return "fireboy-move";
}

Package MoveEvent::serialize() const {
    Package p;

    // STORE AS SIGNED BYTES
    p.push_back(static_cast<int8_t>(_x));
    p.push_back(static_cast<int8_t>(_y));

    return p;
}

Data MoveEvent::deserialize(const Package& package) const {
    Data data;

    if (package.size() >= 2) {
        // interpret bytes as SIGNED (int8)
        int8_t sx = static_cast<int8_t>(package[0]);
        int8_t sy = static_cast<int8_t>(package[1]);

        data.push_back(static_cast<uint8_t>(sx));
        data.push_back(static_cast<uint8_t>(sy));
    }

    return data;
}

void MoveEvent::apply(GameObject* gameObject) {
    auto pos = gameObject->getTransform()->getPosition();

    pos->setX(_x);
    pos->setY(_y);
}
