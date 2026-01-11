#include "scenes/RestartScene.hpp"
#include "Engine/GameEngine.h"
#include "Network/GameState.hpp"
#include "Scenes/SceneSystem.h"
#include "server/packet/QuitPacket.hpp"
#include "UI/Button.h"

RestartScene::RestartScene(std::shared_ptr<NetworkSystem> network) : Scene("Restart") {
    _network = network;
}

void RestartScene::onInitialRender() {
    bool isOnline = (GameState::getInstance().get("lobby", "nope") != "nope");
    
    if (!isOnline) {
        std::unique_ptr<GameObject> restartButtonObject = std::make_unique<GameObject>();
        std::unique_ptr<Button> restartButton = std::make_unique<Button>("Restart", std::make_unique<Color>(0, 255, 0));
        restartButton->setOnClick([this]() {
            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
            sceneSystem->setScene(_previousLevel);
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
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
        if (isOnline) {
            QuitPacket quit;
            _network->send(quit);
        }
    });
    mainMenuButtonObject->addComponent(std::move(mainMenuButton));
    mainMenuButtonObject->getTransform()->getPosition()->setX(640);
    mainMenuButtonObject->getTransform()->getPosition()->setY(360);

    addObject(std::move(mainMenuButtonObject));
}
