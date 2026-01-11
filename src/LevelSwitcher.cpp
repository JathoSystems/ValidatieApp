//
// Created by jusra on 7-1-2026.
//

#include "LevelSwitcher.hpp"

#include <string>

#include "scenes/LevelScene.hpp"
#include "scenes/RoomSelectionScene.hpp"
#include "GameObjects/ObjectRegistry.hpp"

extern std::map<int, std::function<std::unique_ptr<Scene>()>> g_levels;

void LevelSwitcher::openLevel(int level, bool online) {
    GameEngine *engine = &GameEngine::getInstance();
    SceneSystem* sceneSystem = engine->getSystem<SceneSystem>();

    if (!sceneSystem) {
        std::cerr << "SceneSystem not found!\n";
        return;
    }

    // Clear the packet queue BEFORE changing scenes to prevent stale packets
    // from accessing destroyed objects
    if (_network && _network->getMiddleware()) {
        _network->getMiddleware()->clearPacketQueue();
    }
    
    // NOTE: Don't clear ObjectRegistry here - let Broadcastable destructors
    // handle cleanup naturally when the old scene is destroyed. Clearing here
    // can cause issues because new scene objects get registered, then old
    // scene destruction tries to remove by same IDs.

    std::string levelSceneName = "level_" + std::to_string(level) + (online ? "_online" : "");

    Scene* currentScene = sceneSystem->getActiveSceneObj();
    if (currentScene) {
        LevelScene* currentLevelScene = dynamic_cast<LevelScene*>(currentScene);
        if (currentLevelScene) {
            std::cout << "[LevelSwitcher] Cleaning up current level scene before switching..." << std::endl;
            currentLevelScene->cleanup();
        }
    }

    auto newLevelScene = g_levels[level]();
    sceneSystem->addScene(std::move(newLevelScene));
    sceneSystem->setScene(levelSceneName);

    // if (!currentSceneName.empty() && currentSceneName != levelSceneName) {
    //     sceneSystem->removeScene(currentSceneName);
    // }

    std::cout << "Switched to level scene: " << levelSceneName << "\n";
}
