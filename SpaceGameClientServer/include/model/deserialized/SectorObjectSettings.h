#ifndef _SECTOR_OBJECT_SETTINGS_H_
#define _SECTOR_OBJECT_SETTINGS_H_

#include "model/deserialized/NamedSettings.h"
#include "OgreVector3.h"

class SectorObjectSettings : public NamedSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= NamedSettings::deserialize(_node);

		float tempPosX = 0.f, tempPosY = 0.f, tempPosZ = 0.f;
		XMLHelper::getFloatFrom(_node, "posX", tempPosX);
		XMLHelper::getFloatFrom(_node, "posY", tempPosY);
		XMLHelper::getFloatFrom(_node, "posZ", tempPosZ);
		mInitialPosition = Ogre::Vector3(tempPosX, tempPosY, tempPosZ);

		XMLHelper::getStringFrom(_node, "mesh", mMesh);

		XMLHelper::getStringFrom(_node, "primitive", mPrimitive);

		float tempScaleX = 1.f, tempScaleY = 1.f, tempScaleZ = 1.f;
		XMLHelper::getFloatFrom(_node, "scaleX", tempScaleX);
		XMLHelper::getFloatFrom(_node, "scaleY", tempScaleY);
		XMLHelper::getFloatFrom(_node, "scaleZ", tempScaleZ);
		mInitialScale = Ogre::Vector3(tempScaleX, tempScaleY, tempScaleZ);

		float tempRoll = 0.f, tempPitch = 0.f, tempYaw = 0.f;
		XMLHelper::getFloatFrom(_node, "roll", tempRoll);
		XMLHelper::getFloatFrom(_node, "pitch", tempPitch);
		XMLHelper::getFloatFrom(_node, "yaw", tempYaw);
		mInitialOrientation = Ogre::Quaternion(Ogre::Radian(tempYaw), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Radian(tempPitch), Ogre::Vector3::UNIT_X) * Ogre::Quaternion(Ogre::Radian(tempRoll), Ogre::Vector3::UNIT_Z);

		return success;
	}

	Ogre::Vector3 mInitialPosition;
	Ogre::Vector3 mInitialScale;
	Ogre::Quaternion mInitialOrientation;
	std::string mMesh;
	std::string mPrimitive;
};

#endif //_SECTOR_OBJECT_SETTINGS_H_