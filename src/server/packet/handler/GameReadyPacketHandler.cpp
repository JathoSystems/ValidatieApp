#include "server/packet/handler/GameReadyPacketHandler.hpp"
#include "server/packet/GameReady.hpp"
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "Network/NetworkSystem.h"
#include "Events/EventManager.h"
#include "LevelSelector.h"
#include "scenes/levels/Level1Scene.hpp"
#include "scenes/levels/Level2Scene.hpp"
#include "scenes/levels/Level3Scene.hpp"

#include <mutex>
#include <vector>
#include <functional>
#include <iostream>

extern std::mutex eventMutex;
extern std::vector<std::function<void()> > eventQueue;
extern std::map<int, std::function<std::unique_ptr<Scene>()> > g_levels;

// Static storage for network and event manager (set from Main.cpp)
static std::shared_ptr<NetworkSystem> g_network = nullptr;
static EventManager* g_eventManager = nullptr;

void GameReadyPacketHandler::setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network, EventManager* eventManager) {
    g_network = network;
    g_eventManager = eventManager;
}

void GameReadyPacketHandler::handle(const Packet &packet) {
    GameReadyPacket gameReady;
    gameReady.getBuffer().setData(packet.getBuffer().getData());
    gameReady.deserialize();

    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (sceneSystem) {
        std::cout << "[GameReadyPacketHandler] Starting game for level " << gameReady.levelId << std::endl;

        std::string sceneName = "level_" + std::to_string(gameReady.levelId) + "_online";

        auto network = g_network;
        auto eventManager = g_eventManager;
        int levelId = gameReady.levelId;

        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push_back([sceneSystem, sceneName, levelId, network, eventManager]() {
            // Check if level scene already exists
            Scene* existingLevel = sceneSystem->getScene(sceneName);
            if (!existingLevel) {
                std::unique_ptr<LevelScene> newLevelScene;
                // Create the appropriate online level scene based on levelId
                switch (levelId) {
                    case 1:
                        newLevelScene = std::make_unique<Level1Scene>(true, network, eventManager);
                        break;
                    case 2:
                        newLevelScene = std::make_unique<Level2Scene>(true, network, eventManager);
                        break;
                    case 3:
                        newLevelScene = std::make_unique<Level3Scene>(true, network, eventManager);
                        break;
                    default:
                        std::cerr << "[GameReadyPacketHandler] Unknown level ID: " << levelId << std::endl;
                        return;
                }
                if (newLevelScene) {
                    sceneSystem->addScene(std::move(newLevelScene));
                }
            }
            // Set the scene
            sceneSystem->setScene(sceneName);
        });
    }
}
