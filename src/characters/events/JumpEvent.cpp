//
// Created by kikker234 on 11-12-2025.
//

#include "characters/events/JumpEvent.h"
#include <iostream>
#include <vector>

#include "characters/BaseCharacter.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/PhysicsComponent.h"
#include "server/GlobalFlags.h"

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
    if (!gameObject) {
        return;
    }
    if (GlobalFlags::isLevelCleaning) {
        return;
    }

    BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(gameObject);
    if (!baseChar) return;
    BaseCharacterController *controller = baseChar->getController();

    if (controller && controller->isActive()) {
        return;
    }

    // Store pending jump instead of applying immediately
    baseChar->setPendingJump(true);
}