#ifndef _NETWORK_LAYER_H_
#define _NETWORK_LAYER_H_

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakString.h"

#include <list>

#define SERVER_PORT 60000 //TODO useconfig file or prompt

//TODO close connection, reinit connection etc

class INetworkService;
class IConnectionReadyListener;

class NetworkLayer
{
public:
	typedef char NetworkChannel;

	NetworkLayer();

	~NetworkLayer()
	{
		RakNet::RakPeerInterface::DestroyInstance(mPeer);
	}

	void init();
	void connect(const char* _serverAddress);
	//void start();
	void registerNetworkService(INetworkService* _networkService) { mNetworkService = _networkService; }
	void registerConnectionReadyListener(IConnectionReadyListener* _connectionReadyListener) { mConnectionReadyListener = _connectionReadyListener; }

	void getNetworkData();

	void send(const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType);
	void send(RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const;

	bool isConnected() const { return mIsConnected; }

	RakNet::RakNetGUID getMyGUID() const { return mPeer->GetMyGUID(); }

	//RakNet::Time getClockDifferentialToServer() const;

protected:
	RakNet::RakPeerInterface* mPeer = nullptr;

	INetworkService* mNetworkService = nullptr;
	IConnectionReadyListener* mConnectionReadyListener = nullptr;

	RakNet::SocketDescriptor mSocketDescriptor;

	void handlePacket(RakNet::Packet* _packet);

	RakNet::AddressOrGUID mServerAddress;

	void setIsConnected(bool _value);

private:
	bool mIsConnected = false; //Use setter only to change value
};

#endif //_NETWORK_LAYER_H_