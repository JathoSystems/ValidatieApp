//
// Created by kikker234 on 11-12-2025.
//

#include "characters/events/JumpEvent.h"
#include <iostream>
#include <vector>

#include "characters/BaseCharacter.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/PhysicsComponent.h"

std::string JumpEvent::getName() const {
    return "jump";
}

Package JumpEvent::serialize() const {
    std::vector<uint8_t> vector;
    vector.push_back(_objectId);
    return vector;
}

Data JumpEvent::deserialize(const Package &package) {
    _objectId = package.at(0);
    return package;
}

void JumpEvent::apply(GameObject *gameObject) {
    BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(gameObject);
    if (!baseChar) return;
    BaseCharacterController *controller = baseChar->getController();

    if (controller && controller->isActive()) {
        return;
    }

    PhysicsComponent *physics = gameObject->getComponent<PhysicsComponent>();
    if (!physics) return;

    if (controller && !controller->isGrounded()) {
        return;
    }

    float jumpVelocity = -800.0f;

    float vx, vy;
    physics->getVelocity(vx, vy);
    physics->setVelocity(vx, jumpVelocity);

    if (controller) {
        controller->setGrounded(false);
    }
}