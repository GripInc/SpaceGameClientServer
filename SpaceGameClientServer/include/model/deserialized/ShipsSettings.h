#ifndef _SHIPS_SETTINGS_H_
#define _SHIPS_SETTINGS_H_

#include "model/deserialized/DynamicObjectSettings.h"
#include "model/deserialized/HardPointSettings.h"

class ShipSettings : public DynamicObjectSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= DynamicObjectSettings::deserialize(_node);

		success &= XMLHelper::getFloatFrom(_node, "maxYawRate", mMaxYawRate);
		success &= XMLHelper::getFloatFrom(_node, "maxPitchRate", mMaxPitchRate);
		success &= XMLHelper::getFloatFrom(_node, "maxRollRate", mMaxRollRate);
		
		XMLHelper::getFloatFrom(_node, "headPosX", mHeadPosX);
		XMLHelper::getFloatFrom(_node, "headPosY", mHeadPosY);
		XMLHelper::getFloatFrom(_node, "headPosZ", mHeadPosZ);
		
		//HardPoints
		tinyxml2::XMLElement* hardPointNode = _node->FirstChildElement("HardPoint");
		while(hardPointNode != NULL)
		{
			mHardPoints.push_back(HardPointSettings());
			if(mHardPoints[mHardPoints.size() - 1].deserialize(hardPointNode))
				hardPointNode = hardPointNode->NextSiblingElement("HardPoint");
			else
				success &= false;
		}

		XMLHelper::getUnsignedIntFrom(_node, "cargoSpace", mCargoSpace);

		return success;
	}

	float mMaxYawRate = 0.f;
	float mMaxPitchRate = 0.f;
	float mMaxRollRate = 0.f;
	float mHeadPosX = 0.f;
	float mHeadPosY = 0.f;
	float mHeadPosZ = 0.f;
	unsigned int mCargoSpace = 0U;

	btAlignedObjectArray<HardPointSettings> mHardPoints;
};

class ShipsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* shipNode = _node->FirstChildElement("Ship");
		while(shipNode != NULL)
		{
			mShips.push_back(ShipSettings());
			if(mShips[mShips.size() - 1].deserialize(shipNode))
				shipNode = shipNode->NextSiblingElement("Ship");
			else
				return false;
		}

		return true;
	}

	const ShipSettings* getShipSettings(const std::string& _shipName) const
	{
		for(int i = 0; i < mShips.size(); ++i)
			if(mShips[i].mName.compare(_shipName) == 0)
				return &mShips[i];

		return NULL;
	}

	btAlignedObjectArray<ShipSettings> mShips;
};

#endif //_SHIPS_SETTINGS_H_