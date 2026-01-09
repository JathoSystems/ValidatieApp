//
// Created by jusra on 5-1-2026.
//

#include "scenes/RestartScene.hpp"

#include "characters/BaseCharacter.hpp"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "Scenes/SceneSystem.h"
#include "server/packet/QuitPacket.hpp"
#include "server/packet/RestartPacket.hpp"
#include "UI/Button.h"

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
            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
            Scene *scene = sceneSystem->getScene(_previousLevel);

            for (std::unique_ptr<GameObject> &game_object: scene->getObjects()) {
                if (BaseCharacter *base = dynamic_cast<BaseCharacter *>(game_object.get()))
                    base->destroy();
            }

            sceneSystem->setScene(_previousLevel);
        }
    });
    background->addComponent(std::move(restartButton));
    background->getTransform()->getPosition()->setX(640);
    background->getTransform()->getPosition()->setY(360);

    std::unique_ptr<GameObject> mainMenuButtonObject = std::make_unique<GameObject>();
    std::unique_ptr<Button> mainMenuButton = std::make_unique<Button>("Main Menu",
                                                                      std::make_unique<Color>(255, 0, 0));
    mainMenuButton->setOnClick([this]() {
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
        if (_isOnline) {
            QuitPacket quit;
            _network->send(quit);
        }
    });
    mainMenuButtonObject->addComponent(std::move(mainMenuButton));
    mainMenuButtonObject->getTransform()->getPosition()->setX(640);
    mainMenuButtonObject->getTransform()->getPosition()->setY(460);

    addObject(std::move(mainMenuButtonObject));
    addObject(std::move(background));
}
