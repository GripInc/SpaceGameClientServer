#include "network/NetworkLayer.h"

#include "network/NetworkService.h"
#include "network/IConnectionReadyListener.h"

#include "BitStream.h"
#include <iostream>

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "NetworkLayer";
}

NetworkLayer::NetworkLayer()
{
	mPeer = RakNet::RakPeerInterface::GetInstance();
	mPeer->SetOccasionalPing(true);

	//DEBUG
	//mPeer->ApplyNetworkSimulator(0.1, 10, 200);
}

void NetworkLayer::init()
{
#	ifdef _GAME_CLIENT
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "init", "Starting the client.", true);
	RakNet::SocketDescriptor socketDescriptor;
	mPeer->Startup(1, &socketDescriptor, 1); //TODO use choosed options
#	else
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "init", "Starting the server.", true);

	RakNet::SocketDescriptor socketDescriptor(SERVER_PORT, 0); //TODO set real values
	mPeer->Startup(MAX_CLIENTS, &socketDescriptor, 1); //TODO set real values
	// Make server accept incoming connections from the clients
	mPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
#	endif
}

void NetworkLayer::connect(const char* _serverAddress)
{
	RakNet::ConnectionAttemptResult connectionResult = mPeer->Connect(_serverAddress, SERVER_PORT, 0, 0); //TODO use choosed options

	//TODO handle result
	switch (connectionResult)
	{
	case RakNet::CONNECTION_ATTEMPT_STARTED:
		break;
	case RakNet::INVALID_PARAMETER:
	case RakNet::CANNOT_RESOLVE_DOMAIN_NAME:
	case RakNet::ALREADY_CONNECTED_TO_ENDPOINT:
	case RakNet::CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
	case RakNet::SECURITY_INITIALIZATION_FAILED:
	default:
		//TODO Handle network error
		break;
	}
}

void NetworkLayer::getNetworkData()
{
	RakNet::Packet* packet = nullptr;

	for (packet = mPeer->Receive(); packet != nullptr; mPeer->DeallocatePacket(packet), packet = mPeer->Receive())
		handlePacket(packet);
}

void NetworkLayer::handlePacket(RakNet::Packet* _packet)
{
	static const std::string LOG_METHOD_TAG = "handlePacket";

	switch (_packet->data[0])
	{
	//////////////////////////////////////////
	//////////Client side messages////////////
	//////////////////////////////////////////
	case ID_CONNECTION_REQUEST_ACCEPTED:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "Our connection request has been accepted.", false);
		mServerAddress = _packet->systemAddress;
		mIsConnected = true;
		mConnectionReadyListener->notifyIsConnected(mIsConnected);
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "The server is full.", false);
		mIsConnected = false;
		mConnectionReadyListener->notifyIsConnected(mIsConnected);
		break;
	//////////////////////////////////////////
	////////Client/Server side messages///////
	//////////////////////////////////////////
	case ID_DISCONNECTION_NOTIFICATION:
#		ifdef _GAME_CLIENT
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "We have been disconnected.", false);
		mIsConnected = false;
		mConnectionReadyListener->notifyIsConnected(mIsConnected);
#		else
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A client has disconnected.", true);
#		endif
		break;
	case ID_CONNECTION_LOST:
#		ifdef _GAME_CLIENT
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "Connection lost.", false);
		mIsConnected = false;
		mConnectionReadyListener->notifyIsConnected(mIsConnected);
#		else
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A client lost the connection.", true);
#		endif
		break;
	//////////////////////////////////////////
	//////////Server side messages////////////
	//////////////////////////////////////////
	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A client has disconnected.", true);
		break;
	case ID_REMOTE_CONNECTION_LOST:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A client has lost the connection.", true);
		break;
	case ID_REMOTE_NEW_INCOMING_CONNECTION:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A client has connected.", true);
		break;
	case ID_NEW_INCOMING_CONNECTION:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "A connection is incoming.", true);
		break;
	default:
		//Packets destinated to be treated by network service
		mNetworkService->handlePacket(_packet);
		break;
	}
}

//Send functions
void NetworkLayer::clientSend(const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType)
{
	//TODO use compressed

	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_message);
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, mServerAddress, false);
}

void NetworkLayer::clientSend(RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const
{
	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_bitStream, _bitStream.GetNumberOfBitsUsed() - _bitStream.GetReadOffset());
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, mServerAddress, false);
}

void NetworkLayer::serverSend(const RakNet::AddressOrGUID& _client, const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const
{
	//TODO use compressed? -> seems compressed for understandable words only

	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_message);
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, _client, false);
}

void NetworkLayer::serverSend(const RakNet::AddressOrGUID& _client, RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const
{
	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_bitStream, _bitStream.GetNumberOfBitsUsed() - _bitStream.GetReadOffset());
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, _client, false);
}
//End send functions
