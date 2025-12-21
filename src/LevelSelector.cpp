#include "LevelSelector.h"
#include "scenes/LevelScene.hpp"
#include "scenes/RoomSelectionScene.hpp"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Engine/GameEngine.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "SpawnEvent.hpp"
#include <asio.hpp>
#include <iostream>

LevelSelector::LevelSelector(SceneSystem *sceneSystem, std::shared_ptr<NetworkSystem> network, EventManager* eventManager)
    : _sceneSystem(sceneSystem), _network(network), _eventManager(eventManager) {
}

void LevelSelector::createLevelSelectorScene() {
    std::unique_ptr<Scene> selectorScene = std::make_unique<Scene>("level_selector");

    std::unique_ptr<Text> titleText = std::make_unique<Text>("Select Level");
    titleText->setColor(std::make_unique<Color>(255, 255, 255));
    titleText->setFontSize(48);
    std::unique_ptr<GameObject> titleObject = std::make_unique<GameObject>();
    titleObject->addComponent(std::move(titleText));
    titleObject->getTransform()->getPosition()->setX(480);
    titleObject->getTransform()->getPosition()->setY(30);
    titleObject->getTransform()->getSize()->setWidth(400);
    titleObject->getTransform()->getSize()->setHeight(60);
    selectorScene->addObject(std::move(titleObject));

    auto backButton = std::make_unique<Button>("Back", std::make_unique<Color>(255, 100, 100));
    backButton->setOnClick([this]() {
        _sceneSystem->setScene("MainMenu");
    });
    auto backButtonObj = std::make_unique<GameObject>();
    backButtonObj->addComponent(std::move(backButton));
    backButtonObj->getTransform()->getPosition()->setX(20);
    backButtonObj->getTransform()->getPosition()->setY(20);
    backButtonObj->getTransform()->getSize()->setWidth(80);
    backButtonObj->getTransform()->getSize()->setHeight(40);
    selectorScene->addObject(std::move(backButtonObj));

    float startX = 100;
    float startY = 120;
    float cardWidth = 250;
    float cardHeight = 200;
    float spacing = 70;

    for (int i = 1; i <= 4; i++) {
        float x = startX + ((i - 1) % 2) * (cardWidth + spacing);
        float y = startY + ((i - 1) / 2) * (cardHeight + spacing);

        auto levelText = std::make_unique<Text>("Level " + std::to_string(i));
        levelText->setColor(std::make_unique<Color>(255, 255, 255));
        auto levelTextObj = std::make_unique<GameObject>();
        levelTextObj->addComponent(std::move(levelText));
        levelTextObj->getTransform()->getPosition()->setX(x + 80);
        levelTextObj->getTransform()->getPosition()->setY(y);
        levelTextObj->getTransform()->getSize()->setWidth(150);
        levelTextObj->getTransform()->getSize()->setHeight(40);
        selectorScene->addObject(std::move(levelTextObj));

        int levelNum = i;
        auto playButton = std::make_unique<Button>("Play", std::make_unique<Color>(0, 128, 255));
        playButton->setOnClick([this, levelNum]() {
            onPlayClicked(levelNum);
        });
        auto playButtonObj = std::make_unique<GameObject>();
        playButtonObj->addComponent(std::move(playButton));
        playButtonObj->getTransform()->getPosition()->setX(x);
        playButtonObj->getTransform()->getPosition()->setY(y + 50);
        playButtonObj->getTransform()->getSize()->setWidth(cardWidth);
        playButtonObj->getTransform()->getSize()->setHeight(50);
        selectorScene->addObject(std::move(playButtonObj));

        auto onlineButton = std::make_unique<Button>("Online Play", std::make_unique<Color>(128, 0, 128));
        onlineButton->setOnClick([this, levelNum]() {
            onOnlinePlayClicked(levelNum);
        });
        auto onlineButtonObj = std::make_unique<GameObject>();
        onlineButtonObj->addComponent(std::move(onlineButton));
        onlineButtonObj->getTransform()->getPosition()->setX(x);
        onlineButtonObj->getTransform()->getPosition()->setY(y + 110);
        onlineButtonObj->getTransform()->getSize()->setWidth(cardWidth);
        onlineButtonObj->getTransform()->getSize()->setHeight(50);
        selectorScene->addObject(std::move(onlineButtonObj));
    }

    std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    std::unique_ptr<FixedCamera> camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    selectorScene->setCamera(std::move(camera));

    _sceneSystem->addScene(std::move(selectorScene));
}

void LevelSelector::onPlayClicked(int levelNumber) {
    std::string sceneName = "level_" + std::to_string(levelNumber);
    
    if (_sceneSystem->getActiveSceneObj()->getName() != sceneName) {
        auto levelScene = std::make_unique<LevelScene>(levelNumber, false, nullptr, _eventManager);
        _sceneSystem->addScene(std::move(levelScene));
    }
    
    _sceneSystem->setScene(sceneName);
}

void LevelSelector::setupNetworkCallbacks() {
    if (_networkCallbacksSetup) return;
    
    _network->getMiddleware()->setOnEventReceived([](int id, std::shared_ptr<IEvent> event) {
        if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
            spawn->spawn();
            return;
        }

        GameObject *object = ObjectRegistry::getInstance().getObject(id);
        if (!object) return;
        event->apply(object);
    });

    _eventManager->setEventCallback([](int id, std::shared_ptr<IEvent> event) {
        if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
            spawn->spawn();
            return;
        }

        GameObject *object = ObjectRegistry::getInstance().getObject(id);
        if (!object) return;
        event->apply(object);
    });
    
    _networkCallbacksSetup = true;
}

void LevelSelector::onOnlinePlayClicked(int levelNumber) {
    // Navigate to room selection scene for this level
    std::string sceneName = "room_selection_level_" + std::to_string(levelNumber);
    
    // Create the room selection scene (addScene handles duplicates or we can track)
    auto roomScene = std::make_unique<RoomSelectionScene>(_network, levelNumber);
    _sceneSystem->addScene(std::move(roomScene));
    
    _sceneSystem->setScene(sceneName);
}
