#ifndef _DIRECTIONALS_SETTINGS_H_
#define _DIRECTIONALS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class DirectionalSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "turnRateMultiplier", mTurnRateMultiplier);
		XMLHelper::getFloatFrom(_node, "inertiaMultiplier", mInertiaMultiplier);

		return success;
	}

	float mTurnRateMultiplier;
	float mInertiaMultiplier;
};

class DirectionalsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* directionalNode = _node->FirstChildElement("Directional");
		while(directionalNode != NULL)
		{
			mDirectionals.push_back(DirectionalSettings());
			if(mDirectionals[mDirectionals.size() - 1].deserialize(directionalNode))
				directionalNode = directionalNode->NextSiblingElement("Directional");
			else
				return false;
		}

		return true;
	}

	const DirectionalSettings* getDirectionalSettings(const std::string& _directional) const
	{
		for(int i = 0; i < mDirectionals.size(); ++i)
			if(mDirectionals[i].mName.compare(_directional) == 0)
				return &mDirectionals[i];

		return NULL;
	}

	btAlignedObjectArray<DirectionalSettings> mDirectionals;
};

#endif //_SHIELDS_SETTINGS_H_