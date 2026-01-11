#include "LevelSelector.h"
#include "scenes/LevelScene.hpp"
#include "scenes/LevelScene.hpp"
#include "scenes/RoomSelectionScene.hpp"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/Camera/FixedCamera.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "SpawnEvent.hpp"
#include "LevelSaver.hpp"
#include <asio.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "scenes/levels/Level1Scene.hpp"
#include "scenes/levels/Level2Scene.hpp"
#include "scenes/levels/Level3Scene.hpp"

extern std::map<int, std::function<std::unique_ptr<Scene>()> > g_levels;

LevelSelector* LevelSelector::_instance = nullptr;

LevelSelector::LevelSelector(SceneSystem *sceneSystem, std::shared_ptr<NetworkSystem> network,
                             EventManager *eventManager) : _sceneSystem(sceneSystem), _network(network),
                                                           _eventManager(eventManager) {
    _instance = this;
    g_levels[1] = []() { return std::make_unique<Level1Scene>(); };
    g_levels[2] = []() { return std::make_unique<Level2Scene>(); };
    g_levels[3] = []() { return std::make_unique<Level3Scene>(); };
}

LevelSelector::~LevelSelector() {
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
    backButton->setOnClick([this]() { _sceneSystem->setScene("MainMenu"); });
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


    for (const auto &[levelNum, factory]: g_levels) {
        float x = startX + ((levelNum - 1) % 2) * (cardWidth + spacing);
        float y = startY + ((levelNum - 1) / 2) * (cardHeight + spacing);
        auto levelText = std::make_unique<Text>("Level " + std::to_string(levelNum));
        LevelSaver saver;
        float completionTime = saver.getCompletionTime(levelNum);
        int redGems = saver.getRedGems(levelNum);
        int blueGems = saver.getBlueGems(levelNum);
        std::unique_ptr<Color> color = std::make_unique<Color>(255, 255, 255);
        if (completionTime != -1) color = std::make_unique<Color>(0, 255, 0);
        levelText->setColor(std::move(color));
        auto levelTextObj = std::make_unique<GameObject>();
        Text* levelTextPtr = levelText.get();
        _levelTextMap[levelNum] = levelTextPtr;
        levelTextObj->addComponent(std::move(levelText));
        levelTextObj->getTransform()->getPosition()->setX(x + 80);
        levelTextObj->getTransform()->getPosition()->setY(y);
        levelTextObj->getTransform()->getSize()->setWidth(150);
        levelTextObj->getTransform()->getSize()->setHeight(40);
        selectorScene->addObject(std::move(levelTextObj));
        
        // Always add stats text (empty if level not completed)
        std::ostringstream statsText;
        if (completionTime != -1) {
            statsText << std::fixed << std::setprecision(1) << completionTime << "s";
            if (redGems != -1 || blueGems != -1) {
                statsText << " | ";
                if (redGems != -1) {
                    statsText << "R:" << redGems;
                }
                if (blueGems != -1) {
                    if (redGems != -1) statsText << " ";
                    statsText << "B:" << blueGems;
                }
            }
        }
        std::string statsTextStr = statsText.str();
        // Only create stats text object if there's actual text to display
        if (!statsTextStr.empty()) {
            auto statsTextObj = std::make_unique<Text>(statsTextStr);
            statsTextObj->setColor(std::make_unique<Color>(200, 200, 200));
            statsTextObj->setFontSize(10);
            auto statsGameObj = std::make_unique<GameObject>();
            Text* statsTextPtr = statsTextObj.get();
            _statsTextMap[levelNum] = statsTextPtr;
            statsGameObj->addComponent(std::move(statsTextObj));
            statsGameObj->getTransform()->getPosition()->setX(x + 10);
            statsGameObj->getTransform()->getPosition()->setY(y + 35);
            statsGameObj->getTransform()->getSize()->setWidth(cardWidth);
            statsGameObj->getTransform()->getSize()->setHeight(20);
            selectorScene->addObject(std::move(statsGameObj));
        }
        auto playButton = std::make_unique<Button>("Play", std::make_unique<Color>(0, 128, 255));
        playButton->setOnClick([this, levelNum]() { onPlayClicked(levelNum); });
        auto playButtonObj = std::make_unique<GameObject>();
        playButtonObj->addComponent(std::move(playButton));
        playButtonObj->getTransform()->getPosition()->setX(x);
        playButtonObj->getTransform()->getPosition()->setY(y + 70);
        playButtonObj->getTransform()->getSize()->setWidth(cardWidth);
        playButtonObj->getTransform()->getSize()->setHeight(50);
        selectorScene->addObject(std::move(playButtonObj));
        auto onlineButton = std::make_unique<Button>("Online Play", std::make_unique<Color>(128, 0, 128));
        onlineButton->setOnClick([this, levelNum]() { onOnlinePlayClicked(levelNum); });
        auto onlineButtonObj = std::make_unique<GameObject>();
        onlineButtonObj->addComponent(std::move(onlineButton));
        onlineButtonObj->getTransform()->getPosition()->setX(x);
        onlineButtonObj->getTransform()->getPosition()->setY(y + 130);
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
    if (g_levels.find(levelNumber) == g_levels.end()) {
        std::cerr << "Level " << levelNumber << " not found!" << std::endl;
        return;
    }
    
    Scene* currentScene = _sceneSystem->getActiveSceneObj();
    if (currentScene) {
        LevelScene* currentLevelScene = dynamic_cast<LevelScene*>(currentScene);
        if (currentLevelScene) {
            std::cout << "[LevelSelector] Cleaning up current level scene before switching..." << std::endl;
            currentLevelScene->cleanup();
        }
    }
    
    std::string sceneName = "level_" + std::to_string(levelNumber);
    std::unique_ptr<Scene> scene = g_levels[levelNumber]();
    if (_sceneSystem->getActiveSceneObj()->getName() != sceneName) { _sceneSystem->addScene(std::move(scene)); }
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

void LevelSelector::updateLevelStatus(Scene *selectorScene) {
    if (!selectorScene || selectorScene->getName() != "level_selector") {
        return;
    }
    
    LevelSaver saver;
    for (const auto &[levelNum, textPtr] : _levelTextMap) {
        if (textPtr) {
            float completionTime = saver.getCompletionTime(levelNum);
            std::unique_ptr<Color> color = std::make_unique<Color>(255, 255, 255);
            if (completionTime != -1) {
                color = std::make_unique<Color>(0, 255, 0);
            }
            textPtr->setColor(std::move(color));
        }
    }
    
    // Update stats text
    for (const auto &[levelNum, statsTextPtr] : _statsTextMap) {
        if (statsTextPtr) {
            float completionTime = saver.getCompletionTime(levelNum);
            int redGems = saver.getRedGems(levelNum);
            int blueGems = saver.getBlueGems(levelNum);
            
            std::ostringstream statsText;
            if (completionTime != -1) {
                statsText << std::fixed << std::setprecision(1) << completionTime << "s";
                if (redGems != -1 || blueGems != -1) {
                    statsText << " | ";
                    if (redGems != -1) {
                        statsText << "R:" << redGems;
                    }
                    if (blueGems != -1) {
                        if (redGems != -1) statsText << " ";
                        statsText << "B:" << blueGems;
                    }
                }
            }
            statsTextPtr->setText(statsText.str());
        }
    }
}

void LevelSelector::onOnlinePlayClicked(int levelNumber) {
    if (g_levels.find(levelNumber) == g_levels.end()) {
        std::cerr << "Level " << levelNumber << " not found!" << std::endl;
        return;
    }
    std::string sceneName = "room_selection_level_" + std::to_string(levelNumber);
    auto roomScene = std::make_unique<RoomSelectionScene>(_network, levelNumber, g_levels[levelNumber]);
    _sceneSystem->addScene(std::move(roomScene));
    _sceneSystem->setScene(sceneName);
}
