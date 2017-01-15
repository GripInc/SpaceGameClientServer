#ifndef _STATIC_OBJECT_H_
#define _STATIC_OBJECT_H_

#include "model/SectorObject.h"
#include "model/MyMotionState.h"
#include "model/ObjectPart.h"

#include "model/deserialized/StaticObjectSettings.h"

#include "LinearMath/btAlignedObjectArray.h"

class StaticObjectSettings;
class btCompoundShape;
class btCollisionShape;
class btRigidBody;
class btDiscreteDynamicsWorld;

class StaticObject : public SectorObject
{
public:
	const static float DEFAULT_RESTITUTION_VALUE;
	
	void init(const SectorObjectSettings* _sectorObjectSettings, Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld)
	{
		SectorObject::init(_sectorObjectSettings, _sceneManager);

		mDynamicWorld = _dynamicWorld;
	}

    virtual ~StaticObject();

	virtual void instantiateObject() override;

	const btAlignedObjectArray<ObjectPart>& getObjectParts() const { return mObjectParts; }
	btRigidBody* getRigidBody() const { return mRigidBody; }

	virtual void destroy() override;

	void forceWorldTransform(const btTransform& _worldTransform);

protected:
	virtual void instantiateCollisionObject();
	virtual void instantiateObjectParts();

	btDiscreteDynamicsWorld* mDynamicWorld = nullptr;
	btCompoundShape* mCompoundShape = nullptr;
	btRigidBody* mRigidBody = nullptr;

	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

	btRigidBody* createRigidBody(const btTransform& _startTransform, btCollisionShape* _shape, float _mass, const btVector3& _overrideInertia = btVector3(0.f, 0.f, 0.f));

	btAlignedObjectArray<ObjectPart> mObjectParts;
};

#endif //_STATIC_OBJECT_H_