/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2023 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "creatures/players/player.hpp"
#include "server/network/message/networkmessage.hpp"

class GameServerMessage;
class GameClientMessage;
class NetworkMessage;
class Player;

class GameMessage {
public:
	explicit GameMessage(Player &player) :
		m_player(player), m_name("Unknown") { }

	uint32_t getIdentifier() const {
		return m_player.getGUID();
	}

	const Player &getReference() const {
		return m_player;
	}

	NetworkMessage &getMessage() {
		return m_msg;
	}

	virtual std::shared_ptr<GameServerMessage> getServerPacket() {
		return nullptr;
	}

	virtual std::shared_ptr<GameClientMessage> getClientPacket() {
		return nullptr;
	}

	const std::string &getName() const {
		return m_name;
	}

protected:
	NetworkMessage m_msg;
	std::string m_name;

	friend class Player;
	Player &m_player;
};
