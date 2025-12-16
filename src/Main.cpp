#include <iostream>
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "LevelSelector.h"

int main() {
    try {
        std::unique_ptr<GameEngine> engine = std::make_unique<GameEngine>();
        engine->init("Vuurjongen en Watermeisje", 800, 600);

        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
        LevelSelector levelSelector(sceneSystem);

        levelSelector.createLevelSelectorScene();
        levelSelector.createLevelScenes();

        sceneSystem->setScene("level_selector");

        engine->start();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
