//
// Created by jusra on 11-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP
#include "scenes/LevelScene.hpp"

class Level3Scene : public LevelScene {

public:
    Level3Scene(bool isOnline = false, std::shared_ptr<NetworkSystem> network = nullptr,
                EventManager *eventManager = nullptr)
        : LevelScene(3, isOnline, network, eventManager) {}

protected:
    void createLevelGrid() override;
    void setupLevelSpecifics() override;
    std::string getLevelName() const override { return "Level 3: Boxes, boxes everywhere!"; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LEVEL3SCENE_HPP