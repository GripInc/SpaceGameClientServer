#ifndef _NETWORK_SERVICE_H_
#define _NETWORK_SERVICE_H_

#include "network/INetworkService.h"
#include "network/NetworkLayer.h"

#include "SpaceGameTypes.h"

class GameController;
class InputState;

class NetworkService : public INetworkService
{
public:
	static const char LEVEL_1_CHANNEL;

	///Singleton
	static NetworkService& getInstance();
	void init(NetworkLayer* _networkLayer, GameController* _gameController);

	enum GameMessages 
	{ 
		ID_GAME_MESSAGE_GET_PLAYER_DATA = ID_USER_PACKET_ENUM,
		ID_GAME_MESSAGE_REQUIRE_LAUNCH,
		ID_GAME_MESSAGE_INPUT_STATE,
		ID_GAME_MESSAGE_SECTOR_STATE
	};

	void processNetworkBuffer();
	virtual void handlePacket(RakNet::Packet* _packet);

	void getPlayerData() const;
	void requireLaunchFromStation() const;
	void sendShipInput(SectorTick _gameTick, const InputState& _inputState) const;

	//RakNet::Time getClockDifferentialToServer() const;

protected:
	///Singleton
	static NetworkService* mInstance;
	NetworkService() {}

	NetworkLayer* mNetworkLayer = nullptr;
	GameController* mGameController = nullptr;

	void receivedLaunchData(RakNet::BitStream& _data) const;
};

#endif //_NETWORK_SERVICE_H_