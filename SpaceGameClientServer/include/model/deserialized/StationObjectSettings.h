#ifndef _STATION_OBJECT_SETTINGS_H_
#define _STATION_OBJECT_SETTINGS_H_

#include "model/deserialized/SectorObjectSettings.h"

class StationObjectSettings : public SectorObjectSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SectorObjectSettings::deserialize(_node);

		XMLHelper::getStringFrom(_node, "stationModel", mStationModel);

		return success;
	}

	std::string mStationModel;
};

#endif //_STATION_OBJECT_SETTINGS_H_