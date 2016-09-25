#ifndef _NETWORK_SERVICE_H_
#define _NETWORK_SERVICE_H_

#include "network/NetworkLayer.h"

namespace RakNet
{
	struct Packet;
}

class NetworkService
{
public:
	static const char LEVEL_1_CHANNEL;

	enum GameMessages 
	{ 
		ID_GAME_MESSAGE_GET_PLAYER_DATA = ID_USER_PACKET_ENUM,
		ID_GAME_MESSAGE_REQUIRE_LAUNCH,
		ID_GAME_MESSAGE_INPUT_STATE,
		ID_GAME_MESSAGE_SECTOR_STATE
	};

	///Handle a packet
	virtual void handlePacket(RakNet::Packet* _packet) = 0;

	///Get network messages, process it and release it
	void processNetworkBuffer();

protected:
	///The network layer
	NetworkLayer* mNetworkLayer = nullptr;
};

#endif //_NETWORK_SERVICE_H_