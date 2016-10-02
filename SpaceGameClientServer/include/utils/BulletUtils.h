#ifndef _BULLET_UTILS_H_
#define _BULLET_UTILS_H_

#include "LinearMath/btScalar.h"

class btDynamicsWorld;
class btManifoldPoint;
struct btCollisionObjectWrapper;
class btCollisionShape;
class CollisionShapeSettings;

void MyTickCallback(btDynamicsWorld* _world, btScalar _timeStep);
bool MyContactCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
btCollisionShape* createCollisionShape(const CollisionShapeSettings& _collisionShapeSettings, void* _userPointer);

#endif //_BULLET_UTILS_H_