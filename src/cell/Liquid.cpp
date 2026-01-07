//
// Created by jusra on 5-1-2026.
//

#include "cell/Liquid.hpp"

#include "LevelGrid.h"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include <memory>

Liquid::Liquid(
    LevelGrid* grid,
    int x,
    int y,
    std::string left,
    std::string right,
    std::string middle
)
    : _leftSprite(std::move(left)),
      _rightSprite(std::move(right)),
      _middleSprite(std::move(middle))
{
    CellType currentType = grid->getCellType(x, y);
    CellType leftType = grid->getCellType(x - 1, y);
    CellType rightType = grid->getCellType(x + 1, y);

    bool hasLeft = (leftType == currentType);
    bool hasRight = (rightType == currentType);

    std::string spriteToUse;
    if (hasLeft && hasRight) {
        spriteToUse = _middleSprite;
    } else if (hasLeft && !hasRight) {
        spriteToUse = _rightSprite;
    } else if (!hasLeft && hasRight) {
        spriteToUse = _leftSprite;
    } else {
        spriteToUse = _middleSprite;
    }

    int cellSize = grid->getCellSize();
    getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
    getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
    getTransform()->getSize()->setWidth(cellSize);
    getTransform()->getSize()->setHeight(cellSize);
    setLayer(0);

    auto sprite = std::make_unique<SpriteRenderer>(spriteToUse);
    addComponent(std::move(sprite));
}