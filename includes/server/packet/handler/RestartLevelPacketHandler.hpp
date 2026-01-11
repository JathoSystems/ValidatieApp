//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "scenes/LevelScene.hpp"
#include "server/packet/RestartPacket.hpp"
#include "Scenes/SceneSystem.h"

class RestartLevelPacketHandler : public IPacketHandler {
    void handle(const Packet &packet) override {
        std::cout << "[RestartLevelPacketHandler] Restart packet received\n";
        // Deserialize the packet to get the level name
        RestartPacket restartPacket;
        restartPacket.getBuffer().setData(packet.getBuffer().getData());
        restartPacket.deserialize();
        
        std::string sceneName = restartPacket.getLevel();
        std::cout << "[RestartLevelPacketHandler] Switching to level: " << sceneName << std::endl;
        
        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
        
        if (!sceneSystem) {
            std::cerr << "[RestartLevelPacketHandler] SceneSystem is null!" << std::endl;
            return;
        }
        
        // Get the current scene and clean it up if it's a LevelScene
        Scene* currentScene = sceneSystem->getActiveSceneObj();
        LevelScene* currentLevelScene = nullptr;
        if (currentScene) {
            std::cout << "[RestartLevelPacketHandler] Current scene: " << currentScene->getName() << std::endl;
            currentLevelScene = dynamic_cast<LevelScene*>(currentScene);
            if (currentLevelScene) {
                std::cout << "[RestartLevelPacketHandler] Cleaning up current level scene before restart..." << std::endl;
                currentLevelScene->cleanup();
            }
        }
        
        // Handle different scenarios:
        // 1. If we're on RestartScene, switch directly to the level (client 2 case)
        // 2. If we're already on the target level scene, switch away first to force reinitialization
        // 3. Otherwise, just switch to the level scene
        if (currentScene) {
            std::string currentSceneName = currentScene->getName();
            if (currentSceneName == "Restart") {
                std::cout << "[RestartLevelPacketHandler] Switching from RestartScene to level scene: " << sceneName << std::endl;
                // Directly switch to level - this will trigger onInitialRender
                sceneSystem->setScene(sceneName);
                return; // Early return since we've already switched
            } else if (currentSceneName == sceneName) {
                std::cout << "[RestartLevelPacketHandler] Forcing scene reinitialization by switching away and back..." << std::endl;
                // Switch away first to force reinitialization
                sceneSystem->setScene("Restart");
            }
        }
        
        // Switch to the level scene - this will trigger onInitialRender
        // which will properly reinitialize both players in sync (same as initial multiplayer setup)
        std::cout << "[RestartLevelPacketHandler] Setting scene to: " << sceneName << std::endl;
        sceneSystem->setScene(sceneName);
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP