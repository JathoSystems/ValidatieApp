#ifndef VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP
#define VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP

#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"

class LevelGrid;

class LevelScene : public Scene {
private:
    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager* _eventManager;
    std::unique_ptr<LevelGrid> _levelGrid;

public:
    LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network, EventManager* eventManager);
    ~LevelScene();
    
    void onInitialRender() override;
private:
    void setupLevel();
    void setupCharacters();
    void createBasicLevelGrid();
};

#endif
