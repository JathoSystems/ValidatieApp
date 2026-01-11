#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <memory>
#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "scenes/LevelScene.hpp"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "UI/Text.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <map>

class LevelSelector {
private:
    SceneSystem *_sceneSystem;
    std::shared_ptr<NetworkSystem> _network;
    EventManager* _eventManager;
    bool _networkCallbacksSetup = false;
    std::map<int, Text*> _levelTextMap;
    static LevelSelector* _instance;
public:
    LevelSelector(SceneSystem *sceneSystem, std::shared_ptr<NetworkSystem> network, EventManager* eventManager);

    ~LevelSelector();

    void updateLevelStatus(Scene *selectorScene);

    void createLevelSelectorScene();

    void update(float deltaTime);
    
    static LevelSelector* getInstance() { return _instance; }

    void setupNetworkCallbacks();

private:
    void onPlayClicked(int levelNumber);
    void onOnlinePlayClicked(int levelNumber);
};

#endif
