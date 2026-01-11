//
// Created by jusra on 10-1-2026.
//
#include "scenes/levels/Level1Scene.hpp"

#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"

void Level1Scene::createLevelGrid() {
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);

    for (int x = 0; x < 32; x++) {
        levelGrid->setCellType(x, 17, CellType::Ground);
    }

    for (int y = 0; y < 18; y++) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(31, y, CellType::Ground);
    }

    for (int x = 0; x < 12; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }

    for (int x = 20; x < 32; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }

    for (int x = 12; x < 20; x++) {
        levelGrid->setCellType(x, 8, CellType::Ground);
    }

    levelGrid->setCellType(8, 11, CellType::RedDiamond);
    levelGrid->setCellType(23, 11, CellType::BlueDiamond);
    levelGrid->setCellType(15, 7, CellType::RedDiamond);
    levelGrid->setCellType(16, 7, CellType::BlueDiamond);

    levelGrid->setCellType(3, 15, CellType::RedDoor);
    levelGrid->setCellType(28, 15, CellType::BlueDoor);

    GridManager::registerGrid(getName(), std::move(levelGrid));
}

void Level1Scene::setupLevelSpecifics() {
    LevelGrid* grid = GridManager::getGrid(getName());
    if (!grid) return;

    createCellObjects(grid);

    Fireboy* fireboy = getFireboy(this);
    Watergirl* watergirl = getWatergirl(this);

    watergirl->getTransform()->getPosition()->setX(550);
    watergirl->getTransform()->getPosition()->setY(10);

    fireboy->getTransform()->getPosition()->setX(650);
    fireboy->getTransform()->getPosition()->setY(10);
}