#include "network/ClientNetworkService.h"

#include "controller/ClientGameController.h"
//#include "model/InputState.h"

#include "utils/StringUtils.h"

//#include <iostream>
#include "BitStream.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ClientNetworkService";
}

ClientNetworkService* ClientNetworkService::mInstance;

ClientNetworkService& ClientNetworkService::getInstance()
{
	if (!mInstance)
		mInstance = new ClientNetworkService();

	return *mInstance;
}

void ClientNetworkService::init(NetworkLayer* _networkLayer, ClientGameController* _clientGameController)
{
	mNetworkLayer = _networkLayer;
	mClientGameController = _clientGameController;
}

void ClientNetworkService::handlePacket(RakNet::Packet* _packet)
{
	static const std::string LOG_METHOD_TAG = "handlePacket";

	RakNet::BitStream stream(_packet->data, _packet->length, false);
	RakNet::MessageID messageType;
	stream.Read((RakNet::MessageID)messageType);

	//Client way of messages handling
	switch (messageType)
	{
	case ID_GAME_MESSAGE_GET_PLAYER_DATA:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "Received ID_GAME_MESSAGE_GET_PLAYER_DATA", false);

		RakNet::RakString playerData;
		stream.Read(playerData);
		mClientGameController->startGame(playerData);
		break;
	}
	case ID_GAME_MESSAGE_REQUIRE_LAUNCH:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "Received ID_GAME_MESSAGE_REQUIRE_LAUNCH", false);

		handleLaunchData(stream);
		break;
	}
	case ID_GAME_MESSAGE_SECTOR_STATE:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, LOG_METHOD_TAG, "Received ID_GAME_MESSAGE_SECTOR_STATE", false);

		mClientGameController->receivedSectorState(stream);
		break;
	}
	default:
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, LOG_METHOD_TAG, "A message with unknown identifier '" + StringUtils::toStr(messageType) + "' has arrived.");
	}
}

void ClientNetworkService::requestPlayerData() const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "requestPlayerData", "", false);

	mNetworkLayer->clientSend("grip_inc", IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_GET_PLAYER_DATA);
}

void ClientNetworkService::requireLaunchFromStation() const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "requireLaunchFromStation", "", false);

	mNetworkLayer->clientSend("", IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_REQUIRE_LAUNCH);
}

void ClientNetworkService::handleLaunchData(RakNet::BitStream& _data) const
{
	Ogre::Vector3 initialPosition;
	Ogre::Quaternion initialOrientation;
	RakNet::RakString sectorName;
	UniqueId shipUniqueId;
	SectorTick sectorTick;

	_data.Read(sectorName);
	_data.Read(initialPosition);
	_data.Read(initialOrientation);
	_data.Read(shipUniqueId);
	_data.Read(sectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedLaunchData", "sectorName is : " + std::string(sectorName.C_String()) + "; shipUniqueId is : " + StringUtils::toStr(shipUniqueId) + "; sectorTick is : " + StringUtils::toStr(sectorTick), false);

	mClientGameController->prepareSwitchToInSpaceMode(initialPosition, initialOrientation, sectorName.C_String(), shipUniqueId, mNetworkLayer->getMyGUID(), sectorTick);
}

void ClientNetworkService::sendShipInput(SectorTick _gameTick, const InputState& _inputState) const
{
	RakNet::BitStream stream;
	stream.Write(_gameTick);
	stream.Write(_inputState);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "sendShipInput", "_gameTick is : " + StringUtils::toStr(_gameTick), false);

	mNetworkLayer->clientSend(stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_INPUT_STATE);
}
