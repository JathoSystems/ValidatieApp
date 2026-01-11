#include "server/packet/handler/GameReadyPacketHandler.hpp"
#include "server/packet/GameReady.hpp"
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "Network/NetworkSystem.h"
#include "LevelSelector.h"

#include <iostream>

extern std::map<int, std::function<std::unique_ptr<Scene>()> > g_levels;

void GameReadyPacketHandler::handle(const Packet &packet) {
    GameReadyPacket gameReady;
    gameReady.getBuffer().setData(packet.getBuffer().getData());
    gameReady.deserialize();

    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (sceneSystem) {
        std::cout << "[GameReadyPacketHandler] Starting game for level " << gameReady.levelId << std::endl;

        if (g_levels.find(gameReady.levelId) == g_levels.end()) {
            std::cerr << "[GameReadyPacketHandler] Level " << gameReady.levelId << " not found in g_levels!" <<
                    std::endl;
            return;
        }

        std::string sceneName = "level_" + std::to_string(gameReady.levelId) + "_online";
        sceneSystem->setScene(sceneName);
    }
}
