#ifndef _ENGINES_SETTINGS_H_
#define _ENGINES_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class EngineSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		success &= XMLHelper::getFloatFrom(_node, "reactivity", mReactivity);
		success &= XMLHelper::getFloatFrom(_node, "power", mPower);
		success &= XMLHelper::getFloatFrom(_node, "maxSpeed", mMaxSpeed);
		success &= XMLHelper::getFloatFrom(_node, "thrustSensitivity", mThrustSensitivity);
		success &= XMLHelper::getFloatFrom(_node, "thrustMaxValue", mThrustMaxValue);

		XMLHelper::getFloatFrom(_node, "consumption", mConsumption);

		std::string tempType;
		XMLHelper::getStringFrom(_node, "type", tempType);
		mType = Types::getEngineType(tempType);

		return success;
	}

	float mReactivity = 0.f;
	float mPower = 0.f;
	float mMaxSpeed = 0.f;
	float mThrustSensitivity = 0.f;
	float mThrustMaxValue = 0.f;
	unsigned int mSpace = 0U;
	unsigned int mHitPoints = 0U;
	float mConsumption = 0.f;
	Types::EngineType mType;
};

class EnginesSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* engineNode = _node->FirstChildElement("Engine");
		while(engineNode != NULL)
		{
			mEngines.push_back(EngineSettings());
			if(mEngines[mEngines.size() - 1].deserialize(engineNode))
				engineNode = engineNode->NextSiblingElement("Engine");
			else
				return false;
		}

		return true;
	}

	const EngineSettings* getEngineSettings(const std::string& _engine) const
	{
		for(int i = 0; i < mEngines.size(); ++i)
			if(mEngines[i].mName.compare(_engine) == 0)
				return &mEngines[i];

		return NULL;
	}

	btAlignedObjectArray<EngineSettings> mEngines;
};

#endif //_ENGINES_SETTINGS_H_