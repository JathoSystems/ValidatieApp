//
// Created by jusra on 9-12-2025.
//

#include "characters/BaseCharacter.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

#include "GameObjects/Component/KeyInputComponent.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "Input/InputSystem.h"
#include "Physics/Box2DFacade.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"

BaseCharacter::BaseCharacter(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                             bool activePlayer, KeyBindings bindings): Broadcastable(this) {
    initializeCharacter(getId(), network, eventManager, engine, activePlayer, bindings);
}

BaseCharacter::BaseCharacter(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
                             GameEngine *engine, bool activePlayer, KeyBindings bindings) : Broadcastable(this, parentId) {
    initializeCharacter(parentId, network, eventManager, engine, activePlayer, bindings);
}

void BaseCharacter::initializeCharacter(int id, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
                                        GameEngine *engine, bool activePlayer, KeyBindings bindings) {
    _controller = std::make_unique<BaseCharacterController>(network, id, eventManager, bindings, activePlayer);

    if (activePlayer) {
        auto keyInput = std::make_unique<KeyInputComponent>(this);
        keyInput->setListener(_controller.get());
        engine->getSystem<InputSystem>()->registerKeyComponent(keyInput.get());
        addComponent(std::move(keyInput));
    }

    getTransform()->getSize()->setWidth(50);
    getTransform()->getSize()->setHeight(100);

    std::unique_ptr<PhysicsComponent> component = std::make_unique<PhysicsComponent>(
        engine->getSystem<PhysicsSystem>()->getBox2DFacade());
    component->setBodyType(BodyType::DYNAMIC);
    component->setCollider(std::make_unique<BoxCollider>(50, 100));

    component->setMaterial(Material(1.0f, 0.0f, 0.0f));
    component->setGravityScale(1.0f);
    component->setFixedRotation(true);

    PhysicsComponent *componentPointer = component.get();
    addComponent(std::move(component));
    engine->getSystem<PhysicsSystem>()->registerComponent(componentPointer);
}

void BaseCharacter::update(float delta) {

    GameObject::update(delta);

    if (_controller) {
        _controller->update(delta);
    }

    if (_controller && _controller->isActive()) {
        auto *physics = getComponent<PhysicsComponent>();
        if (physics) {
            _controller->move(Direction::NONE, physics);
        }

        // Check if should leave ground
        float vx, vy;
        physics->getVelocity(vx, vy);
        if (_controller->isGrounded() && vy > 1.0f) {
            _controller->setGrounded(false);
        }
    }
    updateAnimation();
}

void BaseCharacter::onCollisionEnter(const CollisionData &collision) {
    if (collision.normalY > 0.2f) {
        if (_controller) {

            if (!_controller->isGrounded()) {
                // std::cout << "[BaseCharacter] Landed!" << std::endl;
            }
            _controller->setGrounded(true);
        }
    }
}

void BaseCharacter::onCollisionExit(const CollisionData &) {
    PhysicsComponent *physics = getComponent<PhysicsComponent>();
    if (!physics || !_controller) return;

    float vx, vy;
    physics->getVelocity(vx, vy);

    if (vy < -0.1f) {
        _controller->setGrounded(false);
    }
}

void BaseCharacter::setIdleSpritesheet(std::string idle) { this->idle = idle; }
void BaseCharacter::setMovingLeftSpritesheet(std::string left) { this->left = left; }
void BaseCharacter::setMovingRightSpritesheet(std::string right) { this->right = right; }
void BaseCharacter::setJumpingSpritesheet(std::string jump) { this->jump = jump; }
void BaseCharacter::setFallingSpritesheet(std::string falling) { this->falling = falling; }

std::string BaseCharacter::getJumpingSpritesheet() const { return jump; }
std::string BaseCharacter::getLeftSpritesheet() const { return left; }
std::string BaseCharacter::getRightSpritesheet() const { return right; }
std::string BaseCharacter::getIdleSpritesheet() const { return idle; }
std::string BaseCharacter::getFallingSpritesheet() const { return falling; }

void BaseCharacter::updateAnimator(Animation newAnimation) {
    if (_currentAnimation == newAnimation) {
        return;
    }

    _currentAnimation = newAnimation;
    removeComponent<Animator>(true);

    switch (newAnimation) {
        case Animation::IDLE:
            addComponent(std::make_unique<Animator>(idle, 1, 5));
            break;
        case Animation::LEFT:
            addComponent(std::make_unique<Animator>(left, 1, 7));
            break;
        case Animation::RIGHT:
            addComponent(std::make_unique<Animator>(right, 1, 7));
            break;
        case Animation::JUMP:
            addComponent(std::make_unique<Animator>(jump, 1, 4));
            break;
        case Animation::FALLING:
            addComponent(std::make_unique<Animator>(falling, 1, 4));
            break;
    }
}

void BaseCharacter::setMovementDirection(Direction direction) {
    _controller->setMovementDirection(direction);
}

void BaseCharacter::updateAnimation() {
    if (!_controller) return;

    PhysicsComponent *physics = getComponent<PhysicsComponent>();
    if (!physics) return;

    float vx, vy;
    physics->getVelocity(vx, vy);

    bool isGrounded = _controller->isGrounded();
    Direction movementDir = _controller->getMovementDirection();

    bool effectivelyGrounded = isGrounded || (std::abs(vy) < 0.5f);

    if (!effectivelyGrounded) {
        if (vy < -0.1f) {
            updateAnimator(Animation::JUMP);
        } else if (vy > 0.5f) {
            updateAnimator(Animation::FALLING);
        } else {

            if (movementDir != Direction::NONE) {

                if(movementDir == Direction::WEST) updateAnimator(Animation::RIGHT);
                 else updateAnimator(Animation::LEFT);
            } else {
                updateAnimator(Animation::IDLE);
            }
        }
        return;
    }

    if (movementDir == Direction::WEST) {
        updateAnimator(Animation::RIGHT);
        return;
    }

    if (movementDir == Direction::EAST) {
        updateAnimator(Animation::LEFT);
        return;
    }

    updateAnimator(Animation::IDLE);
}