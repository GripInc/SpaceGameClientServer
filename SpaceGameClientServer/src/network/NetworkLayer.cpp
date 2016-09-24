#include "network/NetworkLayer.h"

#include "network/INetworkService.h"
#include "network/IConnectionReadyListener.h"

#include "BitStream.h"
#include <iostream>

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "NetworkLayer";
}

NetworkLayer::NetworkLayer() : mIsConnected(false)
{
	mPeer = RakNet::RakPeerInterface::GetInstance();
	mPeer->SetOccasionalPing(true);

	//DEBUG
	//mPeer->ApplyNetworkSimulator(0.1, 10, 200);
}

void NetworkLayer::init()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "init", "Starting the client.", true);

	mPeer->Startup(1, &mSocketDescriptor, 1); //TODO use choosed options
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
	RakNet::Packet *packet;

	for (packet = mPeer->Receive(); packet != NULL; mPeer->DeallocatePacket(packet), packet = mPeer->Receive())
	{
		handlePacket(packet);
	}
}

void NetworkLayer::setIsConnected(bool _value)
{
	mConnectionReadyListener->notifyIsConnected(_value);

	mIsConnected = _value;
}

//RakNet::Time NetworkLayer::getClockDifferentialToServer() const
//{
	//return mPeer->getClockDifferential(mServerAddress);
//}

void NetworkLayer::handlePacket(RakNet::Packet* _packet)
{
	switch (_packet->data[0])
	{
	case ID_CONNECTION_REQUEST_ACCEPTED:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "Our connection request has been accepted.", false);
		mServerAddress = _packet->systemAddress;
		setIsConnected(true);
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "The server is full.", false);
		setIsConnected(false);
		break;
	case ID_DISCONNECTION_NOTIFICATION:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "We have been disconnected.", false);
		setIsConnected(false);
		break;
	case ID_CONNECTION_LOST:
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "Connection lost.", false);
		setIsConnected(false);
		break;
	default:
		//Packets destinated to be treated by network service
		mNetworkService->handlePacket(_packet);
		break;
	}
}

//Send functions
void NetworkLayer::send(const RakNet::RakString& _message, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType)
{
	//TODO use compressed

	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_message);
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, mServerAddress, false);
}

void NetworkLayer::send(RakNet::BitStream& _bitStream, PacketPriority _priority, PacketReliability _reliability, NetworkChannel _orderingChannel, RakNet::MessageID _messageType) const
{
	RakNet::BitStream bitStreamOut;
	bitStreamOut.Write((RakNet::MessageID)_messageType);
	bitStreamOut.Write(_bitStream, _bitStream.GetNumberOfBitsUsed() - _bitStream.GetReadOffset());
	mPeer->Send(&bitStreamOut, _priority, _reliability, _orderingChannel, mServerAddress, false);
}
//End send functions
