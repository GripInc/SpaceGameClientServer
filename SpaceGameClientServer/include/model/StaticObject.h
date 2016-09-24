#ifndef _STATIC_OBJECT_H_
#define _STATIC_OBJECT_H_

#include "model/SectorObject.h"
#include "model/MyMotionState.h"

#include "model/deserialized/StaticObjectSettings.h"

#include "LinearMath/btAlignedObjectArray.h"

class ObjectPart;
class StaticObjectSettings;
class btCompoundShape;
class btCollisionShape;
class btRigidBody;

class StaticObject : public SectorObject
{
public:
	const static float DEFAULT_RESTITUTION_VALUE;
	
	StaticObject() {}

	StaticObject(const StaticObjectSettings* _staticObjectSettings, Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld) 
		: SectorObject(_staticObjectSettings, _sceneManager),
		mRigidBody(NULL),
		mDynamicWorld(_dynamicWorld),
		mCompoundShape(NULL),
		mMyMotionState(NULL)
	{}

    ~StaticObject();

	virtual void instantiateObject();

	const btAlignedObjectArray<ObjectPart*>& getObjectParts() const { return mObjectParts; }
	btRigidBody* getRigidBody() const { return mRigidBody; }

	virtual void destroy();

	void forceWorldTransform(const btTransform& _worldTransform);

protected:
	virtual void instantiateCollisionObject();
	virtual void instantiateObjectParts();

	btDiscreteDynamicsWorld* mDynamicWorld = nullptr;
	btCompoundShape* mCompoundShape = nullptr;
	btRigidBody* mRigidBody = nullptr;
	MyMotionState* mMyMotionState = nullptr;
	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

	btRigidBody* createRigidBody(const btTransform& _startTransform, btCollisionShape* _shape, MyMotionState* _myMotionState, float _mass, const btVector3& _overrideInertia = btVector3(0.f, 0.f, 0.f));

	btAlignedObjectArray<ObjectPart*> mObjectParts;
};

#endif //_STATIC_OBJECT_H_