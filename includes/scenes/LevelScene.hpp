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
    bool _batCreated;
    int _batCount;  // Track bat index for fixed IDs (bat 1 = ID 1, bat 2 = ID 2, etc.)

public:
    LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network, EventManager* eventManager);
    ~LevelScene();
    
    void onInitialRender() override;
private:
    void setupLevel();
    void setupCharacters();
    void createBasicLevelGrid();
    void createBat();
};

#endif
