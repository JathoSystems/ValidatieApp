//
// Created by kikker234 on 11-12-2025.
//
#include <iostream>
#include "asio/io_context.hpp"
#include "Network/Server.h"

#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Network/Listeners/TcpNetworkListener.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Network/Packet/Packets/PlayerAssignPacket.h"
#include "Network/Packet/Packets/GameReadyPacket.h"
#include "Events/EventRegistry.h"
#include <unordered_map>
#include <vector>

int main() {
    try {
        asio::io_context io_context;
        int port = 7534;

        // Register packets
        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);

        // Register events
        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        // Create server
        auto listener = std::make_unique<TcpNetworkListener>(io_context, port, 2);

        Server server(io_context, std::move(listener), port);

        // Role list for this game (join order defines role)
        std::vector<std::string> roles = {"fireboy", "watergirl"};
        std::unordered_map<int32_t, std::string> clientRoles;

        // Assign roles in order of connection and signal ready when all filled
        server.setClientConnectedCallback([&server, &roles, &clientRoles](int32_t clientId) {
            if (clientRoles.size() < roles.size()) {
                std::string role = roles[clientRoles.size()];
                clientRoles[clientId] = role;

                std::cout << "Assigning role '" << role << "' to client " << clientId << "\n";

                PlayerAssignPacket assign(role);
                assign.serialize();
                server.sendToClient(clientId, assign);
            }

            if (clientRoles.size() == roles.size()) {
                std::cout << "All roles assigned, sending GameReadyPacket\n";
                GameReadyPacket ready;
                ready.serialize();
                server.broadcast(ready);
            }
        });

        // Set packet callback to handle NetworkEventPackets
        server.setPacketCallback([&server](int32_t clientId, const Packet& packet) {
            // Check if it's a NetworkEventPacket
            if (packet.getId() == 100) {
                std::cout << "NetworkEventPacket ontvangen van client " << clientId << "\n";

                // Deserialize the NetworkEventPacket
                NetworkEventPacket eventPacket;
                eventPacket.getBuffer().setData(packet.getBuffer().getData());

                try {
                    eventPacket.deserialize();

                    std::string eventName = eventPacket.getEventName();
                    std::vector<uint8_t> eventData = eventPacket.getEventData();

                    std::cout << "Event type: " << eventName << "\n";

                    // Create the event from the registry
                    EventRegistry::getInstance()->createEvent(eventName);
                    auto event = EventRegistry::getInstance()->getEvent(eventName);

                    if (event) {
                        event->deserialize(eventData);

                        std::cout << "[Client " << clientId << " jumped]\n";

                        // Broadcast to all other clients
                        server.broadcast(packet);
                        std::cout << "Event broadcasted to other clients\n";
                    }

                } catch (const std::exception& e) {
                    std::cerr << "Error processing event, broadcasting anyway idfc anymore: " << e.what() << "\n";
                    server.broadcast(packet);
                }
            } else {
                std::cout << "Unknown packet type: " << packet.getId() << "\n";
            }
        });

        // 5. Start the server
        server.startServer();

        std::cout << "=================================\n";
        std::cout << "Server running on port " << port << "!\n";
        std::cout << "Using Event System!\n";
        std::cout << "Press Ctrl+C to stop\n";
        std::cout << "=================================\n";

        server.run();

    } catch (std::exception& e) {
        std::cerr << "Server Error: " << e.what() << "\n";
    }

    return 0;
}
