//
// Created by jusra on 15-12-2025.
//

#include "characters/BaseCharacterController.hpp"

#include <iostream>

#include "SpawnEvent.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Network/NetworkSystem.h"
#include "Physics/PhysicsComponent.h"
#include <iostream>

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

BaseCharacterController::BaseCharacterController(std::shared_ptr<NetworkSystem> network, int parentId,
                                                 EventManager *eventManager, KeyBindings bindings, bool active) {
    _parentId = parentId;
    _eventManager = eventManager;
    _network = network;
    _keyBindings = bindings;
    _active = active;

    std::cout << COLOR_MAGENTA << "========================================\033[0m" << std::endl;
    std::cout << COLOR_MAGENTA << "[KeyInputComponent] CREATED for ID: "
              << parentId << "\033[0m" << std::endl;
    std::cout << COLOR_MAGENTA << "========================================\033[0m" << std::endl;
}

void BaseCharacterController::onKeyPress(Key key) {
    // Handle movement directly for offline mode
    if (key == _keyBindings.left) {
        // _movementDirection = Direction::EAST;
        if (_eventManager && _active) {
            _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(_parentId, Direction::EAST, true));
        }
    } else if (key == _keyBindings.right) {
        // _movementDirection = Direction::WEST;
        if (_eventManager && _active) {
            _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(_parentId, Direction::WEST, true));
        }
    } else if (key == _keyBindings.jump) {
        if (_grounded) {
            _shouldJump = true;
        }
        if (_eventManager && _active) {
            _eventManager->broadcast(_parentId, std::make_shared<JumpEvent>(_parentId));
        }
    }
}

void BaseCharacterController::onKeyRelease(Key key) {
    // Handle movement directly for offline mode
    if (key == _keyBindings.left || key == _keyBindings.right) {
        _movementDirection = Direction::NONE;
    }

    if (_eventManager) {
        if (key == _keyBindings.left) {
            if (_active)
                _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(_parentId, Direction::WEST, false));
        } else if (key == _keyBindings.right) {
            if (_active)
                _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(_parentId, Direction::EAST, false));
        }
    }
}

bool BaseCharacterController::isGrounded() const {
    return _grounded;
}

void BaseCharacterController::setGrounded(bool grounded) {
    _grounded = grounded;
}

void BaseCharacterController::move(Direction direction, PhysicsComponent *physics) {
    float currentVx, currentVy;
    physics->getVelocity(currentVx, currentVy);

    // Handle jump
    if (_shouldJump && _grounded) {
        physics->applyImpulse(0.0f, -_jumpForce);
        _grounded = false;
        _shouldJump = false;
    }

    // Handle horizontal movement
    float vx = 0.0f;
    if (_movementDirection == Direction::EAST) {
        vx = -_movementSpeed;
    } else if (_movementDirection == Direction::WEST) {
        vx = _movementSpeed;
    }

    physics->setVelocity(vx, currentVy);
}
