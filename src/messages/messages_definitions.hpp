/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

// Enums
enum GameServerMessage_t {
	GAMESERVER_MESSAGE_TYPE_UNKNOWN = 0x00,
	GAMESERVER_MESSAGE_TYPE_TALK = 0xAA,
};

enum GameClientMessage_t {
	GAMECLIENT_MESSAGE_TYPE_UNKNOWN = 0x00,
	GAMECLIENT_MESSAGE_TYPE_TALK = 0x96,
};
