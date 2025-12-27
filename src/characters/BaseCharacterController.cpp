//
// Created by jusra on 15-12-2025.
//

#include "characters/BaseCharacterController.hpp"

#include <iostream>

#include "SpawnEvent.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "GameObjects/ObjectRegistry.hpp"
#include "Network/NetworkSystem.h"
#include "Physics/PhysicsComponent.h"

#define COLOR_RED     "\033[31m"
#define COLOR_MAGENTA "\033[35m"

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

void BaseCharacterController::getCurrentPosition(float &x, float &y) {
    x = 0; y = 0;
    GameObject* obj = ObjectRegistry::getInstance().getObject(_parentId);
    if (obj) {
        x = obj->getTransform()->getPosition()->getX();
        y = obj->getTransform()->getPosition()->getY();
    }
}

// Helper to determine direction based on which keys are held
void BaseCharacterController::updateMovementDirection() {
    Direction newDirection = Direction::NONE;

    if (_isLeftPressed && !_isRightPressed) {
        newDirection = Direction::EAST;
    } else if (_isRightPressed && !_isLeftPressed) {
        newDirection = Direction::WEST;
    }

    if (_movementDirection != newDirection) {
        _movementDirection = newDirection;

        if (_eventManager && _active) {
            // get position to send as well so the error will be fixed
            float x, y;
            getCurrentPosition(x, y);

            _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(_parentId, _movementDirection, _movementDirection != Direction::NONE, x, y));
        }
    }
}

void BaseCharacterController::update(float delta) {
    // Only the active player sends sync packets
    if (!_active || !_eventManager) return;

    // Only sync if we are actually moving
    if (_movementDirection != Direction::NONE) {
        _syncTimer += delta;

        // Send a packet every 0.1 seconds (100ms)
        if (_syncTimer > 0.1f) {
            _syncTimer = 0.0f;

            // Get current real position
            float x = 0, y = 0;
            getCurrentPosition(x, y);

            // Broadcast the correction!
            _eventManager->broadcast(_parentId, std::make_shared<MoveEvent>(
                _parentId,
                _movementDirection,
                true, // Toggle is true because we are still moving
                x,
                y
            ));
        }
    } else {
        _syncTimer = 0.0f;
    }
}

void BaseCharacterController::onKeyPress(Key key) {
    if (key == _keyBindings.left) {
        _isLeftPressed = true;
        updateMovementDirection();
    } else if (key == _keyBindings.right) {
        _isRightPressed = true;
        updateMovementDirection();
    } else if (key == _keyBindings.jump) {
        if (_grounded) {
            _shouldJump = true;

            if (_eventManager && _active) {
                _eventManager->broadcast(_parentId, std::make_shared<JumpEvent>(_parentId));
            }
        }

    }
}

void BaseCharacterController::onKeyRelease(Key key) {
    if (key == _keyBindings.left) {
        _isLeftPressed = false;
        updateMovementDirection();
    } else if (key == _keyBindings.right) {
        _isRightPressed = false;
        updateMovementDirection();
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

    float newVy = currentVy;

    // Handle jump
    if (_shouldJump && _grounded) {
        std::cout << "\033[33m[CONTROLLER] JUMP! Setting vertical velocity.\033[0m" << std::endl;

        newVy = -800.0f;

        _grounded = false;
        _shouldJump = false;
    }

    float targetVx = 0.0f;

    if (_movementDirection == Direction::EAST) {
        targetVx = -_movementSpeed;
    } else if (_movementDirection == Direction::WEST) {
        targetVx = _movementSpeed;
    }

    physics->setVelocity(targetVx, newVy);
}