//
// Created by jusra on 10-1-2026.
//
#include "scenes/levels/Level1Scene.hpp"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"

void Level1Scene::createLevelGrid() {
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);

    // Ground floor
    for (int x = 0; x < 32; x++) {
        levelGrid->setCellType(x, 17, CellType::Ground);
    }

    // Side walls
    for (int y = 0; y < 18; y++) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(31, y, CellType::Ground);
    }

    // Left platform (for Fireboy)
    for (int x = 5; x < 12; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }

    // Right platform (with lava hazard)
    for (int x = 20; x < 27; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }
    for (int x = 22; x < 25; x++) {
        levelGrid->setCellType(x, 11, CellType::Lava);
    }

    // Center platform (neutral zone)
    for (int x = 12; x < 20; x++) {
        levelGrid->setCellType(x, 8, CellType::Ground);
    }

    // Diamonds
    levelGrid->setCellType(8, 11, CellType::RedDiamond);
    levelGrid->setCellType(23, 11, CellType::BlueDiamond);
    levelGrid->setCellType(15, 7, CellType::RedDiamond);
    levelGrid->setCellType(16, 7, CellType::BlueDiamond);

    // Doors (exit)
    levelGrid->setCellType(3, 16, CellType::RedDoor);
    levelGrid->setCellType(28, 16, CellType::BlueDoor);

    GridManager::registerGrid(getName(), std::move(levelGrid));
}

void Level1Scene::setupLevelSpecifics() {
    LevelGrid* grid = GridManager::getGrid(getName());
    if (!grid) return;

    createCellObjects(grid);
}