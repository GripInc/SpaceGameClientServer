#ifndef _SHIP_PART_COLLISION_SHAPE_H_
#define _SHIP_PART_COLLISION_SHAPE_H_

#include <string>

#include "OgreVector3.h"

namespace tinyxml2
{
	class XMLElement;
}

class ShipPartCollisionShape
{
public:
	ShipPartCollisionShape() 
		: mPosition(Ogre::Vector3::ZERO),
		mScale(Ogre::Vector3::ZERO),
		mOrientation(Ogre::Quaternion::IDENTITY)
	{}

	bool deserialize(tinyxml2::XMLElement* _sectorNode);

protected:
	std::string mMesh;
	std::string mPrimitive;
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mScale;
	Ogre::Quaternion mOrientation;
};

#endif //_SHIP_PART_COLLISION_SHAPE_H_