#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Network/NetworkSystem.h"
#include "Scenes/SceneSystem.h"
#include "Network/GameState.hpp"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "QUIT PACKET RECEIVED" << std::endl;

        auto *sceneSystem = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!sceneSystem) return;

        std::string currentSceneName;
        Scene *currentScene = sceneSystem->getActiveSceneObj();
        if (currentScene) {
            currentSceneName = currentScene->getName();
        }

        NetworkSystem *networkSystem = GameEngine::getInstance().getSystem<NetworkSystem>();
        if (networkSystem && networkSystem->getMiddleware()) {
            networkSystem->getMiddleware()->clearPacketQueue();
        }
        GameState::getInstance().clear();

        sceneSystem->setScene("MainMenu");

        if (!currentSceneName.empty() && currentSceneName.find("level_") == 0) {
            sceneSystem->removeScene(currentSceneName);
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
