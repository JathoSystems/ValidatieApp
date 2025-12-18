//
// Created by jusra on 9-12-2025.
//

#include "characters/BaseCharacter.hpp"

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
    _controller = std::make_unique<BaseCharacterController>(network, id, eventManager, bindings);

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
    component->setMaterial(Material(1.0f, 0.8f, 0.0f));
    component->setGravityScale(1.0f);
    component->setFixedRotation(true);

    PhysicsComponent *componentPointer = component.get();
    addComponent(std::move(component));
    engine->getSystem<PhysicsSystem>()->registerComponent(componentPointer);
}

void BaseCharacter::setMovementDirection(Direction direction) {
    _direction = direction;
}

void BaseCharacter::update(float delta) {
    GameObject::update(delta);
    updateAnimation();

    if (_controller) {
        auto* physics = getComponent<PhysicsComponent>();
        if (physics) {
            _controller->move(_direction, physics);
        }
    }
}

void BaseCharacter::onCollisionEnter(const CollisionData &collision) {

    if (collision.normalY > 0.2f) {

        if (_controller) {
            _controller->setGrounded(true);
        }

        removeComponent<Animator>(true);
        addComponent(std::make_unique<Animator>(idle, 1, 5));
    }
}


void BaseCharacter::onCollisionExit(const CollisionData &) {
    if (_controller)
        _controller->setGrounded(false);
}

void BaseCharacter::setIdleSpritesheet(std::string idle) {
    this->idle = idle;
}

void BaseCharacter::setMovingLeftSpritesheet(std::string left) {
    this->left = left;
}

void BaseCharacter::setMovingRightSpritesheet(std::string right) {
    this->right = right;
}

void BaseCharacter::setJumpingSpritesheet(std::string jump) {
    this->jump = jump;
}

void BaseCharacter::setFallingSpritesheet(std::string falling) {
    this->falling = falling;
}

std::string BaseCharacter::getJumpingSpritesheet() const {
    return jump;
}

std::string BaseCharacter::getLeftSpritesheet() const {
    return left;
}

std::string BaseCharacter::getRightSpritesheet() const {
    return right;
}

std::string BaseCharacter::getIdleSpritesheet() const {
    return idle;
}

std::string BaseCharacter::getFallingSpritesheet() const {
    return falling;
}

void BaseCharacter::updateAnimator(Animation newAnimation) {
    if (_currentAnimation == newAnimation) {
        return; // Al de juiste animatie, skip
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

void BaseCharacter::updateAnimation() {
    if (!_controller) return;

    PhysicsComponent *physics = getComponent<PhysicsComponent>();
    if (!physics) return;

    float vx, vy;
    physics->getVelocity(vx, vy);
    bool isGrounded = _controller->isGrounded();
    Direction movementDir = _controller->getMovementDirection();

    // Animatie prioriteit: Spring/Val > Beweging > Idle

    // 1. Check of karakter in de lucht is
    if (!isGrounded) {
        if (vy < 0) {
            // Omhoog (springen)
            updateAnimator(Animation::JUMP);
        } else {
            // Omlaag (vallen)
            updateAnimator(Animation::FALLING);
        }
        return; // Lucht animaties hebben voorrang
    }

    // 2. Check beweging (alleen als op grond)
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
