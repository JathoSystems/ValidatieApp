//
// Created by jusra on 5-1-2026.
//

#include "cell/Lava.hpp"

#include "grid/LevelGrid.h"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "GameObjects/Component/AudioComponent.h"
#include "Network/GameState.hpp"
#include "Physics/PhysicsSystem.h"
#include "scenes/RestartScene.hpp"
#include "Scenes/SceneSystem.h"

Lava::Lava(LevelGrid *grid, int x, int y)
    : Liquid(
        grid,
        x,
        y,
        "resources/fluids/fire/fire_left.jpg",
        "resources/fluids/fire/fire_right.jpg",
        "resources/fluids/fire/fire_middle.jpg"
    ) {
    GameEngine *gameEngine = &GameEngine::getInstance();
    PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
    int cellSize = grid->getCellSize();

    auto physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    physics->setBodyType(BodyType::STATIC);
    physics->setCollider(std::make_unique<BoxCollider>(cellSize, cellSize));
    physics->setMaterial(Material(1.0f, 0.8f, 0.0f));

    PhysicsComponent *physicsPtr = physics.get();
    addComponent(std::move(physics));
    physicsSystem->registerComponent(physicsPtr);
}

void Lava::onCollisionEnter(const CollisionData &collision) {
    // Lava kills Watergirl, not Fireboy
    if (dynamic_cast<Watergirl *>(collision.other)) {
        AudioSystem *audioSystem = GameEngine::getInstance().getSystem<AudioSystem>();
        audioSystem->initialize();
        audioSystem->loadSound("death", "resources/death.mp3");
        audioSystem->playSound("death");

        GameEngine *gameEngine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = gameEngine->getSystem<SceneSystem>();

        std::string previousSceneName = sceneSystem->getActiveSceneObj()->getName();
        if (previousSceneName != "Restart") {
            std::cout << "Current scene" << previousSceneName << std::endl;

            sceneSystem->setScene("Restart");

            Scene *scene = sceneSystem->getActiveSceneObj();
            RestartScene *restartScene = dynamic_cast<RestartScene *>(scene);

            if (restartScene) {
                restartScene->setTargetLevel(previousSceneName);
                restartScene->setOnline((GameState::getInstance().get("lobby", "nope") != "nope"));
            }

            if (previousSceneName.find("level_") == 0) {
                sceneSystem->removeScene(previousSceneName);
            }
        }
    }
}
