#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <memory>
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"

class LevelSelector {
private:
    SceneSystem *_sceneSystem;

public:
    explicit LevelSelector(SceneSystem *sceneSystem);

    void createLevelSelectorScene();

    void createLevelScenes();

private:
    std::unique_ptr<Scene> createLevel(int levelNumber);

    std::unique_ptr<GameObject> createLevelButton(int levelNumber, float x, float y);
};

#endif
