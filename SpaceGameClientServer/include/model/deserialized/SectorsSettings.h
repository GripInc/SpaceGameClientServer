#ifndef _SECTORS_SETTINGS_H_
#define _SECTORS_SETTINGS_H_

#include "model/deserialized/NamedSettings.h"
#include "model/deserialized/StaticObjectSettings.h"
#include "model/deserialized/StationObjectSettings.h"

class SectorSettings : public NamedSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= NamedSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "lightVectorX", mLightVectorX);
		XMLHelper::getFloatFrom(_node, "lightVectorY", mLightVectorY, -1.f);
		XMLHelper::getFloatFrom(_node, "lightVectorZ", mLightVectorZ);

		tinyxml2::XMLElement* staticObjectsNode = _node->FirstChildElement("StaticObjects");
		if(staticObjectsNode)
		{
			tinyxml2::XMLElement* staticObjectNode = staticObjectsNode->FirstChildElement("StaticObject");
			while(staticObjectNode != NULL)
			{
				mStaticObjects.push_back(StaticObjectSettings());
				if(mStaticObjects[mStaticObjects.size() - 1].deserialize(staticObjectNode))
					staticObjectNode = staticObjectNode->NextSiblingElement("StaticObject");
				else
					return false;
			}
		}

		tinyxml2::XMLElement* planetObjectsNode = _node->FirstChildElement("PlanetObjects");
		if(planetObjectsNode)
		{
			tinyxml2::XMLElement* planetObjectNode = planetObjectsNode->FirstChildElement("PlanetObject");
			while(planetObjectNode != NULL)
			{
				mPlanetObjects.push_back(SectorObjectSettings());
				if(mPlanetObjects[mPlanetObjects.size() - 1].deserialize(planetObjectNode))
					planetObjectNode = planetObjectNode->NextSiblingElement("PlanetObject");
				else
					return false;
			}
		}

		tinyxml2::XMLElement* gateObjectsNode = _node->FirstChildElement("GateObjects");
		if(gateObjectsNode)
		{
			tinyxml2::XMLElement* gateObjectNode = gateObjectsNode->FirstChildElement("GateObject");
			while(gateObjectNode != NULL)
			{
				mGateObjects.push_back(SectorObjectSettings());
				if(mGateObjects[mGateObjects.size() - 1].deserialize(gateObjectNode))
					gateObjectNode = gateObjectNode->NextSiblingElement("GateObject");
				else
					return false;
			}
		}

		tinyxml2::XMLElement* stationObjectsNode = _node->FirstChildElement("StationObjects");
		if(stationObjectsNode)
		{
			tinyxml2::XMLElement* stationObjectNode = stationObjectsNode->FirstChildElement("StationObject");
			while(stationObjectNode != NULL)
			{
				mStationObjects.push_back(StationObjectSettings());
				if(mStationObjects[mStationObjects.size() - 1].deserialize(stationObjectNode))
					stationObjectNode = stationObjectNode->NextSiblingElement("StationObject");
				else
					return false;
			}
		}

		return success;
	}

	float mLightVectorX = 0.f, mLightVectorY = 0.f, mLightVectorZ = 0.f;
	btAlignedObjectArray<StaticObjectSettings> mStaticObjects;
	btAlignedObjectArray<SectorObjectSettings> mPlanetObjects;
	btAlignedObjectArray<SectorObjectSettings> mGateObjects;
	btAlignedObjectArray<StationObjectSettings> mStationObjects;
};

class SectorsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* sectorNode = _node->FirstChildElement("Sector");
		while(sectorNode != NULL)
		{
			mSectors.push_back(SectorSettings());
			if(mSectors[mSectors.size() - 1].deserialize(sectorNode))
				sectorNode = sectorNode->NextSiblingElement("Sector");
			else
				return false;
		}

		return true;
	}

	const SectorSettings* getSectorSettings(const std::string& _sectorName) const
	{
		for(int i = 0; i < mSectors.size(); ++i)
			if(mSectors[i].mName.compare(_sectorName) == 0)
				return &mSectors[i];

		return NULL;
	}

	btAlignedObjectArray<SectorSettings> mSectors;
};

#endif //_SECTORS_SETTINGS_H_