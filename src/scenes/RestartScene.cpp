#include "scenes/RestartScene.hpp"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "Network/GameState.hpp"
#include "Scenes/SceneSystem.h"
#include "server/packet/QuitPacket.hpp"
#include "UI/Button.h"
#include "LevelSwitcher.hpp"

RestartScene::RestartScene(std::shared_ptr<NetworkSystem> network) : Scene("Restart") {
    _network = network;
}

void RestartScene::onInitialRender() {
    bool isOnline = (GameState::getInstance().get("lobby", "nope") != "nope");

    if (!isOnline) {
        std::unique_ptr<GameObject> restartButtonObject = std::make_unique<GameObject>();
        std::unique_ptr<Button> restartButton = std::make_unique<Button>("Restart", std::make_unique<Color>(0, 255, 0));
        restartButton->setOnClick([this]() {
            int levelNumber = 1;
            if (_previousLevel.find("level_") == 0) {
                std::string numStr = _previousLevel.substr(6);
                size_t pos = numStr.find('_');
                if (pos != std::string::npos) {
                    numStr = numStr.substr(0, pos);
                }
                try {
                    levelNumber = std::stoi(numStr);
                } catch (...) {
                    levelNumber = 1;
                }
            }
            LevelSwitcher switcher{nullptr, nullptr};
            switcher.openLevel(levelNumber, false);
        });
        restartButtonObject->addComponent(std::move(restartButton));
        restartButtonObject->getTransform()->getPosition()->setX(640);
        restartButtonObject->getTransform()->getPosition()->setY(260);
        addObject(std::move(restartButtonObject));
    }

    std::unique_ptr<GameObject> mainMenuButtonObject = std::make_unique<GameObject>();
    std::unique_ptr<Button> mainMenuButton = std::make_unique<Button>("Main Menu",
                                                                      std::make_unique<Color>(255, 0, 0));
    mainMenuButton->setOnClick([this, isOnline]() {
        if (isOnline) {
            QuitPacket quit;
            _network->send(quit);
            GameState::getInstance().clear();
        }
        SceneSystem *sceneSystem = GameEngine::getInstance().getSystem<SceneSystem>();
        sceneSystem->setScene("MainMenu");
        sceneSystem->removeScene("Restart");
    });
    mainMenuButtonObject->addComponent(std::move(mainMenuButton));
    mainMenuButtonObject->getTransform()->getPosition()->setX(640);
    mainMenuButtonObject->getTransform()->getPosition()->setY(360);

    addObject(std::move(mainMenuButtonObject));
}
