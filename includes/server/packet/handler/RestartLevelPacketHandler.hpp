//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#include "characters/BaseCharacter.hpp"
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Scenes/SceneSystem.h"
#include "server/packet/RestartPacket.hpp"

class RestartLevelPacketHandler : public IPacketHandler {
    void handle(const Packet &packet) override {
        std::cout << "INCOMING\n";
        if (const RestartPacket* p = dynamic_cast<const RestartPacket*>(&packet)) {
            std::string sceneName = p->getLevel();
            GameEngine *engine = &GameEngine::getInstance();
            SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();
            Scene *scene = sceneSystem->getScene(sceneName);

            for (std::unique_ptr<GameObject> &game_object: scene->getObjects()) {
                if (BaseCharacter *base = dynamic_cast<BaseCharacter *>(game_object.get()))
                    base->destroy();
            }

            sceneSystem->setScene(sceneName);
        }
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP