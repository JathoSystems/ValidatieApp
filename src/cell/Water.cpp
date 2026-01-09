//
// Created by jusra on 5-1-2026.
//

#include "cell/Water.hpp"

#include <memory>

#include "grid/LevelGrid.h"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/Collider.h"
#include "characters/Fireboy.hpp"
#include "GameObjects/Component/AudioComponent.h"
#include "Network/GameState.hpp"
#include "scenes/RestartScene.hpp"
#include "Scenes/SceneSystem.h"

Water::Water(LevelGrid* grid, int x, int y)
    : Liquid(
        grid,
        x,
        y,
        "resources/fluids/water/water_left.jpg",
        "resources/fluids/water/water_right.jpg",
        "resources/fluids/water/water_middle.jpg"
    )
{
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

void Water::onCollisionEnter(const CollisionData &collision) {
    // Water kills Fireboy, not Watergirl
    if (dynamic_cast<Fireboy *>(collision.other)) {
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
        }
    }
}
