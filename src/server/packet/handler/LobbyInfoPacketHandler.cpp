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
    
    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();
    
    if (sceneSystem) {
        // Get the active Lobby scene - it should exist (created in Main.cpp)
        Scene* activeScene = sceneSystem->getActiveSceneObj();
        if (activeScene) {
            Lobby* lobby = dynamic_cast<Lobby*>(activeScene);
            // Also try to get from scene name if active scene is not lobby
            if (!lobby) {
                // Try to find lobby scene by checking scene name when we set it
                // For now, we'll create/update the level scene and set lobby scene
                sceneSystem->setScene("Lobby");
                activeScene = sceneSystem->getActiveSceneObj();
                lobby = dynamic_cast<Lobby*>(activeScene);
            }
            
            if (lobby) {
                lobby->setLobbyInfo(lobbyInfo.lobbyId, lobbyInfo.levelId, lobbyInfo.playerCount);
                lobby->updateStatus(lobbyInfo.status);
                
                // Create the level scene for when game starts (if not already created)
                std::string levelSceneName = "level_" + std::to_string(lobbyInfo.levelId) + "_online";
                if (g_network && g_eventManager) {
                    // Just create it - setScene will handle if it already exists
                    auto newLevelScene = std::make_unique<LevelScene>(lobbyInfo.levelId, true, g_network, g_eventManager);
                    newLevelScene->initialize();
                    sceneSystem->addScene(std::move(newLevelScene));
                }
            }
        }
        
        // Switch to lobby scene
        sceneSystem->setScene("Lobby");
    }
}
