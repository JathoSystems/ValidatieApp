//
// Created by jusra on 6-1-2026.
//

#include "diamond/BlueDiamond.hpp"

#include "characters/Watergirl.hpp"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Physics/PhysicsSystem.h"
#include "Engine/GameEngine.h"
#include "diamond/BlueDiamond.hpp"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Engine/GameEngine.h"

BlueDiamond::BlueDiamond(LevelGrid* grid, int x, int y) {
    _grid = grid;
    _x = x;
    _y = y;

    int cellSize = grid->getCellSize();

    getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
    getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
    getTransform()->getSize()->setWidth(cellSize);
    getTransform()->getSize()->setHeight(cellSize);
    setLayer(0);

    auto sprite = std::make_unique<SpriteRenderer>(
        "C:\\Users\\jusra\\CLionProjects\\ValidatieApp\\resources\\diamonds\\diamond_blue.png");
    addComponent(std::move(sprite));
}

void BlueDiamond::checkCollisionWith(GameObject* other) {
    if (Watergirl* watergirl = dynamic_cast<Watergirl*>(other)) {
        float x1 = getTransform()->getPosition()->getX();
        float y1 = getTransform()->getPosition()->getY();
        float w1 = getTransform()->getSize()->getWidth();
        float h1 = getTransform()->getSize()->getHeight();

        float x2 = watergirl->getTransform()->getPosition()->getX();
        float y2 = watergirl->getTransform()->getPosition()->getY();
        float w2 = watergirl->getTransform()->getSize()->getWidth();
        float h2 = watergirl->getTransform()->getSize()->getHeight();

        // AABB collision check
        bool collision = (std::abs(x1 - x2) < (w1 + w2) / 2.0f) &&
                        (std::abs(y1 - y2) < (h1 + h2) / 2.0f);

        if (collision) {
            watergirl->addDiamond();
            destroy();
        }
    }
}

void BlueDiamond::onCollisionEnter(const CollisionData& collision) {
    if (Watergirl* watergirl = dynamic_cast<Watergirl*>(collision.other)) {
        watergirl->addDiamond();
        destroy();
    }
}