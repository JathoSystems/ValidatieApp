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
            std::cout << "[RestartScene] Restarting level..." << std::endl;

            // Do soft reset
            levelScene->resetLevel();

            // Switch to the level
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
        std::cout << "[RestartScene] Going to Main Menu..." << std::endl;
        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();

        if (_isOnline) {
            // Send quit packet - handler will do the cleanup
            QuitPacket quit;
            _network->send(quit);
        } else {
            // For offline mode, do cleanup manually
            if (!_previousLevel.empty()) {
                // Set global flag FIRST
                GlobalFlags::isLevelCleaning = true;

                // Small delay
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                Scene* levelScene = sceneSystem->getScene(_previousLevel);
                if (auto* level = dynamic_cast<LevelScene*>(levelScene)) {
                    level->cleanup();
                }
                sceneSystem->removeScene(_previousLevel);

                GlobalFlags::isLevelCleaning = false;
            }
        }

        // Clear state
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