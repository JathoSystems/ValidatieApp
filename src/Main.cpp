#include <iostream>
#include "Engine/GameEngine.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "LevelCreator.h"
#include "Physics/PhysicsSystem.h" // Nodig voor de pointer, maar we gebruiken het niet echt

int main() {
    try {
        std::cout << "=== Initializing Level Viewer ===" << std::endl;

        GameEngine gameEngine;
        gameEngine.init("Fireboy & Watergirl - Level Viewer", 800, 600);

        auto physicsFacade = gameEngine.getSystem<PhysicsSystem>();

        std::cout << "=== Creating Scene ===" << std::endl;
        auto mainScene = std::make_unique<Scene>("MainScene");

        // Camera instellen
        auto viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
        auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(0, 0));
        mainScene->setCamera(std::move(camera));

        std::cout << "Creating level..." << std::endl;
        LevelCreator levelCreator;
        levelCreator.initLevel(mainScene.get(), physicsFacade ? physicsFacade->getBox2DFacade() : nullptr);

        levelCreator.createFromTextMap("Levels/level_1.txt");
        levelCreator.renderLevel();
        levelCreator.registerGrid("MainScene");

        std::cout << "Level rendered with " << mainScene->getObjects().size() << " objects" << std::endl;

        SceneSystem* sceneSystem = gameEngine.getSystem<SceneSystem>();
        if (sceneSystem) {
            sceneSystem->addScene(std::move(mainScene));
            sceneSystem->setScene("MainScene");
        }

        std::cout << "=== Starting Viewer ===" << std::endl;
        gameEngine.start();

    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}