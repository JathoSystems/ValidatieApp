//
// Created by jusra on 16-12-2025.
//

#include "scenes/Game.hpp"

#include "UI/Text.h"

Game::Game() : Scene("Game") {
    auto object = std::make_unique<GameObject>();
    object->getTransform()->getSize()->setWidth(100);
    object->getTransform()->getSize()->setHeight(50);

    auto text = std::make_unique<Text>("In game!");
    object->addComponent(std::move(text));

    addObject(std::move(object));
}
