//
// Created by jusra on 10-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVEL2SCENE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVEL2SCENE_HPP
#include "scenes/LevelScene.hpp"

class Level2Scene : public LevelScene {
public:
    Level2Scene(bool isOnline = false, std::shared_ptr<NetworkSystem> network = nullptr,
                EventManager *eventManager = nullptr)
        : LevelScene(2, isOnline, network, eventManager) {}

protected:
    void createLevelGrid() override;
    void setupLevelSpecifics() override;
    std::string getLevelName() const override { return "Level 3: Lost without you"; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LEVEL2SCENE_HPP