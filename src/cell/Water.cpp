//
// Created by jusra on 5-1-2026.
//

#include "cell/Water.hpp"

#include <memory>

#include "LevelGrid.h"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"

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
    // GameEngine *gameEngine = &GameEngine::getInstance();
    // PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
    // int cellSize = grid->getCellSize();
    //
    // auto physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    // physics->setBodyType(BodyType::STATIC);
    // physics->setCollider(std::make_unique<BoxCollider>(cellSize, cellSize));
    // physics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    //
    // PhysicsComponent *physicsPtr = physics.get();
    // addComponent(std::move(physics));
    // physicsSystem->registerComponent(physicsPtr);

    // auto sprite = std::make_unique<SpriteRenderer>("resources/square_blue.png");
    // addComponent(std::move(sprite));
}
