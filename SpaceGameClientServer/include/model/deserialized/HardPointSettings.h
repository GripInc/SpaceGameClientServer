#ifndef _HARD_POINT_SETTINGS_H_
#define _HARD_POINT_SETTINGS_H_

#include "utils/XMLHelper.h"
#include "LinearMath/btVector3.h"

class HardPointSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= XMLHelper::getIntFrom(_node, "index", mIndex);
		success &= XMLHelper::getFloatFrom(_node, "roll", mRoll);

		float tempPosX, tempPosY, tempPosZ;
		XMLHelper::getFloatFrom(_node, "posX", tempPosX);
		XMLHelper::getFloatFrom(_node, "posY", tempPosY);
		XMLHelper::getFloatFrom(_node, "posZ", tempPosZ);
		mPosition = btVector3(tempPosX, tempPosY, tempPosZ);

		return success;
	}

	int mIndex = 0;
	btVector3 mPosition;
	float mRoll = 0.f; //roll value of the weapon attached around the z axis
};

#endif //_HARD_POINT_SETTINGS_H_