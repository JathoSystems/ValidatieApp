//
// Created by kikker234 on 11-12-2025.
//
#include <iostream>
#include "asio/io_context.hpp"
#include "Network/Server.h"

#include "characters/events/JumpEvent.h"
#include "Network/Listeners/TcpNetworkListener.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Events/EventRegistry.h"

int main() {
    asio::io_context io_context;
    int port = 7534;
    EventRegistry::getInstance()->registerEvent("JumpEvent", []() {
        return std::make_shared<JumpEvent>();
    });

    auto listener = std::make_unique<TcpNetworkListener>(io_context, port, 100);

    Server server(io_context, std::move(listener), port);

    server.setPacketCallback([&server](int32_t clientId, const Packet& packet) {
        std::cout << clientId << "\n";

        // for (int i : packet.getBuffer().getData()) {
            // std::cout << i << "\n";
        // }

        server.broadcast(packet);
    });
    server.startServer();

    std::cout << "=================================\n";
    std::cout << "Server running on port " << port << "\n";
    std::cout << "Using Event System!\n";
    std::cout << "Press Ctrl+C to stop\n";
    std::cout << "=================================\n";

    server.run();

    return 0;
}