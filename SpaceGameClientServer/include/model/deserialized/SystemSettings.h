#ifndef _SYSTEM_SETTINGS_H_
#define _SYSTEM_SETTINGS_H_

#include "model/deserialized/Types.h"
#include "model/deserialized/NamedSettings.h"

class SystemSettings : public NamedSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;
	
		success &= NamedSettings::deserialize(_node);

		XMLHelper::getUnsignedIntFrom(_node, "space", mSpace);
		XMLHelper::getUnsignedIntFrom(_node, "hitPoints", mHitPoints);
		XMLHelper::getUnsignedIntFrom(_node, "mass", mMass);

		return success;
	}

	unsigned int mSpace = 0U;
	unsigned int mHitPoints = 0U;
	unsigned int mMass = 0U;
};

#endif //_SYSTEM_SETTINGS_H_