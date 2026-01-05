//
// Created for lobby system
//

#ifndef VUURJONGEN_WATERMEISJE_LOBBYINFOPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_LOBBYINFOPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include <memory>

class NetworkSystem;
class EventManager;

class LobbyInfoPacketHandler : public IPacketHandler {
public:
    static void setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network, EventManager* eventManager);
    void handle(const Packet &packet) override;
};

#endif //VUURJONGEN_WATERMEISJE_LOBBYINFOPACKETHANDLER_HPP
