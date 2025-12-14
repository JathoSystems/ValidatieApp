#include <iostream>
#include "Engine/GameEngine.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "LevelGrid.h"
#include "GameObjects/GameObject.h"

int main() {
    try {
        std::cout << "=== Initializing Game Engine ===" << std::endl;

        GameEngine gameEngine;
        gameEngine.init("Vuurjongen Watermeisje", 800, 600);

        std::cout << "=== Creating Level Grid ===" << std::endl;

        const int GRID_WIDTH = 80;
        const int GRID_HEIGHT = 60;
        const int CELL_SIZE = 10;

        LevelGrid levelGrid(GRID_WIDTH, GRID_HEIGHT);

        auto mainScene = std::make_unique<Scene>("MainScene");

        // Ground floor
        for (int x = 0; x < GRID_WIDTH; ++x) {
            for (int y = GRID_HEIGHT - 5; y < GRID_HEIGHT; ++y) {
                levelGrid.setCellType(x, y, CellType::Ground);

                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * CELL_SIZE);
                block->getTransform()->getPosition()->setY(y * CELL_SIZE);
                block->getTransform()->getSize()->setWidth(CELL_SIZE);
                block->getTransform()->getSize()->setHeight(CELL_SIZE);
                mainScene->addObject(std::move(block));
            }
        }
        std::cout << "Ground floor created: " << (GRID_WIDTH * 5) << " blocks" << std::endl;

        // Left wall
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                levelGrid.setCellType(x, y, CellType::Ground);

                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * CELL_SIZE);
                block->getTransform()->getPosition()->setY(y * CELL_SIZE);
                block->getTransform()->getSize()->setWidth(CELL_SIZE);
                block->getTransform()->getSize()->setHeight(CELL_SIZE);
                mainScene->addObject(std::move(block));
            }
        }
        std::cout << "Left wall created: " << (5 * GRID_HEIGHT) << " blocks" << std::endl;

        // Right wall
        for (int x = GRID_WIDTH - 5; x < GRID_WIDTH; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                levelGrid.setCellType(x, y, CellType::Ground);

                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * CELL_SIZE);
                block->getTransform()->getPosition()->setY(y * CELL_SIZE);
                block->getTransform()->getSize()->setWidth(CELL_SIZE);
                block->getTransform()->getSize()->setHeight(CELL_SIZE);
                mainScene->addObject(std::move(block));
            }
        }
        std::cout << "Right wall created: " << (5 * GRID_HEIGHT) << " blocks" << std::endl;

        // Middle platform
        int platformBlocks = 0;
        for (int x = 30; x < 50; ++x) {
            for (int y = 35; y < 37; ++y) {
                levelGrid.setCellType(x, y, CellType::Ground);

                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * CELL_SIZE);
                block->getTransform()->getPosition()->setY(y * CELL_SIZE);
                block->getTransform()->getSize()->setWidth(CELL_SIZE);
                block->getTransform()->getSize()->setHeight(CELL_SIZE);
                mainScene->addObject(std::move(block));
                platformBlocks++;
            }
        }
        std::cout << "Middle platform created: " << platformBlocks << " blocks" << std::endl;
        std::cout << "Grid created: " << GRID_WIDTH << "x" << GRID_HEIGHT << std::endl;
        std::cout << "Total objects in scene: " << mainScene->getObjects().size() << std::endl;

        // Get SceneSystem using getSystem method
        SceneSystem* sceneSystem = gameEngine.getSystem<SceneSystem>();

        if (sceneSystem) {
            sceneSystem->addScene(std::move(mainScene));
            sceneSystem->setScene("MainScene");
            std::cout << "Scene created and set as active" << std::endl;
        }

        // Verify grid data
        const auto& grid = levelGrid.getGrid();
        int groundCount = 0;
        for (const auto& column : grid) {
            for (const auto& cell : column) {
                if (cell == CellType::Ground) {
                    groundCount++;
                }
            }
        }
        std::cout << "Total ground cells in grid: " << groundCount << std::endl;

        // Start the game engine
        gameEngine.start();

    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
