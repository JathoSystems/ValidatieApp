#ifndef VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP
#define VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP

#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"
#include "LevelGrid.h"
#include "LevelSwitcher.hpp"
#include "server/packet/NextLevelPacket.hpp"

class LevelGrid;

class LevelScene : public Scene {
public:
    explicit LevelScene(int levelNumber, bool isOnline = false,
                        std::shared_ptr<NetworkSystem> network = nullptr,
                        EventManager *eventManager = nullptr);
    
    ~LevelScene();

    void onInitialRender() override;

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
        }
    }

private:
    void createBasicLevelGrid();
    void setupLevel();
    void setupCharacters();

    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager *_eventManager;
    std::unique_ptr<LevelGrid> _levelGrid;
    int _peopleAtDoor = 0;
};

#endif
