#ifndef _DYNAMIC_OBJECT_SETTINGS_H_
#define _DYNAMIC_OBJECT_SETTINGS_H_

#include "model/deserialized/StaticObjectSettings.h"

class DynamicObjectSettings : public StaticObjectSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;
		success &= StaticObjectSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "mass", mMass);
		XMLHelper::getFloatFrom(_node, "linearDamping", mLinearDamping);
		XMLHelper::getFloatFrom(_node, "angularDamping", mAngularDamping);

		float tempYawInertia, tempPitchInertia, tempRollInertia;
		XMLHelper::getFloatFrom(_node, "yawInertia", tempYawInertia);
		XMLHelper::getFloatFrom(_node, "pitchInertia", tempPitchInertia);
		XMLHelper::getFloatFrom(_node, "rollInertia", tempRollInertia);
		mLocalInertia = btVector3(tempPitchInertia, tempYawInertia, tempRollInertia);

		return success;
	}

	float mMass = 0.f;
	float mLinearDamping = 0.f;
	float mAngularDamping = 0.f;
	btVector3 mLocalInertia;
};

#endif //_DYNAMIC_OBJECT_SETTINGS_H_