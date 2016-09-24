#ifndef _STATION_SCREENS_SETTINGS_H_
#define _STATION_SCREENS_SETTINGS_H_

#include "utils/XMLHelper.h"
#include <tinyxml2.h>
#include "LinearMath/btAlignedObjectArray.h"

#include "model/deserialized/NamedSettings.h"

class ClickZoneSettings : public NamedSettings
{
	public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= NamedSettings::deserialize(_node);

		success &= XMLHelper::getIntFrom(_node, "x", mX);
		success &= XMLHelper::getIntFrom(_node, "y", mY);
		success &= XMLHelper::getIntFrom(_node, "width", mWidth);
		success &= XMLHelper::getIntFrom(_node, "height", mHeight);
		success &= XMLHelper::getStringFrom(_node, "target", mTarget);

		return success;
	}

	int mX = 0;
	int mY = 0;
	int mWidth = 0;
	int mHeight = 0;
	std::string mTarget;
};

class ScreenSettings : public NamedSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= NamedSettings::deserialize(_node);

		success &= XMLHelper::getStringFrom(_node, "background", mBackground);

		tinyxml2::XMLElement* clickZoneNode = _node->FirstChildElement("ClickZone");
		while(clickZoneNode != NULL)
		{
			mClickZones.push_back(ClickZoneSettings());
			if(mClickZones[mClickZones.size() - 1].deserialize(clickZoneNode))
				clickZoneNode = clickZoneNode->NextSiblingElement("ClickZone");
			else
				return false;
		}

		return success;
	}

	std::string mBackground;
	btAlignedObjectArray<ClickZoneSettings> mClickZones;
};

class StationScreenSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= XMLHelper::getStringFrom(_node, "screenModel", mScreenModel);
		success &= XMLHelper::getStringFrom(_node, "firstScreen", mFirstScreen);

		tinyxml2::XMLElement* screenNode = _node->FirstChildElement("Screen");
		while(screenNode != NULL)
		{
			mScreens.push_back(ScreenSettings());
			if(mScreens[mScreens.size() - 1].deserialize(screenNode))
				screenNode = screenNode->NextSiblingElement("Screen");
			else
				return false;
		}

		return success;
	}

	const ScreenSettings* getScreen(const std::string& _screen) const
	{
		for(int i = 0; i < mScreens.size(); ++i)
			if(mScreens[i].mName.compare(_screen) == 0)
				return &mScreens[i];

		return NULL;
	}

	std::string mScreenModel;
	std::string mFirstScreen;
	btAlignedObjectArray<ScreenSettings> mScreens;
};

class StationScreensSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* stationScreenNode = _node->FirstChildElement("StationScreen");
		while(stationScreenNode != NULL)
		{
			mStationScreens.push_back(StationScreenSettings());
			if(mStationScreens[mStationScreens.size() - 1].deserialize(stationScreenNode))
				stationScreenNode = stationScreenNode->NextSiblingElement("StationScreen");
			else
				return false;
		}

		return true;
	}

	const StationScreenSettings* getStationScreenSettings(const std::string& _stationScreen) const
	{
		for(int i = 0; i < mStationScreens.size(); ++i)
			if(mStationScreens[i].mScreenModel.compare(_stationScreen) == 0)
				return &mStationScreens[i];

		return NULL;
	}

	btAlignedObjectArray<StationScreenSettings> mStationScreens;
};

#endif //_STATION_SCREENS_SETTINGS_H_