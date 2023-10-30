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

class GameMessage;
class NetworkMessage;

using PacketsPool = std::vector<std::shared_ptr<GameMessage>>;
using PacketsBatch = std::map<uint32_t, std::unique_ptr<GameMessage>>;
using PacketsBatches = std::map<uint32_t, PacketsPool>;

class GameMessages {
public:
	GameMessages() { }

    // Ensures that we don't accidentally copy it
    GameMessages(const GameMessages &) = delete;
    GameMessages operator=(const GameMessages &) = delete;
    static GameMessages &getInstance();

	void addToPool(std::shared_ptr<GameMessage> packet);
	void abort(GameServerMessage_t type, uint32_t playerId);
    bool parsePacketFromInput(NetworkMessage& msg, Player &player, uint8_t header);

private:
    void serverCycle();
    void clientCycle();

    PacketsPool m_server;
    PacketsPool m_client;

    // Make it thread safe since client packets initially comes from protocol thread
    std::mutex m_serverThreadSafe;
    std::mutex m_clientThreadSafe;
};

constexpr auto g_messages = GameMessages::getInstance;