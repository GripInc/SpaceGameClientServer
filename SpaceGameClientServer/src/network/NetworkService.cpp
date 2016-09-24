#include "network/NetworkService.h"

#include "controller/GameController.h"
#include "model/InputState.h"

#include "utils/StringUtils.h"

#include <iostream>
#include "BitStream.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "NetworkService";
}

const char NetworkService::LEVEL_1_CHANNEL = 1;

NetworkService* NetworkService::mInstance;

NetworkService& NetworkService::getInstance()
{
	if(!mInstance)
		mInstance = new NetworkService();

	return *mInstance;
}

void NetworkService::init(NetworkLayer* _networkLayer, GameController* _gameController)
{
	mNetworkLayer = _networkLayer;
	mGameController = _gameController;
}

//RakNet::Time NetworkService::getClockDifferentialToServer() const
//{
//	return mNetworkLayer->getClockDifferentialToServer();
//}

void NetworkService::processNetworkBuffer()
{
	mNetworkLayer->getNetworkData();
}

void NetworkService::handlePacket(RakNet::Packet* _packet)
{
	RakNet::BitStream stream(_packet->data, _packet->length, false);
	RakNet::MessageID messageType;
	stream.Read((RakNet::MessageID)messageType);

	switch (messageType)
	{
	case ID_GAME_MESSAGE_GET_PLAYER_DATA:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "Received ID_GAME_MESSAGE_GET_PLAYER_DATA", false);

		RakNet::RakString playerData;
		stream.Read(playerData);
		mGameController->startGame(playerData);
		break;
	}
	case ID_GAME_MESSAGE_REQUIRE_LAUNCH:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "Received ID_GAME_MESSAGE_REQUIRE_LAUNCH", false);

		receivedLaunchData(stream);
		break;
	}
	case ID_GAME_MESSAGE_SECTOR_STATE:
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "handlePacket", "Received ID_GAME_MESSAGE_SECTOR_STATE", false);

		mGameController->receivedSectorState(stream);
		break;
	}
	default:
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "handlePacket", "A message with unknown identifier '" + StringUtils::toStr(messageType) + "' has arrived.");
	}
}

/////// GAME SPECIFIC FUNCTIONS ///////
void NetworkService::getPlayerData() const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getPlayerData", "", false);

	mNetworkLayer->send("grip_inc", IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_GET_PLAYER_DATA);
}

void NetworkService::requireLaunchFromStation() const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "requireLaunchFromStation", "", false);

	mNetworkLayer->send("", IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_REQUIRE_LAUNCH);
}

void NetworkService::receivedLaunchData(RakNet::BitStream& _data) const
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

	mGameController->prepareSwitchToInSpaceMode(initialPosition, initialOrientation, sectorName.C_String(), shipUniqueId, mNetworkLayer->getMyGUID(), sectorTick);
}

void NetworkService::sendShipInput(SectorTick _gameTick, const InputState& _inputState) const
{
	RakNet::BitStream stream;
	stream.Write(_gameTick);
	stream.Write(_inputState);
	
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "sendShipInput", "_gameTick is : " + StringUtils::toStr(_gameTick), false);

	mNetworkLayer->send(stream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, LEVEL_1_CHANNEL, ID_GAME_MESSAGE_INPUT_STATE);
}