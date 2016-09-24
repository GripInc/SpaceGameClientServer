#include "model/StaticObject.h"

#include "model/ObjectPart.h"
#include "model/deserialized/StaticObjectSettings.h"

#include "OgreSceneNode.h"

#include "utils/OgreBulletConvert.h"
#include "utils/OgreUtils.h"
#include "utils/StringUtils.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

const float StaticObject::DEFAULT_RESTITUTION_VALUE = 0.8f;

void StaticObject::instantiateObject()
{
	SectorObject::instantiateObject();
	
	instantiateObjectParts();

	instantiateCollisionObject();
}

void StaticObject::instantiateCollisionObject()
{
	if(mCompoundShape != NULL)
	{
		const StaticObjectSettings* staticObjectSettings = static_cast<const StaticObjectSettings*>(mObjectSettings);

		btTransform startTransform = btTransform(convert(staticObjectSettings->mInitialOrientation), convert(staticObjectSettings->mInitialPosition));
		mMyMotionState = new MyMotionState(mSceneNode, startTransform);
		mRigidBody = createRigidBody(startTransform, mCompoundShape, mMyMotionState, 0.f);
		mRigidBody->setRestitution(DEFAULT_RESTITUTION_VALUE);
		mDynamicWorld->addRigidBody(mRigidBody);
	}
}

void StaticObject::instantiateObjectParts()
{
	const StaticObjectSettings* staticObjectSettings = static_cast<const StaticObjectSettings*>(mObjectSettings);

	for(int i = 0; i < staticObjectSettings->mObjectParts.size(); ++i)
	{
		mObjectParts.push_back(new ObjectPart());
		ObjectPartSettings objectPartSettings;
		objectPartSettings.mName = staticObjectSettings->mObjectParts[i].mName;
		objectPartSettings.mHitPoints = staticObjectSettings->mObjectParts[i].mHitPoints;
		mObjectParts[mObjectParts.size() - 1]->init(objectPartSettings);
		
		const btAlignedObjectArray<CollisionShapeSettings>& collisionShapesSettings = staticObjectSettings->mObjectParts[i].mCollisionShapes;
		for(int j = 0; j < collisionShapesSettings.size(); ++j)
		{
			btCollisionShape* collisionShape = mObjectParts[mObjectParts.size() - 1]->createCollisionShape(collisionShapesSettings[j]);
			mCollisionShapes.push_back(collisionShape);

			if(mCompoundShape == NULL)
			{
				mCompoundShape = new btCompoundShape();
				mCompoundShape->setUserPointer(this);
			}

			mCompoundShape->addChildShape(btTransform(collisionShapesSettings[j].mInitialOrientation, collisionShapesSettings[j].mInitialPosition), collisionShape);
		}
	}
}

StaticObject::~StaticObject()
{
	SectorObject::~SectorObject();
}

void StaticObject::destroy()
{
	if(mRigidBody)
		mDynamicWorld->removeRigidBody(mRigidBody);

	delete mMyMotionState;
	mMyMotionState = NULL;
	for(int i = 0; i < mCollisionShapes.size(); ++i)
	{
		delete mCollisionShapes[i];
	}
	mCollisionShapes.clear();

	for(int i = 0; i < mObjectParts.size(); ++i)
	{
		delete mObjectParts[i];
	}
	mObjectParts.clear();

	delete mCompoundShape;
	mCompoundShape = NULL;
	delete mRigidBody;
	mRigidBody = NULL;

	SectorObject::destroy();
}

btRigidBody* StaticObject::createRigidBody(const btTransform& _startTransform, btCollisionShape* _shape, MyMotionState* _myMotionState, float _mass, const btVector3& _overrideInertia /* = btVector3(0.f, 0.f, 0.f) */)
{
	btAssert(!_shape || _shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);
	btAssert(_myMotionState);

	btVector3 localInertia = _overrideInertia;
	if (_mass != 0.f && localInertia.isZero())
		_shape->calculateLocalInertia(_mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo cInfo(_mass, _myMotionState, _shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);

	return body;
}

void StaticObject::forceWorldTransform(const btTransform& _worldTransform)
{
	mMyMotionState->setWorldTransform(_worldTransform);
	mRigidBody->setWorldTransform(_worldTransform);
}