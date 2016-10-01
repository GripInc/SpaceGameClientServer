#ifndef _BULLET_CALLBACKS_H_
#define _BULLET_CALLBACKS_H_

#include "LinearMath/btScalar.h"

class btDynamicsWorld;
class btManifoldPoint;
struct btCollisionObjectWrapper;

void MyTickCallback(btDynamicsWorld* _world, btScalar _timeStep);
bool MyContactCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

#endif //_BULLET_CALLBACKS_H_