//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Network/NetworkSystem.h"
#include "Scenes/SceneSystem.h"
#include "GameObjects/ObjectRegistry.hpp"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "QUIT PACKET RECEIVED" << std::endl;
        
        auto* sceneSystem = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!sceneSystem) return;
        
        // Get current scene name before switching
        std::string currentSceneName;
        Scene* currentScene = sceneSystem->getActiveSceneObj();
        if (currentScene) {
            currentSceneName = currentScene->getName();
        }
        
        // Clear packet queue before scene change
        NetworkSystem* networkSystem = GameEngine::getInstance().getSystem<NetworkSystem>();
        if (networkSystem && networkSystem->getMiddleware()) {
            networkSystem->getMiddleware()->clearPacketQueue();
        }
        // NOTE: Don't clear ObjectRegistry - let Broadcastable destructors handle it
        
        // Switch to main menu
        sceneSystem->setScene("MainMenu");
        
        // Remove the old level scene to properly destroy GameObjects
        if (!currentSceneName.empty() && currentSceneName.find("level_") == 0) {
            sceneSystem->removeScene(currentSceneName);
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP