//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "scenes/LevelScene.hpp"
#include "server/packet/RestartPacket.hpp"
#include "LevelSwitcher.hpp"

class RestartLevelPacketHandler : public IPacketHandler {
private:
    static std::shared_ptr<NetworkSystem> g_network;
    static EventManager* g_eventManager;

public:
    void handle(const Packet &packet) override {
        // Now called on main thread - safe to execute directly
        std::cout << "RESTART PACKET RECEIVED\n";
        
        RestartPacket restartPacket;
        restartPacket.getBuffer().setData(packet.getBuffer().getData());
        restartPacket.deserialize();
        
        std::string sceneName = restartPacket.getLevel();
        int levelNumber = 1;
        if (sceneName.find("level_") == 0) {
            size_t pos = 6;
            size_t end = sceneName.find('_', pos);
            std::string numStr = (end != std::string::npos) ? sceneName.substr(pos, end - pos) : sceneName.substr(pos);
            try {
                levelNumber = std::stoi(numStr);
            } catch (...) {
                levelNumber = 1;
            }
        }
        
        LevelSwitcher switcher{g_network, g_eventManager};
        switcher.openLevel(levelNumber, true);
    }
    
    static void setNetworkAndEventManager(const std::shared_ptr<NetworkSystem> &network, EventManager *eventManager) {
        g_network = network;
        g_eventManager = eventManager;
    }
};

inline std::shared_ptr<NetworkSystem> RestartLevelPacketHandler::g_network = nullptr;
inline EventManager* RestartLevelPacketHandler::g_eventManager = nullptr;

#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTLEVELPACKETHANDLER_HPP