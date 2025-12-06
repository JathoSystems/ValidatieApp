//
// Created by jusra on 5-12-2025.
//

#include "characters/Fireboy.hpp"
#include "characters/events/MoveEvent.hpp"
#include "Events/EventRegistry.h"

Fireboy::Fireboy(GameEngine *engine) {
    addComponent(std::make_unique<Animator>("../resources/dropping.png", 1, 4));
    addComponent(std::make_unique<MovementComponent>(
        this, engine, Key::W, Key::S, Key::A, Key::D
    ));

    getTransform()->getPosition()->setX(1);
    getTransform()->getPosition()->setY(1);
}
