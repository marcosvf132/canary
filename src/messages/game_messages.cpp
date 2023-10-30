/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2023 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#include "pch.hpp"

#include "core.hpp"
#include "declarations.hpp"
#include "game/game.hpp"
#include "game/scheduling/dispatcher.hpp"
#include "messages/game_messages.hpp"
#include "messages/game_message.hpp"
#include "messages/server.hpp"
#include "messages/client.hpp"
#include "server/network/message/networkmessage.hpp"

// From protocol thread and dispatcher thread
void GameMessages::addToPool(std::shared_ptr<GameMessage> packet) {
    if (auto server = packet->getServerPacket()) {
        std::lock_guard<std::mutex> lockClass(m_serverThreadSafe);
        if (server->isUnique()) {
            abort(server->getType(), packet->getIdentifier());
        }

        if (m_server.empty()) {
            g_dispatcher().addTask(std::bind(&GameMessages::serverCycle, this), "GameMessages::serverCycle");
        }

        m_server.push_back(server);
    } else if (auto client = packet->getClientPacket()) {
        std::lock_guard<std::mutex> lockClass(m_clientThreadSafe);
        if (m_client.empty()) {
            g_dispatcher().addTask(std::bind(&GameMessages::clientCycle, this), "GameMessages::clientCycle");
        }
        m_client.push_back(client);
    }
}

// From dispatcher thread
void GameMessages::serverCycle() {
    std::unique_lock<std::mutex> gate(m_serverThreadSafe, std::defer_lock);
    PacketsBatch batch;
    gate.lock();
    for (auto it = m_server.begin(); it != m_server.end(); it++) {
        auto packet = (*it)->getServerPacket();
        if (!packet || packet->isAborted()) {
            continue;
        }

        if (!packet->writeHeader() || !packet->write()) {
            continue;
        }

        auto& message = batch[packet->getIdentifier()];
        if (!message) {
            message = std::make_unique<GameServerMessage>(packet->getReference());
        }

        if (!message->getServerPacket()) {
            continue;
        }

        message->getServerPacket()->append(packet->getMessage());
    }
    m_server.clear();
    gate.unlock();

    for (auto& [_, message] : batch) {
        auto packet = message->getServerPacket();
        if (!packet) {
            continue;
        }

        packet->send();
    }
}

// From dispatcher thread
void GameMessages::clientCycle() {
    if (g_game().getGameState() == GAME_STATE_SHUTDOWN) {
        return;
    }

    std::unique_lock<std::mutex> gate(m_clientThreadSafe, std::defer_lock);
    PacketsBatches batches;
    gate.lock();
    for (auto it = m_client.begin(); it != m_client.end(); it++) {
        auto packet = (*it)->getClientPacket();
        if (!packet) {
            continue;
        }

        batches[packet->getIdentifier()].push_back(packet);
    }
    m_client.clear();
    gate.unlock();

    // All pool blocks
    for (auto& [_, messages] : batches) {

        // Player block
        for (auto& message : messages) {
            auto packet = message->getClientPacket();
            if (!packet || !packet->canParse()) {
                continue;
            }

            packet->parse();
        }
    }
}

// From protocol thread
bool GameMessages::parsePacketFromInput(NetworkMessage& msg, Player &player, uint8_t header) {
    std::shared_ptr<GameClientMessage> packet;
    switch (header) {
        case GameClientMessage_t::GAMECLIENT_MESSAGE_TYPE_TALK: {
            packet = std::make_shared<GameClientMessageTalk>(player);
            break;
        }
        default: {
            break;
        }
    }

    if (!packet) {
        std::string hexString = fmt::format("0x{:02x}", header); // Fixed the undefined variable 'recvbyte'
        g_logger().debug("[GameMessages::parsePacketFromInput] - Player '{}' sent an unknown packet header: hex[{}], decimal[{}]", player.getName(), asUpperCaseString(hexString), header);
        return false;
    }

    if (!packet->read()) {
        return false;
    }

    g_logger().debug("[GameMessages::parsePacketFromInput] - Player '{}' packet added into the pool: hex[{}], decimal[{}], name[{}]", player.getName(), asUpperCaseString(hexString), header, packet->getName());
    addToPool(packet);
    return true;
}

void GameMessages::abort(GameServerMessage_t type, uint32_t playerId) {
    if (auto it = std::find_if(m_server.begin(), m_server.end(), [type, playerId](const auto packet) {
            return packet->getType() == type && packet->getIdentifier() == playerId;
        }); it != m_server.end() && it->getServerPacket()) {
        it->getServerPacket()->abort();
    }
}