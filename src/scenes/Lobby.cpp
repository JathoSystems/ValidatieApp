//
// Created by jusra on 16-12-2025.
//

#include "scenes/Lobby.hpp"

#include "UI/Text.h"

Lobby::Lobby() : Scene("Lobby") {
    auto object = std::make_unique<GameObject>();
    object->getTransform()->getSize()->setWidth(100);
    object->getTransform()->getSize()->setHeight(50);

    auto text = std::make_unique<Text>("In lobby");
    object->addComponent(std::move(text));

    addObject(std::move(object));
}
