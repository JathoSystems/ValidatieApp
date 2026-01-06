#ifndef VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP
#define VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP

#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"
#include "LevelGrid.h"
#include "UI/Text.h"

class LevelGrid;
class Fireboy;
class Watergirl;

class LevelScene : public Scene {
public:
    explicit LevelScene(int levelNumber, bool isOnline = false,
                       std::shared_ptr<NetworkSystem> network = nullptr,
                       EventManager* eventManager = nullptr);

    void onInitialRender() override;
    void onUpdate(float deltaTime) override;

    void checkDiamondCollisions();
    void resetCharacterPointers() {
        _fireboy = nullptr;
        _watergirl = nullptr;
        _fireboyDiamondText = nullptr;
        _watergirlDiamondText = nullptr;
    }

private:
    void createBasicLevelGrid();
    void setupLevel();
    void setupCharacters();
    void setupHUD();
    void cleanup();
    void updateDiamondCounters();

    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager* _eventManager;
    std::unique_ptr<LevelGrid> _levelGrid;
    bool _isInitialized;
    // Pointers naar characters voor diamond counting
    Fireboy* _fireboy = nullptr;
    Watergirl* _watergirl = nullptr;

    // Pointers naar diamond counter text elements
    Text* _fireboyDiamondText = nullptr;
    Text* _watergirlDiamondText = nullptr;
};

#endif
