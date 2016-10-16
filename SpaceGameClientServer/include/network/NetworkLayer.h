#ifndef _NETWORK_LAYER_H_
#define _NETWORK_LAYER_H_

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"

#include <list>

//TODO close connection, reinit connection etc

class NetworkService;
class IConnectionReadyListener;

namespace RakNet
{
	class RakString;
}

class NetworkLayer
{
public:
	typedef char NetworkChannel;

	NetworkLayer();

	~NetworkLayer()
	{
		RakNet::RakPeerInterface::DestroyInstance(mPeer);
	}

	void init(unsigned short _serverPort, unsigned int _maxConnections);

	//Connect the client
	void connect(const char* _serverAddress, unsigned short _serverPort);

	//Link up a network service instance to this layer
	void registerNetworkService(NetworkService* _networkService) { mNetworkService = _networkService; }

	//Link up a listener that will be warned that the connection is ready
	void registerConnectionReadyListener(IConnectionReadyListener* _connectionReadyListener) { mConnectionReadyListener = _connectionReadyListener; }

	//Get and handle network data
	void getNetworkData();

	//Client specific method to send data to server
	void clientSend(const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType);
	//Client specific method to send data to server
	void clientSend(RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const;
	//Server specific method to send data to server
	void serverSend(const RakNet::AddressOrGUID& _clientAdress, const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const;
	//Server specific method to send data to server
	void serverSend(const RakNet::AddressOrGUID& _clientAdress, RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const;

	//Is client connect
	bool isConnected() const { return mIsConnected; }

	//Get client GUID
	RakNet::RakNetGUID getMyGUID() const { return mPeer->GetMyGUID(); }

protected:

	//The RakPeer interface
	RakNet::RakPeerInterface* mPeer = nullptr;

	//The network service
	NetworkService* mNetworkService = nullptr;

	//A connection ready listener
	IConnectionReadyListener* mConnectionReadyListener = nullptr;

	//Handle packets
	void handlePacket(RakNet::Packet* _packet);

	//Server address. Filled in atclient connection from server data.
	RakNet::AddressOrGUID mServerAddress;

	//Flag for client is connected
	bool mIsConnected = false;
};

#endif //_NETWORK_LAYER_H_