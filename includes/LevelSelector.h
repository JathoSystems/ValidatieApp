#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <memory>
#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"

class LevelSelector {
private:
    SceneSystem *_sceneSystem;
    std::shared_ptr<NetworkSystem> _network;
    EventManager* _eventManager;
    bool _networkCallbacksSetup = false;

public:
    LevelSelector(SceneSystem *sceneSystem, std::shared_ptr<NetworkSystem> network, EventManager* eventManager);

    void createLevelSelectorScene();
    void setupNetworkCallbacks();

private:
    void onPlayClicked(int levelNumber);
    void onOnlinePlayClicked(int levelNumber);
};

#endif
