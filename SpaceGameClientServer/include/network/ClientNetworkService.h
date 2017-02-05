#ifndef _CLIENT_NETWORK_SERVICE_H_
#define _CLIENT_NETWORK_SERVICE_H_

#include "network/NetworkService.h"

#include <set>

#include "SpaceGameTypes.h"

namespace RakNet
{
	struct Packet;
}

class ClientGameController;
class InputState;
class PlayerData;

class ClientNetworkService : public NetworkService
{
public:
	///Singleton
	static ClientNetworkService& getInstance();
	void init(NetworkLayer* _networkLayer, ClientGameController* _clientGameController);

	///Handle a game specific packet
	virtual void handlePacket(RakNet::Packet* _packet) override;

	///Client specific method. Ask the server the player data.
	void requestPlayerData() const;
	///Client specific method. Requires to the server the launch from a station.
	void requireLaunchFromStation() const;
	///Client specific method. Send player input when in space.
	void sendInput(const std::list<InputState>& _inputs, SectorTick _lastAcknowledgedInput) const;

protected:
	///Singleton
	static ClientNetworkService* mInstance;
	ClientNetworkService() {}

	///The game controller
	ClientGameController* mClientGameController;

	///Client specific. Handle launch data received from server
	void handleLaunchData(RakNet::BitStream& _data) const;
};

#endif //_CLIENT_NETWORK_SERVICE_H_