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
        std::cout << "INCOMING\n";
        if (const RestartPacket* p = dynamic_cast<const RestartPacket*>(&packet)) {
            std::string sceneName = p->getLevel();
            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
            
            // Switch to the level scene - it will reinitialize itself
            sceneSystem->setScene(sceneName);
        }
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP