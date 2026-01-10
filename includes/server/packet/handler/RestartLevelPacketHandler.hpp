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

            // Check if we're already on the level scene
            if (currentScene && currentScene->getName() == sceneName) {
                // We're already on the level scene, just do soft reset
                if (auto* levelScene = dynamic_cast<LevelScene*>(currentScene)) {
                    std::cout << "[RestartLevelPacketHandler] Already on level scene, doing soft reset only...\n";
                    levelScene->resetLevel();
                    std::cout << "[RestartLevelPacketHandler] Soft reset complete (no scene switch needed)\n";
                }
            } else {
                // We're on the restart screen, need to switch back to level
                Scene* scene = sceneSystem->getScene(sceneName);
                if (auto* levelScene = dynamic_cast<LevelScene*>(scene)) {
                    std::cout << "[RestartLevelPacketHandler] Switching from restart screen, doing soft reset then switch...\n";

                    // Do soft reset first
                    levelScene->resetLevel();

                    // Then switch (onInitialRender will early-return)
                    sceneSystem->setScene(sceneName);

                    std::cout << "[RestartLevelPacketHandler] Restart complete\n";
                } else {
                    std::cout << "[RestartLevelPacketHandler] ERROR: Scene not found or not a LevelScene\n";
                }
            }
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP