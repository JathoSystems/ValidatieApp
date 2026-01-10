#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP

#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Scenes/SceneSystem.h"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "[QuitLevelPacketHandler] QUIT PACKET RECEIVED" << std::endl;

        auto* engine = &GameEngine::getInstance();
        auto* sceneSystem = engine->getSystem<SceneSystem>();

        Scene* currentScene = sceneSystem->getActiveSceneObj();
        std::string levelToRemove = "";

        if (currentScene) {
            std::string name = currentScene->getName();
            if (name.find("level_") != std::string::npos) {
                levelToRemove = name;
            }
        }

        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");

        sceneSystem->setScene("MainMenu");

        if (!levelToRemove.empty()) {
            std::cout << "[QuitLevelPacketHandler] Removing old scene: " << levelToRemove << std::endl;
            sceneSystem->removeScene(levelToRemove);
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP