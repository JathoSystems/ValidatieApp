//
// Created by jusra on 9-12-2025.
//

#include "characters/BaseCharacter.hpp"

#include "characters/movement/MovementComponent.hpp"
#include "GameObjects/Component/SpriteRenderer.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "Physics/Box2DFacade.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/RigidBody.h"

BaseCharacter::BaseCharacter(GameEngine *engine, bool activePlayer) {
    if (activePlayer) {
        std::unique_ptr<MovementComponent> movement = std::make_unique<MovementComponent>(
            this, engine, Key::W, Key::S, Key::A, Key::D);

        movement->onUp([this]() {
            this->removeComponent<Animator>(true);
            this->addComponent(std::make_unique<Animator>(jump, 1, 4));
        });

        movement->onDown([this]() {
            this->removeComponent<Animator>(true);
            this->addComponent(std::make_unique<Animator>(falling, 1, 4));
        });

        movement->onLeft([this]() {
            this->removeComponent<Animator>(true);
            this->addComponent(std::make_unique<Animator>(left, 1, 7));
        });

        movement->onRight([this]() {
            this->removeComponent<Animator>(true);
            this->addComponent(std::make_unique<Animator>(right, 1, 7));
        });

        movement->onIdle([this]() {
            this->removeComponent<Animator>(true);
            this->addComponent(std::make_unique<Animator>(idle, 1, 5));
        });

        addComponent(std::move(movement));
    }
    getTransform()->getPosition()->setX(1);
    getTransform()->getPosition()->setY(1);

    Box2DFacade *facade = new Box2DFacade();
    facade->init(300, 300);

    std::unique_ptr<Material> material = std::make_unique<Material>(5, 0.1, 0);
    Material* mat = material.get();

    std::unique_ptr<PhysicsComponent> component = std::make_unique<PhysicsComponent>(facade);
    component->setBodyType(BodyType::KINEMATIC);
    component->setGravityScale(300);
    component->setFixedRotation(0);
    component->applyForce(3, 1);
    component->setMaterial(*mat);
    component->setCollider(std::make_unique<BoxCollider>(100, 100));

    PhysicsComponent *componentPointer = component.get();
    addComponent(std::move(component));
    engine->getSystem<PhysicsSystem>()->registerComponent(componentPointer);
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
    addComponent(std::make_unique<Animator>(jump, 1, 4));
}

void BaseCharacter::setFallingSpritesheet(std::string falling) {
    this->falling = falling;
}
