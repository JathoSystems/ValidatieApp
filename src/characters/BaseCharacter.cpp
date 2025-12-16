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

// [ToDo] fix duplicates
BaseCharacter::BaseCharacter(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                             bool activePlayer) {
    _controller = std::make_unique<BaseCharacterController>(network, getId(), eventManager);
    if (activePlayer) {
        auto keyInput = std::make_unique<KeyInputComponent>(this);
        keyInput->setListener(_controller.get());
        engine->getSystem<InputSystem>()->registerKeyComponent(keyInput.get());
        addComponent(std::move(keyInput));
    }

    addComponent(std::make_unique<Animator>("resources/fireboy/idle.png", 1, 5));

    float width = 50.0f;
    float height = 100.0f;

    getTransform()->getPosition()->setX(260);
    getTransform()->getPosition()->setY(300.0f);
    getTransform()->getSize()->setWidth(width);
    getTransform()->getSize()->setHeight(height);

    std::unique_ptr<PhysicsComponent> component = std::make_unique<PhysicsComponent>(
        engine->getSystem<PhysicsSystem>()->getBox2DFacade());
    component->setBodyType(BodyType::DYNAMIC);
    component->setCollider(std::make_unique<BoxCollider>());
    component->setMaterial(Material(1.0f, 0.8f, 0.0f));
    component->setGravityScale(1.0f);
    component->setFixedRotation(true);
    component->setParent(this);

    PhysicsComponent *componentPointer = component.get();
    addComponent(std::move(component));
    engine->getSystem<PhysicsSystem>()->registerComponent(componentPointer);
}

BaseCharacter::BaseCharacter(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
                             GameEngine *engine, bool activePlayer) : GameObject(parentId) {
    _controller = std::make_unique<BaseCharacterController>(network, parentId, eventManager);

    if (activePlayer) {
        auto keyInput = std::make_unique<KeyInputComponent>(this);
        keyInput->setListener(_controller.get());
        engine->getSystem<InputSystem>()->registerKeyComponent(keyInput.get());
        addComponent(std::move(keyInput));
    }

    addComponent(std::make_unique<Animator>("resources/fireboy/idle.png", 1, 5));
    
    float width = 50.0f;
    float height = 100.0f;
    
    getTransform()->getPosition()->setX(260);
    getTransform()->getPosition()->setY(300.0f);
    getTransform()->getSize()->setWidth(width);
    getTransform()->getSize()->setHeight(height);

    std::unique_ptr<PhysicsComponent> component = std::make_unique<PhysicsComponent>(
        engine->getSystem<PhysicsSystem>()->getBox2DFacade());
    component->setBodyType(BodyType::DYNAMIC);
    // Collider will automatically use Transform size (width, height)
    component->setCollider(std::make_unique<BoxCollider>());
    component->setMaterial(Material(1.0f, 0.8f, 0.0f));
    component->setGravityScale(1.0f);
    component->setFixedRotation(true);
    component->setParent(this);

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
    if (collision.normalY > 0.5f) {
        if (_controller) {
            _controller->setGrounded(true);
        }
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

    const float velocityThreshold = 10.0f;

    // 1. Check of karakter in de lucht is
    if (!isGrounded) {
        if (vy < -velocityThreshold) {
            // Omhoog (springen)
            updateAnimator(Animation::JUMP);
        } else if (vy > velocityThreshold) {
            // Omlaag (vallen)
            updateAnimator(Animation::FALLING);
        } else {
            // In de lucht maar kleine velocity
            if (vy < 0) {
                updateAnimator(Animation::JUMP);
            } else {
                updateAnimator(Animation::FALLING);
            }
        }
        return; // Lucht animaties hebben voorrang
    }

    // 2. Check beweging (alleen als op grond)
    if (movementDir == Direction::WEST) {
        updateAnimator(Animation::LEFT);
        return;
    }

    if (movementDir == Direction::EAST) {
        updateAnimator(Animation::RIGHT);
        return;
    }

    // 3. Idle als op grond en niet bewegen
    updateAnimator(Animation::IDLE);
}
