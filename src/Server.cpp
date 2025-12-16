//
// Created by kikker234 on 11-12-2025.
//
#include <iostream>
#include "asio/io_context.hpp"
#include "Network/Server.h"

#include "SpawnEvent.hpp"
#include "characters/Fireboy.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Network/Listeners/TcpNetworkListener.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Events/EventRegistry.h"
#include "GameObjects/ObjectRegistry.hpp"

int main() {
    try {
        asio::io_context io_context;
        int port = 7534;

        // Register packets (we only need NetworkEventPacket now!)
        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);

        // Register events
        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl");
        });

        // Create server
        auto listener = std::make_unique<TcpNetworkListener>(io_context, port, 2);

        Server server(io_context, std::move(listener), port);

        // Set packet callback to handle NetworkEventPackets
        server.setPacketCallback([&server](int32_t clientId, const Packet &packet) {
            // Check if it's a NetworkEventPacket
            if (packet.getId() == 100) {

                // Deserialize the NetworkEventPacket
                NetworkEventPacket eventPacket;
                eventPacket.getBuffer().setData(packet.getBuffer().getData());

                try {
                    eventPacket.deserialize();

                    std::string eventName = eventPacket.getEventName();
                    std::vector<uint8_t> eventData = eventPacket.getEventData();

                    // Create the event from the registry
                    EventRegistry::getInstance()->createEvent(eventName);
                    auto event = EventRegistry::getInstance()->getEvent(eventName);

                    if (event) {
                        event->deserialize(eventData);
                        server.broadcastExcept(packet, clientId);
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Error processing event, broadcasting anyway idfc anymore: " << e.what() << "\n";
                    server.broadcast(packet);
                }
            }
        });

        // 5. Start the server
        server.startServer();
        server.run();
    } catch (std::exception &e) {
        std::cerr << "Server Error: " << e.what() << "\n";
    }

    return 0;
}
