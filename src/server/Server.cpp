//
// Created by kikker234 on 11-12-2025.
//
#include <iostream>
#include "asio/io_context.hpp"
#include "Network/Server.h"

#include "SpawnEvent.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Network/Listeners/TcpNetworkListener.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Events/EventRegistry.h"
#include "server/PlayerManager.hpp"
#include "server/LobbyManager.hpp"
#include "server/packet/GameReady.hpp"
#include "server/packet/PlayerAssignPacket.hpp"
#include "server/packet/CreateLobbyPacket.hpp"
#include "server/packet/JoinLobbyPacket.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "server/packet/QuitPacket.hpp"
#include "server/packet/RestartPacket.hpp"

int main() {
    try {
        asio::io_context io_context;
        int port = 7534;

        // Register packets
        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);
        PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
        PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);
        PacketRegistery::getInstance().registerPacket<CreateLobbyPacket>(103);
        PacketRegistery::getInstance().registerPacket<JoinLobbyPacket>(104);
        PacketRegistery::getInstance().registerPacket<LobbyInfoPacket>(105);
        PacketRegistery::getInstance().registerPacket<QuitPacket>(120);
        PacketRegistery::getInstance().registerPacket<RestartPacket>(121);
        PacketRegistery::getInstance().registerPacket<NextLevelPacket>(122);

        // Register events
        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl", 0.0f, 0.0f);
        });

        // Create server
        auto listener = std::make_unique<TcpNetworkListener>(io_context, port, 10);
        // Allow more connections for multiple lobbies

        Server server(io_context, std::move(listener), port);
        PlayerManager playerManager;
        LobbyManager lobbyManager;

        server.onConnect([&server](int32_t clientId) {
            std::cout << "Player " << clientId << " connected\n";
        });

        // Note: onDisconnect callback is not available in Server class
        // Disconnection cleanup would need to be handled through other means
        // For now, we'll handle it when trying to send packets fails

        // Set packet callback to handle all packets
        server.setPacketCallback([&server, &lobbyManager, &playerManager](int32_t clientId, const Packet &packet) {
            int packetId = packet.getId();

            // Handle CreateLobbyPacket
            if (packetId == 103) {
                CreateLobbyPacket createPacket;
                createPacket.getBuffer().setData(packet.getBuffer().getData());
                createPacket.deserialize();

                int lobbyId = lobbyManager.createLobby(createPacket.levelId, clientId);
                std::cout << "Lobby " << lobbyId << " created for level " << createPacket.levelId << " by player " <<
                        clientId << "\n";

                // Assign fireboy role to first player (lobby creator)
                playerManager.join(clientId, "fireboy");
                PlayerAssignPacket assign("fireboy");
                assign.serialize();
                server.sendToClient(clientId, assign);

                // Send lobby info to creator
                LobbyInfoPacket info(lobbyId, createPacket.levelId, 1, "waiting");
                info.serialize();
                server.sendToClient(clientId, info);
            }
            // Handle JoinLobbyPacket
            else if (packetId == 104) {
                JoinLobbyPacket joinPacket;
                joinPacket.getBuffer().setData(packet.getBuffer().getData());
                joinPacket.deserialize();

                Lobby *lobby = lobbyManager.getLobby(joinPacket.lobbyId);
                if (!lobby && lobby->isFull()) {
                    std::cout << "Lobby " << joinPacket.lobbyId << " not found or full\n";
                    return;
                }

                bool joined = lobbyManager.joinLobby(joinPacket.lobbyId, clientId, joinPacket.levelId);
                if (!joined) {
                    std::cout << "Failed to join lobby " << joinPacket.lobbyId << " for player " << clientId << "\n";
                    return;
                }

                playerManager.join(clientId, "watergirl");
                PlayerAssignPacket assign("watergirl");
                assign.serialize();
                server.sendToClient(clientId, assign);

                LobbyInfoPacket info(lobby->lobbyId, lobby->levelId, lobby->getPlayerCount(),
                                     lobby->isFull() ? "ready" : "waiting");
                info.serialize();
                lobby->broadcastInLobby(info, server);

                if (!lobby->isFull()) return;

                GameReadyPacket ready(lobby->levelId);
                ready.serialize();

                lobby->broadcastInLobby(ready, server);
            } else if (packetId == 120) {
                QuitPacket quitPacket;
                quitPacket.getBuffer().setData(packet.getBuffer().getData());
                quitPacket.deserialize();

                Lobby *lobby = lobbyManager.getLobby(quitPacket.getLobby());
                std::cout << "Disbanding " << std::to_string(quitPacket.getLobby()) << std::endl;
                lobby->broadcastInLobby(quitPacket, server);
                for (int32_t player: lobby->players)
                    lobbyManager.leaveLobby(quitPacket.getLobby(), player);

                lobbyManager.removeLobby(quitPacket.getLobby());
            } else if (packetId == 121) {
                RestartPacket restart;
                restart.getBuffer().setData(packet.getBuffer().getData());
                restart.deserialize();

                Lobby *lobby = lobbyManager.getLobby(restart.getLobby());
                if (!lobby) {
                    std::cerr << "Could not restart, lobby does not exist\n";
                    return;
                }

                std::cout << "Restarting lobby: " << restart.getLobby() << "\n";
                lobby->broadcastInLobby(restart, server);
                // Handle NetworkEventPacket
            } else if (packetId == 122) {
                NextLevelPacket nextLevel;
                nextLevel.getBuffer().setData(packet.getBuffer().getData());
                nextLevel.deserialize();

                Lobby *lobby = lobbyManager.getLobby(nextLevel.getLobby());
                if (!lobby) {
                    std::cerr << "Could not load next level, lobby does not exist\n";
                    return;
                }

                std::cout << "Next level " << nextLevel.getNextLevel()
                        << " for lobby: " << nextLevel.getLobby() << "\n";

                // Update lobby's level
                lobby->levelId = nextLevel.getNextLevel();

                // Broadcast to all players in lobby
                lobby->broadcastInLobby(nextLevel, server);
            } else if (packetId == 100) {
                // Deserialize the NetworkEventPacket
                std::cout << "[SERVER] Received event from client " << clientId << std::endl;

                NetworkEventPacket eventPacket;
                eventPacket.getBuffer().setData(packet.getBuffer().getData());
                eventPacket.deserialize();

                std::cout << "[SERVER] Event: " << eventPacket.getEventName() << std::endl;

                try {
                    eventPacket.deserialize();

                    std::string eventName = eventPacket.getEventName();
                    std::vector<uint8_t> eventData = eventPacket.getEventData();

                    // Create the event from the registry
                    EventRegistry::getInstance()->createEvent(eventName);
                    auto event = EventRegistry::getInstance()->getEvent(eventName);

                    if (event) {
                        event->deserialize(eventData);
                        // Broadcast to other players in the same lobby
                        int lobbyId = lobbyManager.getLobbyIdForPlayer(clientId);
                        std::cout << "[SERVER] ClientID: " << clientId << " LobbyID: " << lobbyId << std::endl;

                        if (lobbyId > 0) {
                            Lobby *lobby = lobbyManager.getLobby(lobbyId);
                            if (lobby) {
                                std::cout << "[SERVER] Broadcasting to lobby " << lobbyId
                                          << " (" << lobby->players.size() << " players)" << std::endl;
                                for (int32_t playerId: lobby->players) {
                                    std::cout << "[SERVER] PlayerID: " << playerId
                                              << (playerId == clientId ? " (SENDER - SKIP)" : " (SEND)") << std::endl;
                                    if (playerId != clientId) {
                                        server.sendToClient(playerId, packet);
                                    }
                                }
                            }
                        } else {
                            server.broadcastExcept(packet, clientId);
                        }
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Error processing event, broadcasting anyway idfc anymore: " << e.what() << "\n";
                    server.broadcastExcept(packet, clientId);
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
