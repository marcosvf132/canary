/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2023 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "messages/server.hpp"
#include "server/network/message/networkmessage.hpp"
#include "game/movement/position.hpp"

class NetworkMessage;

class GameServerMessageTalk : public GameServerMessage {
public:
    explicit GameServerMessageTalk(Player &player) : GameServerMessage(player) {
		// Message header type
		m_type = GameServerMessage_t::GAMESERVER_MESSAGE_TYPE_TALK;

		// Unique server messages to prevent duplications and spams, its marked as true by default
		m_unique = false;

		// Used for debug only
        m_name = "Talk";
    }

	std::shared_ptr<GameServerMessage> init(bool traded, uint8_t type, uint16_t level, uint32_t statement, std::string author, std::string text, Position pos) {
		l_traded = traded;
		l_type = type;
		l_level = level;
		l_pos = pos;
		l_statement = statement;
		l_author = std::move(author);
		l_text = std::move(text);
		return shared_from_this();
	}
 
    virtual bool write() {
		m_msg.add<uint32_t>(l_statement);
		m_msg.addString(l_author);
		m_msg.addByte(l_traded ? 1 : 0);
		m_msg.add<uint16_t>(l_level);
		m_msg.addByte(l_type);
		m_msg.addPosition(l_pos);
		m_msg.addString(l_text);
		return true;
	}

private:
	bool l_traded;

	uint8_t l_type;

	uint16_t l_level;

	uint32_t l_statement;

	std::string l_author;
	std::string l_text;

	Position l_pos;
};
