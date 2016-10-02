#ifndef _OBJECT_PART_H_
#define _OBJECT_PART_H_

#include <string>

#include "btBulletCollisionCommon.h"

class CollisionShapeSettings;
class ObjectPartSettings;

class ObjectPart
{
public:
	void init(const ObjectPartSettings& _objectPartSettings);
	static btCollisionShape* createCollisionShape(const CollisionShapeSettings& _collisionShapeSettings, void* _userPointer);

	const std::string& getName() const { return mName; }

	int mHitPoints = 0;

protected:
	std::string mName;
};

#endif //_SHIP_PART_H_