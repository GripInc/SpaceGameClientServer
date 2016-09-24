#ifndef _SHIELDS_SETTINGS_H_
#define _SHIELDS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class ShieldSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		XMLHelper::getUnsignedIntFrom(_node, "regenerationRate", mRegeneration);
		XMLHelper::getUnsignedIntFrom(_node, "capacity", mCapacity);

		return success;
	}

	unsigned int mRegeneration = 0U;
	unsigned int mCapacity = 0U;
};

class ShieldsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* shieldNode = _node->FirstChildElement("Shield");
		while(shieldNode != NULL)
		{
			mShields.push_back(ShieldSettings());
			if(mShields[mShields.size() - 1].deserialize(shieldNode))
				shieldNode = shieldNode->NextSiblingElement("Shield");
			else
				return false;
		}

		return true;
	}

	const ShieldSettings* getShieldSettings(const std::string& _shield) const
	{
		for(int i = 0; i < mShields.size(); ++i)
			if(mShields[i].mName.compare(_shield) == 0)
				return &mShields[i];

		return NULL;
	}

	btAlignedObjectArray<ShieldSettings> mShields;
};

#endif //_SHIELDS_SETTINGS_H_