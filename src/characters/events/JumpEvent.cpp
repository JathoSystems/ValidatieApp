//
// Created by kikker234 on 11-12-2025.
//

#include "characters/events/JumpEvent.h"

#include <iostream>
#include <vector>
#include "Physics/PhysicsComponent.h"

std::string JumpEvent::getName() const {
    return "jump";
}

Package JumpEvent::serialize() const {
    std::vector<uint8_t> vector;

    std:: cout << "Serializing JumpEvent " << _objectId << std::endl;

    vector.push_back(_objectId);

    return vector;
}

Data JumpEvent::deserialize(const Package &package) {
    _objectId = package.at(0);

    std::cout << "Deserializing JumpEvent " << _objectId << std::endl;

    return package;
}

void JumpEvent::apply(GameObject* gameObject) {
    PhysicsComponent* physics = gameObject->getComponent<PhysicsComponent>();

    if (!physics) return;

    float jumpForce = 10000;
    float vx, vy;
    physics->getVelocity(vx, vy);
    physics->setVelocity(vx, - jumpForce);
}
