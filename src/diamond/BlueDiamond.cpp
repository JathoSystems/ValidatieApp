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
    _collected = false;

    int cellSize = grid->getCellSize();

    getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
    getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
    getTransform()->getSize()->setWidth(cellSize);
    getTransform()->getSize()->setHeight(cellSize);
    setLayer(0);

    auto sprite = std::make_unique<SpriteRenderer>(
        "resources/diamonds/diamond_blue.png");
    addComponent(std::move(sprite));
}

void BlueDiamond::checkCollisionWith(GameObject* other) {
    // Don't process collisions if already collected
    if (_collected) {
        return;
    }
    
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
            // Mark as collected and hide the diamond
            _collected = true;
            removeComponent<SpriteRenderer>(false);
            setLayer(-1);
        }
    }
}

void BlueDiamond::onCollisionEnter(const CollisionData& collision) {
    // Don't process collisions if already collected
    if (_collected) {
        return;
    }
    
    if (Watergirl* watergirl = dynamic_cast<Watergirl*>(collision.other)) {
        watergirl->addDiamond();
        // Mark as collected and hide the diamond
        _collected = true;
        removeComponent<SpriteRenderer>(false);
        setLayer(-1);
    }
}