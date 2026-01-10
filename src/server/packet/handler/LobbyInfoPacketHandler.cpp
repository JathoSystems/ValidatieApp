//
// Created for lobby system
//

#include "server/packet/handler/LobbyInfoPacketHandler.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "scenes/Lobby.hpp"
#include "scenes/LevelScene.hpp"
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

extern std::map<int, std::function<std::unique_ptr<Scene>()>> g_levels;

void LobbyInfoPacketHandler::setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network, EventManager* eventManager) {
    g_network = network;
    g_eventManager = eventManager;
}

void LobbyInfoPacketHandler::handle(const Packet &packet) {
    LobbyInfoPacket lobbyInfo;
    lobbyInfo.getBuffer().setData(packet.getBuffer().getData());
    lobbyInfo.deserialize();
    
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
        
        eventQueue.push_back([sceneSystem, lobbyId, levelId, playerCount, status]() {
            // Set the scene first
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
        if (g_network && g_eventManager) {
            // Copy static variables to local variables for lambda capture
            auto network = g_network;
            auto eventManager = g_eventManager;
            eventQueue.push_back([sceneSystem, levelSceneName, levelId, network, eventManager]() {
                // addScene handles duplicates, so we can just try to add it
                auto scene = g_levels[levelId]();

                if (LevelScene* lvl = dynamic_cast<LevelScene*>(scene.get()))
                    lvl->toggleOnline(network, eventManager);

                sceneSystem->addScene(std::move(scene));
            });
        }
    }
}




