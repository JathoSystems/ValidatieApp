#include "grid/LevelGrid.h"
#include "scenes/levels/Level2Scene.hpp"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "grid/GridManager.h"

void Level2Scene::createLevelGrid() {
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);

    int w = levelGrid->getWidth();
    int h = levelGrid->getHeight();

    for (int x = 0; x < w; ++x) {
        levelGrid->setCellType(x, 0, CellType::Ground);
        levelGrid->setCellType(x, h - 1, CellType::Ground);
    }
    for (int y = 0; y < h; ++y) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(w - 1, y, CellType::Ground);
    }

    levelGrid->setCellType(3, h - 3, CellType::BlueDoor);
    levelGrid->setCellType(8, h - 3, CellType::RedDoor);

    int line1Y = h - 1 - h / 3;
    int line2Y = h - 1 - (h / 3) * 2;

    int gapStartX = w - 4;
    int gapEndX = w - 1;

    for (int x = 1; x < w - 1; ++x) {
        if (x >= gapStartX) continue;

        if (x <= 3) levelGrid->setCellType(x, line1Y, CellType::Ground);
        else if (x >= 4 && x <= 5) levelGrid->setCellType(x, line1Y, CellType::Lava);
        else if (x >= 6 && x <= 9) levelGrid->setCellType(x, line1Y, CellType::Ground);
        else if (x >= 10 && x <= 11) levelGrid->setCellType(x, line1Y, CellType::Water);
        else levelGrid->setCellType(x, line1Y, CellType::Ground);
    }

    for (int x = 1; x < w - 1; ++x) {
        if (x >= gapStartX) continue;

        if (x <= 3) levelGrid->setCellType(x, line2Y, CellType::Ground);
        else if (x >= 4 && x <= 5) levelGrid->setCellType(x, line2Y, CellType::Water);
        else if (x >= 7 && x <= 9) levelGrid->setCellType(x, line2Y, CellType::Ground);
        else if (x >= 10 && x <= 11) levelGrid->setCellType(x, line2Y, CellType::Lava);
        else levelGrid->setCellType(x, line2Y, CellType::Ground);
    }

    GridManager::registerGrid(getName(), std::move(levelGrid));
}


void Level2Scene::setupLevelSpecifics() {
    LevelGrid *grid = GridManager::getGrid(getName());
    if (!grid) return;

    createCellObjects(grid);

    _fireboy = getFireboy(this);
    _watergirl = getWatergirl(this);

    _watergirl->getTransform()->getPosition()->setX(100);
    _watergirl->getTransform()->getPosition()->setY(250);

    _fireboy->getTransform()->getPosition()->setX(100);
    _fireboy->getTransform()->getPosition()->setY(100);
}
