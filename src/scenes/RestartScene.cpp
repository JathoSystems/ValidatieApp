//
// Created by jusra on 5-1-2026.
//

#include "scenes/RestartScene.hpp"

#include "characters/BaseCharacter.hpp"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "scenes/LevelScene.hpp"
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
        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();

        // Get the level scene
        Scene* oldScene = sceneSystem->getScene(_previousLevel);
        if (auto* levelScene = dynamic_cast<LevelScene*>(oldScene)) {
            // Do soft reset
            levelScene->resetLevel();

            // Just switch to the scene - DON'T call onInitialRender() again!
            // The scene is already initialized, we just need to make it active
            sceneSystem->setScene(_previousLevel);

            if (_isOnline) {
                RestartPacket p{_previousLevel};
                _network->send(p);
            }
        }
    });
    background->addComponent(std::move(restartButton));
    background->getTransform()->getPosition()->setX(640);
    background->getTransform()->getPosition()->setY(360);

    std::unique_ptr<GameObject> mainMenuButtonObject = std::make_unique<GameObject>();
    std::unique_ptr<Button> mainMenuButton = std::make_unique<Button>("Main Menu",
                                                                      std::make_unique<Color>(255, 0, 0));
    mainMenuButton->setOnClick([this]() {
        std::cout << "[RestartScene] Going to Main Menu (Hard Cleanup)..." << std::endl;
        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();

        if (_isOnline) {
            QuitPacket quit;
            _network->send(quit);
        }

        // Clean up the level scene BEFORE switching
        if (!_previousLevel.empty()) {
            Scene* levelScene = sceneSystem->getScene(_previousLevel);
            if (auto* level = dynamic_cast<LevelScene*>(levelScene)) {
                level->cleanup();
            }
            sceneSystem->removeScene(_previousLevel);
        }

        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");

        sceneSystem->setScene("MainMenu");
    });
    mainMenuButtonObject->addComponent(std::move(mainMenuButton));
    mainMenuButtonObject->getTransform()->getPosition()->setX(640);
    mainMenuButtonObject->getTransform()->getPosition()->setY(460);

    addObject(std::move(mainMenuButtonObject));
    addObject(std::move(background));
}