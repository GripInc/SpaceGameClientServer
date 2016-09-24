#ifndef _SHOTS_SETTINGS_H_
#define _SHOTS_SETTINGS_H_

#include "model/deserialized/SectorObjectSettings.h"

class ShotSettings : public SectorObjectSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		SectorObjectSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "lifeTime", mLifeTime);
		XMLHelper::getFloatFrom(_node, "speed", mSpeed);

		return true;
	}

	float mLifeTime = 0.f;
	float mSpeed = 0.f;
};

class ShotsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* shotNode = _node->FirstChildElement("Shot");
		while(shotNode != NULL)
		{
			mShots.push_back(ShotSettings());
			if(mShots.back().deserialize(shotNode))
				shotNode = shotNode->NextSiblingElement("Shot");
			else
				return false;
		}

		return true;
	}

	const ShotSettings* getShotSettings(const std::string& _shotName) const
	{
		for(size_t i = 0; i < mShots.size(); ++i)
			if(mShots[i].mName.compare(_shotName) == 0)
				return &mShots[i];

		return NULL;
	}

	std::vector<ShotSettings> mShots;
};

#endif //_SHOTS_SETTINGS_H_