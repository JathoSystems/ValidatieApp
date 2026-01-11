#ifndef VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP
#define VUURJONGEN_WATERMEISJE_LEVELSCENE_HPP

#include "Scenes/Scene.h"
#include "Network/NetworkSystem.h"
#include "Events/EventManager.h"
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>

#include "LevelSaver.hpp"
#include "LevelSwitcher.hpp"
#include "GameObjects/Component/AudioComponent.h"
#include "server/packet/NextLevelPacket.hpp"


class Fireboy;
class Watergirl;
class Text;
class Door;
class LevelGrid;

class LevelScene : public Scene {
public:
    LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network,
               EventManager *eventManager);

    virtual ~LevelScene();

    void toggleOnline(std::shared_ptr<NetworkSystem> network,
               EventManager *eventManager) {
        _isOnline = true;
        _network = network;
        _eventManager = eventManager;
    }

    void onExit() override {
        GameEngine::getInstance().getSystem<AudioSystem>()->stopMusic();
    }

    void onInitialRender() override final;
    void onUpdate(float deltaTime) override final;

    Fireboy *getFireboy(Scene *scene);

    Watergirl *getWatergirl(Scene *scene);

    int getPeopleAtDoor() const { return _peopleAtDoor; }
    void incrementPeopleAtDoor() { _peopleAtDoor++; }
    void reachedDoor() {
        _peopleAtDoor++;

        if (_peopleAtDoor >= 2 && !_levelCompleted) {
            _levelCompleted = true;
            _levelEndTime = _elapsedTime;

            LevelSaver saver;
            saver.save(_levelNumber, _levelEndTime);


            audio->setVolume(0.0f);

            if (_isOnline && _network) {
                int nextLevel = _levelNumber + 1;

                // NextLevelPacket packet(nextLevel);
                // packet.serialize();
                // _network->send(packet);
                // return;
            }

            // LevelSwitcher switcher{_network, _eventManager};
            // switcher.openLevel(_levelNumber + 1, false);
        }
    }

    void leftDoor() {
        if (_peopleAtDoor > 0) {
            _peopleAtDoor--;
            std::cout << "Character left door. People at door: " << _peopleAtDoor << "/2" << std::endl;
        }
    }

protected:
    // Abstract methods that each level must implement
    virtual void createLevelGrid() = 0;
    virtual void setupLevelSpecifics() = 0;
    virtual std::string getLevelName() const = 0;

    // Helper methods available to all levels
    void createGroundBlock(LevelGrid* grid, int x, int y);
    void createCellObjects(LevelGrid* grid);
    void setupBaseLevel();
    void setupCharacters();
    void setupHUD();
    void createBat();

    // Common level data
    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager* _eventManager;

    Fireboy* _fireboy;
    Watergirl* _watergirl;
    Text* _fireboyDiamondText;
    Text* _watergirlDiamondText;
    std::vector<Door*> _doors;
    int _peopleAtDoor;
    int _batCount;
    std::unique_ptr<AudioComponent> audio;

private:
    void cleanup();
    void checkDiamondCollisions();
    void updateDiamondCounters();
    void checkDoorCollisions();

    // Level timing
    float _elapsedTime;
    float _levelEndTime;
    bool _levelCompleted;

    bool _isInitialized;
    bool _batCreated;
};
#endif