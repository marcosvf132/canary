/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2023 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include <memory>

#include "declarations.hpp"
#include "messages/game_message.hpp"
#include "server/network/message/networkmessage.hpp"

class NetworkMessage;

class GameClientMessage : public GameMessage, public std::enable_shared_from_this<GameClientMessage> {
public:
    explicit GameClientMessage(Player &player, NetworkMessage& msg) : GameMessage(player) {
        m_msg = msg;
    }

    // Read NetworkMessage buffer
    virtual bool read() { return false; }

    // Called from dispatcher
    virtual void parse() { /*void*/ }

    virtual std::shared_ptr<GameClientMessage> getClientPacket() override { return shared_from_this(); }

    virtual bool canParse() {
        // Use the 'm_player' member directly instead of 'packer->getReference()'
        return m_player.mustAcceptIncomingPackets() && !m_player.isRemoved() && m_player.getHealth() > 0;
    }

protected:
    GameClientMessage_t m_type = GameClientMessage_t::GAMECLIENT_MESSAGE_TYPE_TALK;
};
