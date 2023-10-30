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
#include "server/network/message/networkmessage.hpp"
#include "messages/game_message.hpp"

class NetworkMessage;

class GameServerMessage : public GameMessage, public std::enable_shared_from_this<GameServerMessage> {
public:
	explicit GameServerMessage(Player &player) :
		GameMessage(player), m_unique(true), m_abort(false) { }

	// Construct NetworkMessage buffer
	virtual bool write() {
		return true;
	}

	// Construct NetworkMessage header
	bool writeHeader() {
		m_msg.reset();
		if (m_type != GameServerMessage_t::GAMESERVER_MESSAGE_TYPE_UNKNOWN) {
			m_msg.addByte(static_cast<uint8_t>(m_type));
			return true;
		}

		return false;
	}

	// Variable to prevent unnecessary packets sent to the client when it is marked as 'unique' on subclass constructor
	bool isUnique() const {
		return m_unique;
	}

	// Abort when duplicated packets are in the pool to prevent duplications and maintain the queue priority by the last element
	bool isAborted() const {
		return m_abort;
	}

	// Identifier to the dispatcher sender
	void abort() {
		m_abort = true;
	}

	const GameServerMessage_t &getType() const {
		return m_type;
	}

	bool append(NetworkMessage &msg) {
		if (msg.getLength() == 0) {
			return false;
		}

		for (int i = 1; i <= msg.getLength(); i++) {
			m_msg.addByte(msg.getBuffer()[i + 8 - 1]);
		}
		// m_msg.addBytes(static_cast<const char*>(msg.getBuffer() + 8), msg.getLength());
		return true;
	}

	void send() {
		if (m_msg.isOverrun() || m_msg.getLength() == 0) {
			return;
		}
		m_player.sendNetworkMessage(m_msg);
	}

	virtual std::shared_ptr<GameServerMessage> getServerPacket() override {
		return shared_from_this();
	}

protected:
	GameServerMessage_t m_type = GameServerMessage_t::GAMESERVER_MESSAGE_TYPE_UNKNOWN;
	bool m_abort;
	bool m_unique;
};
