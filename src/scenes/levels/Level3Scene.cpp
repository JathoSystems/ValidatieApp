//
// Created by jusra on 10-1-2026.
//
#include "scenes/levels/Level3Scene.hpp"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"

void Level3Scene::createLevelGrid() {
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

    // Complex maze structure
    // Left section - Fireboy's path
    for (int x = 2; x < 8; x++) {
        levelGrid->setCellType(x, 14, CellType::Ground);
    }
    for (int y = 10; y < 15; y++) {
        levelGrid->setCellType(7, y, CellType::Ground);
    }
    for (int x = 7; x < 12; x++) {
        levelGrid->setCellType(x, 10, CellType::Ground);
    }

    // Lava pit left
    for (int x = 3; x < 6; x++) {
        levelGrid->setCellType(x, 16, CellType::Lava);
    }

    // Center vertical corridor
    for (int y = 6; y < 18; y++) {
        levelGrid->setCellType(15, y, CellType::Ground);
        levelGrid->setCellType(16, y, CellType::Ground);
    }

    // Right section - Watergirl's path
    for (int x = 24; x < 30; x++) {
        levelGrid->setCellType(x, 14, CellType::Ground);
    }
    for (int y = 10; y < 15; y++) {
        levelGrid->setCellType(24, y, CellType::Ground);
    }
    for (int x = 20; x < 25; x++) {
        levelGrid->setCellType(x, 10, CellType::Ground);
    }

    // Water pit right
    for (int x = 26; x < 29; x++) {
        levelGrid->setCellType(x, 16, CellType::Water);
    }

    // Top platforms (meeting point)
    for (int x = 10; x < 22; x++) {
        levelGrid->setCellType(x, 6, CellType::Ground);
    }

    // Hazard platforms
    for (int x = 5; x < 10; x++) {
        levelGrid->setCellType(x, 7, CellType::Ground);
    }
    for (int x = 6; x < 9; x++) {
        levelGrid->setCellType(x, 8, CellType::Lava);
    }

    for (int x = 22; x < 27; x++) {
        levelGrid->setCellType(x, 7, CellType::Ground);
    }
    for (int x = 23; x < 26; x++) {
        levelGrid->setCellType(x, 8, CellType::Water);
    }

    // Diamonds scattered throughout
    levelGrid->setCellType(4, 13, CellType::RedDiamond);
    levelGrid->setCellType(10, 9, CellType::RedDiamond);
    levelGrid->setCellType(7, 6, CellType::RedDiamond);
    levelGrid->setCellType(13, 5, CellType::RedDiamond);

    levelGrid->setCellType(27, 13, CellType::BlueDiamond);
    levelGrid->setCellType(21, 9, CellType::BlueDiamond);
    levelGrid->setCellType(24, 6, CellType::BlueDiamond);
    levelGrid->setCellType(18, 5, CellType::BlueDiamond);

    // Doors at the top
    levelGrid->setCellType(14, 5, CellType::RedDoor);
    levelGrid->setCellType(17, 5, CellType::BlueDoor);

    GridManager::registerGrid(getName(), std::move(levelGrid));
}

void Level3Scene::setupLevelSpecifics() {
    LevelGrid* grid = GridManager::getGrid(getName());
    if (!grid) return;

    createCellObjects(grid);
}