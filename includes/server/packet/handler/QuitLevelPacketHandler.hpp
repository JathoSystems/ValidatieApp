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
#include "server/GlobalFlags.h"
#include <thread>
#include <chrono>

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

        // 1. Set global cleaning flag to stop all physics/network updates
        GlobalFlags::isLevelCleaning = true;
        std::cout << "[QuitLevelPacketHandler] Set cleaning flag" << std::endl;

        // Small delay to let any pending updates finish
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 2. Clear all game state
        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");
        std::cout << "[QuitLevelPacketHandler] Cleared GameState" << std::endl;

        // 3. Switch to MainMenu FIRST (so we're not cleaning an active scene)
        sceneSystem->setScene("MainMenu");
        std::cout << "[QuitLevelPacketHandler] Switched to MainMenu" << std::endl;

        // Another small delay to ensure scene switch is complete
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 4. NOW cleanup and remove ALL level scenes
        std::vector<std::string> scenesToRemove;

        // Check for any level scenes that might exist
        for (int i = 1; i <= 10; i++) {
            std::string offlineName = "level_" + std::to_string(i);
            std::string onlineName = "level_" + std::to_string(i) + "_online";

            Scene* offlineScene = sceneSystem->getScene(offlineName);
            Scene* onlineScene = sceneSystem->getScene(onlineName);

            if (offlineScene) {
                scenesToRemove.push_back(offlineName);
            }
            if (onlineScene) {
                scenesToRemove.push_back(onlineName);
            }
        }

        // Cleanup each scene
        for (const std::string& sceneName : scenesToRemove) {
            Scene* scene = sceneSystem->getScene(sceneName);
            if (auto* levelScene = dynamic_cast<LevelScene*>(scene)) {
                std::cout << "[QuitLevelPacketHandler] Cleaning up: " << sceneName << std::endl;
                levelScene->cleanup();
            }
            std::cout << "[QuitLevelPacketHandler] Cleaned up: " << sceneName << std::endl;
        }

        // 5. Clear the cleaning flag
        GlobalFlags::isLevelCleaning = false;

        std::cout << "[QuitLevelPacketHandler] Cleanup complete" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP