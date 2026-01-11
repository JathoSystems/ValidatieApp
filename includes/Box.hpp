//
// Created by jusra on 11-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_BOX_HPP
#define VUURJONGEN_WATERMEISJE_GAME_BOX_HPP
#include "Engine/GameEngine.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "grid/LevelGrid.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"

class Box : public GameObject {
public:
    Box(LevelGrid* grid, int x, int y) {
        int cellSize = grid->getCellSize();
        getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
        getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
        getTransform()->getSize()->setWidth(cellSize);
        getTransform()->getSize()->setHeight(cellSize);
        getTransform()->getRotation()->setRotation(120);
        setLayer(0);

        std::unique_ptr<SpriteRenderer> renderer = std::make_unique<SpriteRenderer>("resources/box.png");
        addComponent(std::move(renderer));

        PhysicsSystem* system = GameEngine::getInstance().getSystem<PhysicsSystem>();
        std::unique_ptr<PhysicsComponent> physics = std::make_unique<PhysicsComponent>(system->getBox2DFacade());
        physics->setFixedRotation(true);
        physics->setMaterial(Material(3.0f, 0.8f, 0.0f));
        system->registerComponent(physics.get());
        addComponent(std::move(physics));
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_BOX_HPP