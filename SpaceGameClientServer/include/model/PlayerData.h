#ifndef _PLAYER_DATA_H_
#define _PLAYER_DATA_H_

#include "model/Ship.h"
#include "model/GameSettings.h"

#include "utils/json/JsonHelpers.h"

class Ship;

class PlayerData : public IJsonNodeSerializable
{
public:
	/**
	* Export to node.
	* @param _node node to edit.
	*/
	virtual void serializeToJsonNode(Json::Value& _node) const
	{
		//TODO
		//If we replace all data by lot of redis key, we can save separate data easier?
		//Or maybe separate playersData areas?
	}

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	virtual bool deserializeFromJsonNode(const Json::Value& _node)
	{
		bool result = true;

		mOriginalJsonNode = _node;
		result = result && JsonHelpers::readString(_node, "playerId", mPlayerId, "");
		result = result && JsonHelpers::readInt64(_node, "money", mMoney, 0);
		result = result && JsonHelpers::readString(_node, "ship", mShipId, "");
		result = result && JsonHelpers::readString(_node, "station", mLastStation, "");
		result = result && JsonHelpers::readString(_node, "sector", mLastSector, "");

		mPlayerShip.initShip(GameSettings::getInstance().getShip(mShipId));

		//DEBUG
		mPlayerShip.addEngine(*GameSettings::getInstance().getEngine("FirstEngine"));
		mPlayerShip.addDirectional(*GameSettings::getInstance().getDirectional("SecondDirectional"));
		mPlayerShip.addWeapon(*GameSettings::getInstance().getWeapon("Gun1"), 0);
		mPlayerShip.addWeapon(*GameSettings::getInstance().getWeapon("Gun1"), 1);
		mPlayerShip.addWeapon(*GameSettings::getInstance().getWeapon("Gun1"), 2);
		mPlayerShip.addWeapon(*GameSettings::getInstance().getWeapon("Gun1"), 3);

		return result;
	}

	Json::Value mOriginalJsonNode;

	std::string mPlayerId;
	std::string mShipId;
	long long int mMoney = 0;
	std::string mLastStation;
	std::string mLastSector;

	Ship mPlayerShip;
};

#endif //_PLAYER_DATA_H_