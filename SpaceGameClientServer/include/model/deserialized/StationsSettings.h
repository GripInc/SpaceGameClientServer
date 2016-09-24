#ifndef _STATIONS_SETTINGS_H_
#define _STATIONS_SETTINGS_H_

#include "model/deserialized/NamedSettings.h"
#include "model/deserialized/SectorObjectSettings.h"

class StationSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= XMLHelper::getStringFrom(_node, "stationModel", mStationModel);

		XMLHelper::getStringFrom(_node, "screenModel", mScreenModel);
		XMLHelper::getBoolFrom(_node, "hasEquipementDealer", mHasEquipementDealer);
		XMLHelper::getBoolFrom(_node, "hasCommodityDealer", mHasCommodityDealer);
		XMLHelper::getBoolFrom(_node, "hasBar", mHasBar);
		XMLHelper::getBoolFrom(_node, "hasGuildsOffice", mHasGuildsOffice);
		XMLHelper::getBoolFrom(_node, "hasMissionsComputer", mHasMissionsComputer);
		XMLHelper::getBoolFrom(_node, "hasTrainer", mHasTrainer);

		tinyxml2::XMLElement* launchNode = _node->FirstChildElement("LaunchPoint");
		while(launchNode != NULL)
		{
			mLaunchPoints.push_back(SectorObjectSettings());
			if(mLaunchPoints[mLaunchPoints.size() - 1].deserialize(launchNode))
				launchNode = launchNode->NextSiblingElement("LaunchPoint");
			else
				return false;
		}

		return success;
	}

	std::string mStationModel;
	std::string mScreenModel;
	bool mHasEquipementDealer = false;
	bool mHasCommodityDealer = false;
	bool mHasBar = false;
	bool mHasGuildsOffice = false;
	bool mHasMissionsComputer = false;
	bool mHasTrainer = false;
	btAlignedObjectArray<SectorObjectSettings> mLaunchPoints;
};

class StationsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* stationNode = _node->FirstChildElement("Station");
		while(stationNode != NULL)
		{
			mStations.push_back(StationSettings());
			if(mStations[mStations.size() - 1].deserialize(stationNode))
				stationNode = stationNode->NextSiblingElement("Station");
			else
				return false;
		}

		return true;
	}

	const StationSettings* getStationSettings(const std::string& _station) const
	{
		for(int i = 0; i < mStations.size(); ++i)
			if(mStations[i].mStationModel.compare(_station) == 0)
				return &mStations[i];

		return NULL;
	}

	btAlignedObjectArray<StationSettings> mStations;
};

#endif //_STATIONS_SETTINGS_H_