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

#include "SpawnEvent.hpp"
#include "scenes/RestartScene.hpp"
#include "server/packet/QuitPacket.hpp"

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
    void resetLevel();
    void reachedDoor() {
        // 1. Voorkom dat we dit dubbel doen (als physics update blijft vuren)
        if (_levelFinished) return;

        _peopleAtDoor++;

        // Cap de teller op 2 (voor de zekerheid)
        if (_peopleAtDoor > 2) _peopleAtDoor = 2;

        std::cout << "[LevelScene] Character reached door. Total: " << _peopleAtDoor << "/2" << std::endl;

        // 2. Als beide spelers er zijn...
        if (_peopleAtDoor >= 2) {
            _levelFinished = true; // Zet slot erop

            std::cout << "[LevelScene] Level Finished! Switching to Restart Screen..." << std::endl;

            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem* sceneSystem = engine->getSystem<SceneSystem>();

            if (sceneSystem) {
                // A. Zoek de Restart Scene en stel hem in
                Scene* s = sceneSystem->getScene("Restart");
                if (auto* restartScene = dynamic_cast<RestartScene*>(s)) {
                    restartScene->setTargetLevel(getName()); // Vertel welk level we net deden
                    restartScene->setOnline(_isOnline);
                }

                // B. Schakel over naar het Restart Scherm
                sceneSystem->setScene("Restart");
            }
        }
    }

    void leftDoor() {
        if (_peopleAtDoor > 0) {
            _peopleAtDoor--;
            std::cout << "Character left door. People at door: " << _peopleAtDoor << "/2" << std::endl;
        }
    }
    void cleanup();

private:
    void createBasicLevelGrid();

    void checkDoorCollisions();

    void setupLevel();

    void setupCharacters();

    void setupHUD();

    void updateDiamondCounters();

    void createBat();

    int _levelNumber;
    bool _isOnline;
    std::shared_ptr<NetworkSystem> _network;
    EventManager *_eventManager;
    bool _isInitialized;
    bool _batCreated;
    int _batCount;
    bool _levelFinished = false;

    Fireboy *_fireboy = nullptr;
    Watergirl *_watergirl = nullptr;

    Text *_fireboyDiamondText = nullptr;
    Text *_watergirlDiamondText = nullptr;

    int _peopleAtDoor = 0;
    std::vector<Door*> _doors;
};

#endif
