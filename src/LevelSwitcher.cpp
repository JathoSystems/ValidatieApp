//
// Created by jusra on 7-1-2026.
//

#include "LevelSwitcher.hpp"

#include <string>

#include "scenes/LevelScene.hpp"
#include "scenes/RoomSelectionScene.hpp"

void LevelSwitcher::openLevel(int level, bool online) {
    GameEngine *engine = &GameEngine::getInstance();
    SceneSystem* sceneSystem = engine->getSystem<SceneSystem>();

    if (!sceneSystem) {
        std::cerr << "SceneSystem not found!\n";
        return;
    }

    std::string levelSceneName = "level_" + std::to_string(level) + (online ? "_online" : "");

    auto newLevelScene = std::make_unique<LevelScene>(
        level,
        online,
        _network,
        _eventManager
    );

    sceneSystem->addScene(std::move(newLevelScene));
    sceneSystem->setScene(levelSceneName);

    std::cout << "Switched to level scene: " << levelSceneName << "\n";
}
