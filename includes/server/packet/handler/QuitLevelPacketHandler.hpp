//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP

#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Scenes/SceneSystem.h"
#include "Network/GameState.hpp"
#include "scenes/LevelScene.hpp"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "\n========================================" << std::endl;
        std::cout << "[QuitLevelPacketHandler] QUIT PACKET RECEIVED" << std::endl;
        std::cout << "========================================" << std::endl;

        auto* engine = &GameEngine::getInstance();
        auto* sceneSystem = engine->getSystem<SceneSystem>();

        Scene* currentScene = sceneSystem->getActiveSceneObj();
        std::string currentSceneName = currentScene ? currentScene->getName() : "";

        std::cout << "[QuitLevelPacketHandler] Current scene: " << currentSceneName << std::endl;

        // 1. Find and cleanup ALL level scenes (both online and offline)
        std::vector<std::string> scenesToRemove;

        // Check current scene
        if (currentSceneName.find("level_") != std::string::npos) {
            scenesToRemove.push_back(currentSceneName);
        }

        // Also check for any other level scenes that might exist
        // Pattern: "level_X" or "level_X_online"
        for (int i = 1; i <= 10; i++) {
            std::string offlineName = "level_" + std::to_string(i);
            std::string onlineName = "level_" + std::to_string(i) + "_online";

            if (sceneSystem->getScene(offlineName)) {
                scenesToRemove.push_back(offlineName);
            }
            if (sceneSystem->getScene(onlineName)) {
                scenesToRemove.push_back(onlineName);
            }
        }

        // 2. Cleanup each level scene properly
        for (const std::string& sceneName : scenesToRemove) {
            Scene* scene = sceneSystem->getScene(sceneName);
            if (auto* levelScene = dynamic_cast<LevelScene*>(scene)) {
                std::cout << "[QuitLevelPacketHandler] Cleaning up: " << sceneName << std::endl;
                levelScene->cleanup();
            }
            sceneSystem->removeScene(sceneName);
        }

        // 3. Clear all game state
        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");
        std::cout << "[QuitLevelPacketHandler] Cleared GameState" << std::endl;

        // 4. Go to main menu
        sceneSystem->setScene("MainMenu");

        std::cout << "[QuitLevelPacketHandler] Cleanup complete, switched to MainMenu" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP