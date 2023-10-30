/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2023 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "game/game.hpp"
#include "messages/client.hpp"
#include "server/network/message/networkmessage.hpp"

class NetworkMessage;

class GameClientMessageTalk : public GameClientMessage {
public:
	explicit GameClientMessageTalk(Player &player, NetworkMessage& msg) : GameClientMessage(player, msg) {
		// Message header type
        m_type = GameClientMessage_t::GAMECLIENT_MESSAGE_TYPE_TALK;

		// Used for debug only
        m_name = "Talk";
    }

	// Read NetworkMessage buffer
    virtual bool read() {
        l_type = m_msg.getByte();
        switch (l_type) {
            case 5:
            case 16: {
                l_receiver = m_msg.getString();
                break;
            }

            case 7:
            case 14: {
                l_channel = m_msg.get<uint16_t>();
                break;
            }

            default:
                break;
        }

        l_text = m_msg.getString();
        return true;
    }

	virtual void parse() {
        g_game().playerSay(m_player.getID(), l_channel, static_cast<SpeakClasses>(l_type), l_receiver, l_text);
    }

private:
    uint8_t l_type = 0;

    uint16_t l_channel = 0;

    std::string l_receiver;
    std::string l_text;
};
