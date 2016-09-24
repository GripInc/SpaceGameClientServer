#include "model/ShipPartCollisionShape.h"

#include "utils/XMLHelper.h"

bool ShipPartCollisionShape::deserialize(tinyxml2::XMLElement* _node)
{
	bool succes = true;

	float tempPosX = 0.f, tempPosY = 0.f, tempPosZ = 0.f;
	XMLHelper::getFloatFrom(_node, "posX", tempPosX);
	XMLHelper::getFloatFrom(_node, "posY", tempPosY);
	XMLHelper::getFloatFrom(_node, "posZ", tempPosZ);
	mPosition = Ogre::Vector3(tempPosX, tempPosY, tempPosZ);

	XMLHelper::getStringFrom(_node, "mesh", mMesh);

	XMLHelper::getStringFrom(_node, "primitive", mPrimitive);

	float tempScaleX = 0.f, tempScaleY = 0.f, tempScaleZ = 0.f;
	XMLHelper::getFloatFrom(_node, "scaleX", tempScaleX);
	XMLHelper::getFloatFrom(_node, "scaleY", tempScaleY);
	XMLHelper::getFloatFrom(_node, "scaleZ", tempScaleZ);
	mScale = Ogre::Vector3(tempScaleX, tempScaleY, tempScaleZ);

	float tempRoll = 0.f, tempPitch = 0.f, tempYaw = 0.f;
	XMLHelper::getFloatFrom(_node, "roll", tempRoll);
	XMLHelper::getFloatFrom(_node, "pitch", tempPitch);
	XMLHelper::getFloatFrom(_node, "yaw", tempYaw);
	mOrientation = Ogre::Quaternion(Ogre::Radian(tempYaw), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Radian(tempPitch), Ogre::Vector3::UNIT_X) * Ogre::Quaternion(Ogre::Radian(tempRoll), Ogre::Vector3::UNIT_Z);

	return succes;
}