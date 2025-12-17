//
// Created by jusra on 17-12-2025.
//

#include "server/packet/handler/GameReadyPacketHandler.hpp"

#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"

void GameReadyPacketHandler::handle(const Packet &packet) {
    auto gameEngine = &GameEngine::getInstance();
    auto system = gameEngine->getSystem<SceneSystem>();

    if (system)
        system->setScene("Game");
}
