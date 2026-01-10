//
// Created by jusra on 10-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVEL1SCENE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVEL1SCENE_HPP

#include "scenes/LevelScene.hpp"

class Fireboy;
class Watergirl;

class Level1Scene : public LevelScene {
public:
    Level1Scene(bool isOnline = false, std::shared_ptr<NetworkSystem> network = nullptr,
                EventManager *eventManager = nullptr)
        : LevelScene(1, isOnline, network, eventManager) {}

protected:
    void createLevelGrid() override;
    void setupLevelSpecifics() override;
    std::string getLevelName() const override { return "Level 1: The Beginning"; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LEVEL1SCENE_HPP