//
// Created by kikker234 on 11-12-2025.
//

#include "characters/events/JumpEvent.h"

#include <vector>
#include "Physics/PhysicsComponent.h"

std::string JumpEvent::getName() const {
    return "jump";
}

Package JumpEvent::serialize() const {
    return std::vector<uint8_t>();
}

Data JumpEvent::deserialize(const Package &package) {
    return std::vector<uint8_t>();

}

void JumpEvent::apply(GameObject* gameObject) {
    PhysicsComponent* physics = gameObject->getComponent<PhysicsComponent>();

    if (!physics) return;

    float jumpForce = 10000;
    float vx, vy;
    physics->getVelocity(vx, vy);
    physics->setVelocity(vx, - jumpForce);
}
