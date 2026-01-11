//
// Created for lobby system
//

#include "server/packet/handler/LobbyInfoPacketHandler.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "scenes/Lobby.hpp"
#include "scenes/LevelScene.hpp"
#include "scenes/levels/Level1Scene.hpp"
#include "scenes/levels/Level2Scene.hpp"
#include "scenes/levels/Level3Scene.hpp"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include <mutex>
#include <vector>
#include <functional>

// External event queue for thread-safe scene changes
extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

// Static storage for network and event manager (set from Main.cpp)
static std::shared_ptr<NetworkSystem> g_network = nullptr;
static EventManager* g_eventManager = nullptr;

void LobbyInfoPacketHandler::setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network, EventManager* eventManager) {
    g_network = network;
    g_eventManager = eventManager;
}

void LobbyInfoPacketHandler::handle(const Packet &packet) {
    LobbyInfoPacket lobbyInfo;
    lobbyInfo.getBuffer().setData(packet.getBuffer().getData());
    lobbyInfo.deserialize();

    GameState::getInstance().set("lobby", std::to_string(lobbyInfo.lobbyId));
    std::cout << "[LobbyInfoPacketHandler] Stored lobby ID in GameState: "
              << lobbyInfo.lobbyId << std::endl;

    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (sceneSystem) {
        // Queue all scene operations to main thread to avoid race conditions
        std::lock_guard<std::mutex> lock(eventMutex);

        // Store lobby info for the queued operation
        int lobbyId = lobbyInfo.lobbyId;
        int levelId = lobbyInfo.levelId;
        int playerCount = lobbyInfo.playerCount;
        std::string status = lobbyInfo.status;

        // FIXED: Use g_network instead of network
        auto network = g_network;
        auto eventManager = g_eventManager;

        eventQueue.push_back([sceneSystem, lobbyId, levelId, playerCount, status, network]() {
            // Check if Lobby scene already exists
            Scene* existingLobby = sceneSystem->getScene("Lobby");
            if (!existingLobby) {
                // Create new Lobby scene with network
                auto newLobby = std::make_unique<Lobby>(network);
                sceneSystem->addScene(std::move(newLobby));
            }

            // Set the scene
            sceneSystem->setScene("Lobby");

            // Then update the lobby info
            Scene* activeScene = sceneSystem->getActiveSceneObj();
            if (activeScene) {
                Lobby* lobby = dynamic_cast<Lobby*>(activeScene);
                if (lobby) {
                    lobby->setLobbyInfo(lobbyId, levelId, playerCount);
                    lobby->updateStatus(status);
                }
            }
        });

        // Create the level scene for when game starts (if not already created)
        std::string levelSceneName = "level_" + std::to_string(lobbyInfo.levelId) + "_online";

        eventQueue.push_back([sceneSystem, levelSceneName, levelId, network, eventManager]() {
            // Check if level scene already exists
            Scene* existingLevel = sceneSystem->getScene(levelSceneName);
            if (!existingLevel) {
                std::unique_ptr<LevelScene> newLevelScene;
                // Create the appropriate level scene based on levelId
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
                        std::cerr << "[LobbyInfoPacketHandler] Unknown level ID: " << levelId << std::endl;
                        return;
                }
                sceneSystem->addScene(std::move(newLevelScene));
            }
        });
    }
}