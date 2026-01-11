//
// Created by jusra on 11-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP
#include "grid/GridManager.h"
#include "scenes/LevelScene.hpp"

class Level3Scene : public LevelScene {

public:
    Level3Scene(bool isOnline = false, std::shared_ptr<NetworkSystem> network = nullptr,
                EventManager *eventManager = nullptr)
        : LevelScene(3, isOnline, network, eventManager) {}

protected:
    void createLevelGrid() override {
        auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);
        int w = levelGrid->getWidth();
        int h = levelGrid->getHeight();

        for (int x = 0; x < w; ++x) {
            levelGrid->setCellType(x, 0, CellType::Ground);

            if (x%2==0)
                levelGrid->setCellType(x, 1, CellType::Box);
            levelGrid->setCellType(x, h - 1, CellType::Ground);
        }
        for (int y = 0; y < h; ++y) {
            levelGrid->setCellType(0, y, CellType::Ground);
            levelGrid->setCellType(w - 1, y, CellType::Ground);
        }

        int holeSize = 5;
        int holeStart = (w - holeSize) / 2;
        int holeEnd   = holeStart + holeSize;

        for (int x = 0; x < w; ++x) {
            if (x >= holeStart && x < holeEnd) {
                levelGrid->setCellType(x, 9, CellType::Ground);
                continue;
            }

            levelGrid->setCellType(x, 5, CellType::Ground);
            levelGrid->setCellType(x, 13, CellType::Ground);
        }

        levelGrid->setCellType(3, h - 3, CellType::BlueDoor);
        levelGrid->setCellType(8, h - 3, CellType::RedDoor);

        GridManager::registerGrid(getName(), std::move(levelGrid));
    }
    void setupLevelSpecifics() override {
        LevelGrid* grid = GridManager::getGrid(getName());
        if (!grid) return;

        createCellObjects(grid);

        _fireboy = getFireboy(this);
        _watergirl = getWatergirl(this);

        _watergirl->getTransform()->getPosition()->setX(100);
        _watergirl->getTransform()->getPosition()->setY(100);

        _fireboy->getTransform()->getPosition()->setX(700);
        _fireboy->getTransform()->getPosition()->setY(100);
    }

    std::string getLevelName() const override { return "Level 3: Boxes, boxes everywhere!"; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP