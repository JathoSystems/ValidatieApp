//
// Created by kikker234 on 11-12-2025.
//

#include "characters/events/JumpEvent.h"
#include <iostream>
#include <vector>

#include "characters/BaseCharacter.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "GameObjects/ObjectRegistry.hpp"
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
    // Now called directly on main thread - safe to access game objects
    GameObject* obj = ObjectRegistry::getInstance().getObject(_objectId);
    if (!obj) return;
    
    BaseCharacter *baseChar = dynamic_cast<BaseCharacter *>(obj);
    if (!baseChar) return;
    BaseCharacterController *controller = baseChar->getController();

    if (controller && controller->isActive()) {
        return;
    }

    // Store pending jump instead of applying immediately
    baseChar->setPendingJump(true);
}