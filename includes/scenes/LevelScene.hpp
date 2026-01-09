#ifndef VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP
#define VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP

#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"
#include "grid/LevelGrid.h"
#include "LevelSwitcher.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "UI/Text.h"
#include <vector>

class LevelGrid;
class Fireboy;
class Watergirl;
class Door;

class LevelScene : public Scene {
public:
    explicit LevelScene(int levelNumber, bool isOnline = false,
                        std::shared_ptr<NetworkSystem> network = nullptr,
                        EventManager *eventManager = nullptr);

    ~LevelScene();

    void onInitialRender() override;

    void onUpdate(float deltaTime) override;

    void checkDiamondCollisions();

    void resetCharacterPointers() {
        _fireboy = nullptr;
        _watergirl = nullptr;
        _fireboyDiamondText = nullptr;
        _watergirlDiamondText = nullptr;
    }

    void reachedDoor() {
        _peopleAtDoor++;

        if (_peopleAtDoor >= 2) {
            if (_isOnline && _network) {
                int nextLevel = _levelNumber + 1;

                NextLevelPacket packet(nextLevel);
                packet.serialize();
                _network->send(packet);
                return;
            }

            LevelSwitcher switcher{_network, _eventManager};
            switcher.openLevel(_levelNumber + 1, false);
        }
    }

    void leftDoor() {
        if (_peopleAtDoor > 0) {
            _peopleAtDoor--;
            std::cout << "Character left door. People at door: " << _peopleAtDoor << "/2" << std::endl;
        }
    }

private:
    void createBasicLevelGrid();

    void checkDoorCollisions();

    void setupLevel();

    void setupCharacters();

    void setupHUD();

    void cleanup();

    void updateDiamondCounters();

    void createBat();

    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager *_eventManager;
    bool _isInitialized;
    bool _batCreated;
    int _batCount;

    Fireboy *_fireboy = nullptr;
    Watergirl *_watergirl = nullptr;

    Text *_fireboyDiamondText = nullptr;
    Text *_watergirlDiamondText = nullptr;

    int _peopleAtDoor = 0;
    std::vector<Door*> _doors;
};

#endif
