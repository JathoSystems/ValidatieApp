//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "scenes/LevelScene.hpp"
#include "server/packet/RestartPacket.hpp"

class RestartLevelPacketHandler : public IPacketHandler {
    void handle(const Packet &packet) override {
        std::cout << "[RestartLevelPacketHandler] INCOMING RESTART PACKET\n";

        if (const RestartPacket* p = dynamic_cast<const RestartPacket*>(&packet)) {
            std::string sceneName = p->getLevel();
            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();

            // Get the current active scene
            Scene* currentScene = sceneSystem->getActiveSceneObj();

            // Get the level scene (might not be active if we're on restart screen)
            Scene* scene = sceneSystem->getScene(sceneName);

            if (auto* levelScene = dynamic_cast<LevelScene*>(scene)) {
                std::cout << "[RestartLevelPacketHandler] Performing soft reset..." << std::endl;

                // Do soft reset
                levelScene->resetLevel();

                // Switch to level if not already there
                if (!currentScene || currentScene->getName() != sceneName) {
                    std::cout << "[RestartLevelPacketHandler] Switching to level scene" << std::endl;
                    sceneSystem->setScene(sceneName);
                }

                std::cout << "[RestartLevelPacketHandler] Restart complete" << std::endl;
            } else {
                std::cout << "[RestartLevelPacketHandler] ERROR: Level scene not found: " << sceneName << std::endl;
            }
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP