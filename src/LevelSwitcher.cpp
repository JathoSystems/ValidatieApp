//
// Created by jusra on 7-1-2026.
//

#include "LevelSwitcher.hpp"

#include <string>

#include "scenes/LevelScene.hpp"

extern std::map<int, std::function<std::unique_ptr<Scene>()>> g_levels;

void LevelSwitcher::openLevel(int level, bool online) {
    GameEngine *engine = &GameEngine::getInstance();
    SceneSystem* sceneSystem = engine->getSystem<SceneSystem>();

    if (!sceneSystem) {
        std::cerr << "SceneSystem not found!\n";
        return;
    }

    std::string levelSceneName = "level_" + std::to_string(level) + (online ? "_online" : "");

    auto newLevelScene = g_levels[level]();
    sceneSystem->addScene(std::move(newLevelScene));
    sceneSystem->setScene(newLevelScene->getName());

    std::cout << "Switched to level scene: " << levelSceneName << "\n";
}
