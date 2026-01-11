//
// Created by jusra on 5-1-2026.
//

#include "scenes/RestartScene.hpp"

#include "characters/BaseCharacter.hpp"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "Network/GameState.hpp"
#include "scenes/LevelScene.hpp"
#include "Scenes/SceneSystem.h"
#include "server/packet/QuitPacket.hpp"
#include "server/packet/RestartPacket.hpp"
#include "UI/Button.h"
#include "LevelSwitcher.hpp"

extern std::shared_ptr<NetworkSystem> network;

RestartScene::RestartScene(std::shared_ptr<NetworkSystem> network) : Scene("Restart") {
    _network = network;
}

void RestartScene::onInitialRender() {
    std::unique_ptr<GameObject> background = std::make_unique<GameObject>();
    std::unique_ptr<Button> restartButton = std::make_unique<Button>("Restart", std::make_unique<Color>(0, 255, 0));
    restartButton->setOnClick([this]() {
        std::cout << "PREV " << _previousLevel << std::endl;
        if (_isOnline) {
            RestartPacket p{_previousLevel};
            _network->send(p);
        } else {
            int levelNumber = 1;
            if (_previousLevel.find("level_") == 0) {
                std::string numStr = _previousLevel.substr(6);
                try {
                    levelNumber = std::stoi(numStr);
                } catch (...) {
                    levelNumber = 1;
                }
            }
            LevelSwitcher switcher{nullptr, nullptr};
            switcher.openLevel(levelNumber, false);
        }
    });
    background->addComponent(std::move(restartButton));
    background->getTransform()->getPosition()->setX(640);
    background->getTransform()->getPosition()->setY(360);

    std::unique_ptr<GameObject> mainMenuButtonObject = std::make_unique<GameObject>();
    std::unique_ptr<Button> mainMenuButton = std::make_unique<Button>("Main Menu",
                                                                      std::make_unique<Color>(255, 0, 0));
    mainMenuButton->setOnClick([this]() {
        if (_isOnline) {
            // Send quit packet BEFORE clearing game state (so lobby ID is available)
            QuitPacket quit;
            _network->send(quit);
            // Clear game state so old lobby/role info doesn't interfere with reconnection
            GameState::getInstance().clear();
        }
        SceneSystem* sceneSystem = GameEngine::getInstance().getSystem<SceneSystem>();
        sceneSystem->setScene("MainMenu");
    });
    mainMenuButtonObject->addComponent(std::move(mainMenuButton));
    mainMenuButtonObject->getTransform()->getPosition()->setX(640);
    mainMenuButtonObject->getTransform()->getPosition()->setY(460);

    addObject(std::move(mainMenuButtonObject));
    addObject(std::move(background));
}
