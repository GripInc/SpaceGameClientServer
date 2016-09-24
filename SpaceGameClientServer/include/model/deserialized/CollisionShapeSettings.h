#ifndef _COLLISION_SHAPE_SETTINGS_H_
#define _COLLISION_SHAPE_SETTINGS_H_

#include <string>
#include "utils/XMLHelper.h"
#include "LinearMath/btQuaternion.h"

class CollisionShapeSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		float tempPosX, tempPosY, tempPosZ;
		XMLHelper::getFloatFrom(_node, "posX", tempPosX);
		XMLHelper::getFloatFrom(_node, "posY", tempPosY);
		XMLHelper::getFloatFrom(_node, "posZ", tempPosZ);
		mInitialPosition = btVector3(tempPosX, tempPosY, tempPosZ);

		XMLHelper::getStringFrom(_node, "mesh", mMesh);

		XMLHelper::getStringFrom(_node, "primitive", mPrimitive);

		float tempScaleX, tempScaleY, tempScaleZ;
		XMLHelper::getFloatFrom(_node, "scaleX", tempScaleX, 1.f);
		XMLHelper::getFloatFrom(_node, "scaleY", tempScaleY, 1.f);
		XMLHelper::getFloatFrom(_node, "scaleZ", tempScaleZ, 1.f);
		mInitialScale = btVector3(tempScaleX, tempScaleY, tempScaleZ);

		float tempRoll, tempPitch, tempYaw;
		XMLHelper::getFloatFrom(_node, "roll", tempRoll);
		XMLHelper::getFloatFrom(_node, "pitch", tempPitch);
		XMLHelper::getFloatFrom(_node, "yaw", tempYaw);
		//Keep commented for archive
		//mOrientation = Ogre::Quaternion(Ogre::Radian(tempYaw), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Radian(tempPitch), Ogre::Vector3::UNIT_X) * Ogre::Quaternion(Ogre::Radian(tempRoll), Ogre::Vector3::UNIT_Z);
		mInitialOrientation = btQuaternion(tempYaw, tempPitch, tempRoll);

		XMLHelper::getBoolFrom(_node, "needConvexHullShape", mNeedConvexHullShape);

		return success;
	}

	std::string mMesh;
	std::string mPrimitive;
	//Transform are relative to container object
	btVector3 mInitialPosition;
	btVector3 mInitialScale;
	btQuaternion mInitialOrientation;
	bool mNeedConvexHullShape = false;
};

#endif //_COLLISION_SHAPE_SETTINGS_H_