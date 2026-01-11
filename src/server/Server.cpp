#include <iostream>
#include "asio/io_context.hpp"
#include "Network/Server.h"

#include "SpawnEvent.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "bat/events/BatMoveEvent.hpp"
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

std::map<int, std::function<std::unique_ptr<Scene>()> > g_levels;

int main() {
    try {
        asio::io_context io_context;
        int port = 7534;

        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);
        PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
        PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);
        PacketRegistery::getInstance().registerPacket<CreateLobbyPacket>(103);
        PacketRegistery::getInstance().registerPacket<JoinLobbyPacket>(104);
        PacketRegistery::getInstance().registerPacket<LobbyInfoPacket>(105);
        PacketRegistery::getInstance().registerPacket<QuitPacket>(120);
        PacketRegistery::getInstance().registerPacket<RestartPacket>(121);
        PacketRegistery::getInstance().registerPacket<NextLevelPacket>(122);

        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl", 0.0f, 0.0f);
        });

        EventRegistry::getInstance()->registerEvent("batmove", []() {
            return std::make_shared<BatMoveEvent>(0, 0.0f, 0.0f);
        });

        auto listener = std::make_unique<TcpNetworkListener>(io_context, port, 10);

        Server server(io_context, std::move(listener), port);
        PlayerManager playerManager;
        LobbyManager lobbyManager;

        server.onConnect([&server](int32_t clientId) {
            std::cout << "Player " << clientId << " connected\n";
        });

        server.setPacketCallback([&server, &lobbyManager, &playerManager](int32_t clientId, const Packet &packet) {
            int packetId = packet.getId();

            if (packetId == 103) {
                CreateLobbyPacket createPacket;
                createPacket.getBuffer().setData(packet.getBuffer().getData());
                createPacket.deserialize();

                int existingLobby = lobbyManager.getLobbyIdForPlayer(clientId);
                if (existingLobby > 0) {
                    std::cout << "Player " << clientId << " leaving existing lobby " << existingLobby <<
                            " before creating new one\n";
                    lobbyManager.leaveLobby(existingLobby, clientId);
                    playerManager.leave(clientId);
                }

                int lobbyId = lobbyManager.createLobby(createPacket.levelId, clientId);
                std::cout << "Lobby " << lobbyId << " created for level " << createPacket.levelId << " by player " <<
                        clientId << "\n";

                playerManager.join(clientId, "fireboy");
                PlayerAssignPacket assign("fireboy");
                assign.serialize();
                server.sendToClient(clientId, assign);

                LobbyInfoPacket info(lobbyId, createPacket.levelId, 1, "waiting");
                info.serialize();
                server.sendToClient(clientId, info);
            } else if (packetId == 104) {
                JoinLobbyPacket joinPacket;
                joinPacket.getBuffer().setData(packet.getBuffer().getData());
                joinPacket.deserialize();

                int existingLobby = lobbyManager.getLobbyIdForPlayer(clientId);
                if (existingLobby > 0) {
                    std::cout << "Player " << clientId << " leaving existing lobby " << existingLobby <<
                            " before joining new one\n";
                    lobbyManager.leaveLobby(existingLobby, clientId);
                    playerManager.leave(clientId);
                }

                Lobby *lobby = lobbyManager.getLobby(joinPacket.lobbyId);
                if (!lobby || lobby->isFull()) {
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

                int lobbyId = quitPacket.getLobby();

                if (lobbyId <= 0) {
                    lobbyId = lobbyManager.getLobbyIdForPlayer(clientId);
                }

                if (lobbyId <= 0) {
                    std::cout << "Player " << clientId << " tried to quit but is not in any lobby\n";
                    return;
                }

                Lobby *lobby = lobbyManager.getLobby(lobbyId);
                if (!lobby) {
                    std::cout << "Lobby " << lobbyId << " not found for quit\n";
                    playerManager.leave(clientId);
                    return;
                }

                std::cout << "Disbanding lobby " << lobbyId << " (player " << clientId << " quit)\n";

                lobby->broadcastInLobby(quitPacket, server);

                for (int32_t player: lobby->players) {
                    playerManager.leave(player);
                }

                lobbyManager.removeLobby(lobbyId);
            } else if (packetId == 121) {
                RestartPacket restart;
                restart.getBuffer().setData(packet.getBuffer().getData());
                restart.deserialize();

                Lobby *lobby = lobbyManager.getLobby(restart.getLobby());
                if (!lobby) {
                    std::cerr << "Could not restart, lobby does not exist\n";
                    return;
                }

                std::cout << "Restarting lobby: " << restart.getLobby() << ", level: " << restart.getLevel() << "\n";
                restart.serialize();
                lobby->broadcastInLobby(restart, server);
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

                lobby->levelId = nextLevel.getNextLevel();

                lobby->broadcastInLobby(nextLevel, server);
            } else if (packetId == 100) {
                std::cout << "[SERVER] Received event from client " << clientId << std::endl;

                NetworkEventPacket eventPacket;
                eventPacket.getBuffer().setData(packet.getBuffer().getData());
                eventPacket.deserialize();

                std::cout << "[SERVER] Event: " << eventPacket.getEventName() << std::endl;

                try {
                    eventPacket.deserialize();

                    std::string eventName = eventPacket.getEventName();
                    std::vector<uint8_t> eventData = eventPacket.getEventData();

                    EventRegistry::getInstance()->createEvent(eventName);
                    auto event = EventRegistry::getInstance()->getEvent(eventName);

                    if (event) {
                        event->deserialize(eventData);
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

        server.startServer();
        server.run();
    } catch (std::exception &e) {
        std::cerr << "Server Error: " << e.what() << "\n";
    }

    return 0;
}
