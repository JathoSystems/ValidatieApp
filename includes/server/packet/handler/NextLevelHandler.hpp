#ifndef VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP

#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "scenes/LevelScene.hpp"
#include <iostream>
#include <memory>

class NextLevelPacketHandler : public IPacketHandler {
private:
    static std::shared_ptr<NetworkSystem> g_network;
    static EventManager* g_eventManager;

public:
    void handle(const Packet &packet) override {
        NextLevelPacket nextLevel;
        nextLevel.getBuffer().setData(packet.getBuffer().getData());
        nextLevel.deserialize();

        LevelSwitcher switcher {g_network, g_eventManager};
        switcher.openLevel(nextLevel.getNextLevel(), true);
    }

    static void setNetworkAndEventManager(const std::shared_ptr<NetworkSystem> &network, EventManager *eventManager) {
        g_network = network;
        g_eventManager = eventManager;
    }
};

// Static member initialization (put this in the .cpp file if you have one)
inline std::shared_ptr<NetworkSystem> NextLevelPacketHandler::g_network = nullptr;
inline EventManager* NextLevelPacketHandler::g_eventManager = nullptr;

#endif //VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP