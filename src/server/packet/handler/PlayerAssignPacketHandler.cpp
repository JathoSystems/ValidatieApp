#include "server/packet/handler/PlayerAssignPacketHandler.hpp"

#include "Network/GameState.hpp"
#include "../../../../includes/server/packet/PlayerAssignPacket.hpp"

void PlayerAssignPacketHandler::handle(const Packet &packet) {
    PlayerAssignPacket assign;
    assign.getBuffer().setData(packet.getBuffer().getData());
    assign.deserialize();

    GameState::getInstance().set("role", assign.getRole());
}
