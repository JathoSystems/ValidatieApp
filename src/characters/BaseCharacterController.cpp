//
// Created by jusra on 15-12-2025.
//

#include "characters/BaseCharacterController.hpp"

#include <iostream>

#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Physics/PhysicsComponent.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "characters/BaseCharacter.hpp"
#include "Network/GameState.h"

BaseCharacterController::BaseCharacterController(int parentId, EventManager *eventManager, ControlScheme scheme)
    : _grounded(true),
      _movementDirection(Direction::NONE),
      _movementSpeed(300.0f),
      _jumpingSpeed(5000.0f),
      _eventManager(eventManager),
      _parentId(parentId),
      _scheme(scheme) {}

void BaseCharacterController::onKeyPress(Key key) {
    if (!_eventManager) {
        std::cerr << "Event manager is null!" << std::endl;
        return;
    }

    // Do not process any local input until the server has signaled that
    // the game is ready (both clients connected).
    if (!GameNetworkState::isReady()) {
        return;
    }

    switch (key) {
        // MOVE LEFT
        case Key::A:
            if (_scheme != ControlScheme::WASD) break;
            [[fallthrough]];
        case Key::LEFT:
            if (_scheme != ControlScheme::ARROWS && _scheme != ControlScheme::WASD) break;
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::WEST, true));
            if (auto* obj = ObjectRegistry::getInstance().getObject(_parentId)) {
                if (auto* baseChar = dynamic_cast<BaseCharacter*>(obj)) {
                    baseChar->setMovementDirection(Direction::WEST);
                }
            }
            break;

        // MOVE RIGHT
        case Key::D:
            if (_scheme != ControlScheme::WASD) break;
            [[fallthrough]];
        case Key::RIGHT:
            if (_scheme != ControlScheme::ARROWS && _scheme != ControlScheme::WASD) break;
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::EAST, true));
            if (auto* obj = ObjectRegistry::getInstance().getObject(_parentId)) {
                if (auto* baseChar = dynamic_cast<BaseCharacter*>(obj)) {
                    baseChar->setMovementDirection(Direction::EAST);
                }
            }
            break;

        // JUMP
        case Key::W:
            if (_scheme != ControlScheme::WASD) break;
            [[fallthrough]];
        case Key::UP:
            if (_scheme != ControlScheme::ARROWS && _scheme != ControlScheme::WASD) break;
            if (!_grounded) break;

            // 1) Stuur jump-event via netwerk zodat andere clients gesynchroniseerd blijven
            _eventManager->broadcast(std::make_shared<JumpEvent>(_parentId));

            // 2) Voer de sprong DIRECT lokaal uit voor instant feedback,
            //    in plaats van te wachten op de roundtrip via de server.
            if (GameObject* obj = ObjectRegistry::getInstance().getObject(_parentId)) {
                if (auto* physics = obj->getComponent<PhysicsComponent>()) {
                    float vx, vy;
                    physics->getVelocity(vx, vy);
                    float jumpForce = 10000.0f;
                    physics->setVelocity(vx, -jumpForce);
                }
            }
            break;
    }
}

void BaseCharacterController::onKeyRelease(Key key) {
    // Also ignore key releases until the game is ready
    if (!GameNetworkState::isReady()) {
        return;
    }

    switch (key) {
        case Key::A:
            if (_scheme != ControlScheme::WASD) break;
            [[fallthrough]];
        case Key::LEFT:
            if (_scheme != ControlScheme::ARROWS && _scheme != ControlScheme::WASD) break;
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::WEST, false));
            if (auto* obj = ObjectRegistry::getInstance().getObject(_parentId)) {
                if (auto* baseChar = dynamic_cast<BaseCharacter*>(obj)) {
                    baseChar->setMovementDirection(Direction::NONE);
                }
            }
            break;
        case Key::D:
            if (_scheme != ControlScheme::WASD) break;
            [[fallthrough]];
        case Key::RIGHT:
            if (_scheme != ControlScheme::ARROWS && _scheme != ControlScheme::WASD) break;
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::EAST, false));
            if (auto* obj = ObjectRegistry::getInstance().getObject(_parentId)) {
                if (auto* baseChar = dynamic_cast<BaseCharacter*>(obj)) {
                    baseChar->setMovementDirection(Direction::NONE);
                }
            }
            break;
        default:
            break;
    }
}

bool BaseCharacterController::isGrounded() const {
    return _grounded;
}

void BaseCharacterController::setGrounded(bool grounded) {
    _grounded = grounded;
}

void BaseCharacterController::move(Direction direction, PhysicsComponent* physics) {
    _movementDirection = direction;

    float currentVx, currentVy;
    physics->getVelocity(currentVx, currentVy);

    float vx = 0.0f;
    if (_movementDirection == Direction::WEST) {
        vx = -_movementSpeed;
    } else if (_movementDirection == Direction::EAST) {
        vx = _movementSpeed;
    }

    // Always update X velocity so releases stop the character
    physics->setVelocity(vx, currentVy);
}
