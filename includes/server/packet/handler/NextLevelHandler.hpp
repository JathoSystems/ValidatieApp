#ifndef VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP

#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "scenes/LevelScene.hpp"
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>

extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

class NextLevelPacketHandler : public IPacketHandler {
private:
    static std::shared_ptr<NetworkSystem> g_network;
    static EventManager* g_eventManager;

public:
    void handle(const Packet &packet) override {
        NextLevelPacket nextLevel;
        nextLevel.getBuffer().setData(packet.getBuffer().getData());
        nextLevel.deserialize();

        int level = nextLevel.getNextLevel();
        auto network = g_network;
        auto eventManager = g_eventManager;

        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push_back([level, network, eventManager]() {
            LevelSwitcher switcher{network, eventManager};
            switcher.openLevel(level, true);
        });
    }

    static void setNetworkAndEventManager(const std::shared_ptr<NetworkSystem> &network, EventManager *eventManager) {
        g_network = network;
        g_eventManager = eventManager;
    }
};

inline std::shared_ptr<NetworkSystem> NextLevelPacketHandler::g_network = nullptr;
inline EventManager* NextLevelPacketHandler::g_eventManager = nullptr;

#endif //VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP