#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "GridCell.h"
#include "LevelGrid.h"
#include "GridRenderer.h"
#include "GridManager.h"
#include "LevelBuilder.h"
#include "Scenes/Scene.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Scenes/Camera/Viewport.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Physics/Box2DFacade.h"

class LevelCreator {
private:
    static constexpr int GRID_WIDTH = 80;
    static constexpr int GRID_HEIGHT = 60;
    static constexpr int CELL_SIZE = 10;
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

    std::unique_ptr<LevelGrid> levelGrid;
    std::unique_ptr<GridRenderer> gridRenderer;
    LevelGrid* gridPtr;
    Scene* scenePtr;

public:
    LevelCreator() : gridPtr(nullptr), scenePtr(nullptr) {}

    void initLevel(Scene* scene, Box2DFacade* box2DFacade) {
        scenePtr = scene;

        levelGrid = std::make_unique<LevelGrid>(GRID_WIDTH, GRID_HEIGHT, CELL_SIZE);
        gridPtr = levelGrid.get();

        gridRenderer = std::make_unique<GridRenderer>(gridPtr, scenePtr, box2DFacade);

        auto viewport = std::make_unique<Viewport>(Size(WINDOW_WIDTH, WINDOW_HEIGHT), Position(0, 0));
        int gridCenterX = (GRID_WIDTH * CELL_SIZE) / 2;
        int gridCenterY = (GRID_HEIGHT * CELL_SIZE) / 2;
        auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(gridCenterX,gridCenterY));
        scenePtr->setCamera(std::move(camera));
    }

    void createSimpleLevel() {
        if (!gridPtr || !scenePtr) return;

        addBackground();
        LevelBuilder::buildWithBorders(gridPtr, 5);
        LevelBuilder::buildPlatform(gridPtr, 30, 35, 20, 2);
        LevelBuilder::buildPlatform(gridPtr, 15, 45, 15, 2);
        LevelBuilder::buildPlatform(gridPtr, 55, 45, 15, 2);
        LevelBuilder::buildStairs(gridPtr, 10, 40, 5, 2, 2, true);
    }

    void createFromTextMap(const std::string &path) {
        if (!gridPtr || !scenePtr) return;

        addBackground();

        std::vector<std::string> levelMap;
        std::ifstream file(path);

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                levelMap.push_back(line);
            }
            file.close();
        } else {
            std::cerr << "Error: Could not load path: " << path << std::endl;
            std::cout << "Loading default: " << std::endl;
            createSimpleLevel();
            return;
        }

        LevelBuilder::buildFromString(gridPtr, levelMap);
    }

    void createLevel() {
        if (!gridPtr || !scenePtr) return;

        addBackground();

        for (int x = 0; x < GRID_WIDTH; ++x) {
            for (int y = GRID_HEIGHT - 5; y < GRID_HEIGHT; ++y) {
                gridPtr->setCellType(x, y, CellType::Ground);
            }
        }

        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                gridPtr->setCellType(x, y, CellType::Ground);
            }
        }

        for (int x = GRID_WIDTH - 5; x < GRID_WIDTH; ++x) {
            for (int y = 0; y < GRID_HEIGHT; ++y) {
                gridPtr->setCellType(x, y, CellType::Ground);
            }
        }

        for (int x = 30; x < 50; ++x) {
            for (int y = 35; y < 37; ++y) {
                gridPtr->setCellType(x, y, CellType::Ground);
            }
        }
    }

    void renderLevel() {
        if (gridRenderer) {
            gridRenderer->renderGrid();
        }
    }

    void registerGrid(const std::string& sceneName) {
        GridManager::registerGrid(sceneName, std::move(levelGrid));
    }

private:
    void addBackground() {
        auto background = std::make_unique<GameObject>();
        background->getTransform()->getPosition()->setX(400);
        background->getTransform()->getPosition()->setY(300);
        background->getTransform()->getSize()->setWidth(800);
        background->getTransform()->getSize()->setHeight(600);
        background->addComponent(std::make_unique<SpriteRenderer>("resources/bg.png"));
        background->setLayer(-1);
        scenePtr->addObject(std::move(background));
    }
};