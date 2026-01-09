#include "diamond/RedDiamond.hpp"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Engine/GameEngine.h"

RedDiamond::RedDiamond(LevelGrid* grid, int x, int y) {
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
        "resources/diamonds/diamond_red.png");
    addComponent(std::move(sprite));
}

void RedDiamond::checkCollisionWith(GameObject* other) {
    // Don't process collisions if already collected
    if (_collected) {
        return;
    }
    
    if (Fireboy* fireboy = dynamic_cast<Fireboy*>(other)) {
        // Check overlap via AABB (Axis-Aligned Bounding Box)
        float x1 = getTransform()->getPosition()->getX();
        float y1 = getTransform()->getPosition()->getY();
        float w1 = getTransform()->getSize()->getWidth();
        float h1 = getTransform()->getSize()->getHeight();
        
        float x2 = fireboy->getTransform()->getPosition()->getX();
        float y2 = fireboy->getTransform()->getPosition()->getY();
        float w2 = fireboy->getTransform()->getSize()->getWidth();
        float h2 = fireboy->getTransform()->getSize()->getHeight();
        
        // AABB collision check
        bool collision = (std::abs(x1 - x2) < (w1 + w2) / 2.0f) &&
                        (std::abs(y1 - y2) < (h1 + h2) / 2.0f);
        
        if (collision) {
            fireboy->addDiamond();
            // Mark as collected and hide the diamond
            _collected = true;
            removeComponent<SpriteRenderer>(false);
            setLayer(-1);
        }
    }
}

void RedDiamond::onCollisionEnter(const CollisionData& collision) {
    // Don't process collisions if already collected
    if (_collected) {
        return;
    }
    
    if (Fireboy* fireboy = dynamic_cast<Fireboy*>(collision.other)) {
        fireboy->addDiamond();
        // Mark as collected and hide the diamond
        _collected = true;
        removeComponent<SpriteRenderer>(false);
        setLayer(-1);
    }
}