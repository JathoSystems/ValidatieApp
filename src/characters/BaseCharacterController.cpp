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

BaseCharacterController::BaseCharacterController(int parentId, EventManager *eventManager) {
    _parentId = parentId;
    _eventManager = eventManager;
}

void BaseCharacterController::onKeyPress(Key key) {
    if (!_eventManager) {
        std::cerr << "Event manager is null!" << std::endl;
        return;
    }

    switch (key) {
        case Key::A:
        case Key::LEFT:
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::EAST, true));
            break;
        case Key::D:
        case Key::RIGHT:
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::WEST, true));
            break;
        case Key::SPACE:
        case Key::W:
        case Key::UP:
            if (!_grounded) break;

            _eventManager->broadcast(std::make_shared<JumpEvent>(_parentId));
            break;
        case Key::Q:
            extern std::shared_ptr<NetworkSystem> network;
            std::cout << "sending: " << _parentId << std::endl;
            network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(_parentId, "fireboy"));
            break;
    }
}

void BaseCharacterController::onKeyRelease(Key key) {
    switch (key) {
        case Key::A:
        case Key::LEFT:
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::WEST, false));
            break;
        case Key::D:
        case Key::RIGHT:
            _eventManager->broadcast(std::make_shared<MoveEvent>(_parentId, Direction::EAST, false));
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
    if (_movementDirection == Direction::EAST) {
        vx = -_movementSpeed;
        physics->setVelocity(vx, currentVy);
    } else if (_movementDirection == Direction::WEST) {
        vx = _movementSpeed;
        physics->setVelocity(vx, currentVy);
    }
}
