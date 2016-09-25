#ifndef _SERVER_NETWORK_SERVICE_H_
#define _SERVER_NETWORK_SERVICE_H_

#include "network/NetworkService.h"

#include <set>

#include "SpaceGameTypes.h"

namespace RakNet
{
	struct SystemAddress;
	struct Packet;
}

namespace Ogre
{
	class Vector3;
	class Quaternion;
}

class ServerGameController;
class PlayerData;

class ServerNetworkService : public NetworkService
{
public:
	///Singleton
	static ServerNetworkService& getInstance();
	void init(NetworkLayer* _networkLayer, ServerGameController* _serverGameController);

	///Handle a game specific packet
	virtual void handlePacket(RakNet::Packet* _packet) override;

	///Server specific. Broadcast a sector state to clients in the list.
	void broadcastSector(const std::set<RakNet::RakNetGUID>& _clientIds, RakNet::BitStream& _serializedSector);

protected:
	///Singleton
	static ServerNetworkService* mInstance;
	ServerNetworkService() {}

	///The game controller
	ServerGameController* mServerGameController;

	///Server specific. Send player data to a client.
	void sendPlayerData(const PlayerData* _playerData, const RakNet::SystemAddress& _clientAdress) const;
	///Server specific. Send player launch point to a client.
	void sendPlayerLaunchPoint(const RakNet::RakNetGUID& _clientId, const RakNet::SystemAddress& _clientAdress, const std::string& _sector, Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, SectorTick _sectorTick) const;
	///Add a client input in the list of inputs to be taken care of.
	void addClientInput(const RakNet::RakNetGUID& _clientId, RakNet::BitStream& _data);
};

#endif //_SERVER_NETWORK_SERVICE_H_