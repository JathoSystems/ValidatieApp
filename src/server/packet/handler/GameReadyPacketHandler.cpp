//
// Created by jusra on 17-12-2025.
//

#include "server/packet/handler/GameReadyPacketHandler.hpp"
#include "server/packet/GameReady.hpp"
#include "scenes/LevelScene.hpp"
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "Network/NetworkSystem.h"
#include "Events/EventManager.h"
#include <mutex>
#include <vector>
#include <functional>

// External event queue for thread-safe scene changes
extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

void GameReadyPacketHandler::handle(const Packet &packet) {
    GameReadyPacket gameReady;
    gameReady.getBuffer().setData(packet.getBuffer().getData());
    gameReady.deserialize();
    
    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (sceneSystem) {
        std::cout << "[GameReadyPacketHandler] Starting game for level " << gameReady.levelId << std::endl;
        
        // Navigate to the level scene (should have been created by LobbyInfoPacketHandler)
        std::string sceneName = "level_" + std::to_string(gameReady.levelId) + "_online";

        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push_back([sceneSystem, sceneName]() {
            sceneSystem->setScene(sceneName);
        });
    }
}
