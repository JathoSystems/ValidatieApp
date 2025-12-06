
//
// Created by jusra on 5-12-2025.
//

#include "characters/Watergirl.hpp"

#include "characters/movement/MovementComponent.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"

Watergirl::Watergirl(GameEngine *engine) {
    addComponent(std::make_unique<MovementComponent>(this, engine, Key::UP, Key::DOWN, Key::LEFT, Key::RIGHT));
    addComponent(
        std::make_unique<SpriteRenderer>("C:\\Users\\jusra\\CLionProjects\\ValidatieApp\\resources\\sprite2.png"));
    getTransform()->getPosition()->setX(1);
    getTransform()->getPosition()->setY(1);
}
