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
	instantiateObjectSceneNode(mObjectSettings->mInitialOrientation, mObjectSettings->mInitialPosition, mObjectSettings->mInitialScale, mObjectSettings->mMesh, mObjectSettings->mName);
	
	instantiateObjectParts();

	instantiateCollisionObject();
}

void StaticObject::instantiateCollisionObject()
{
	if(mCompoundShape != nullptr)
	{
		const StaticObjectSettings* staticObjectSettings = static_cast<const StaticObjectSettings*>(mObjectSettings);

		btTransform startTransform(convert(staticObjectSettings->mInitialOrientation), convert(staticObjectSettings->mInitialPosition));
		mMyMotionState = new MyMotionState(mSceneNode, startTransform);
		mRigidBody = createRigidBody(startTransform, mCompoundShape, mMyMotionState, 0.f);
		mRigidBody->setRestitution(DEFAULT_RESTITUTION_VALUE);
		mDynamicWorld->addRigidBody(mRigidBody);
	}
}

void StaticObject::instantiateObjectParts()
{
	const StaticObjectSettings* staticObjectSettings = static_cast<const StaticObjectSettings*>(mObjectSettings);

	mObjectParts.resize(staticObjectSettings->mObjectParts.size());
	for(int i = 0; i < staticObjectSettings->mObjectParts.size(); ++i)
	{
		const ObjectPartSettings& newObjectPartSettings = staticObjectSettings->mObjectParts[i];

		ObjectPart& newObjectPart = mObjectParts[i];
		newObjectPart.init(newObjectPartSettings);
		
		const btAlignedObjectArray<CollisionShapeSettings>& collisionShapesSettings = newObjectPartSettings.mCollisionShapes;
		mCollisionShapes.resize(collisionShapesSettings.size());
		for(int j = 0; j < collisionShapesSettings.size(); ++j)
		{
			const CollisionShapeSettings& collisionShapeSettings = collisionShapesSettings[j];

			btCollisionShape* collisionShape = ObjectPart::createCollisionShape(collisionShapeSettings, &newObjectPart);
			mCollisionShapes[j] = collisionShape;

			if(mCompoundShape == nullptr)
			{
				mCompoundShape = new btCompoundShape();
				mCompoundShape->setUserPointer(this);
			}

			mCompoundShape->addChildShape(btTransform(collisionShapeSettings.mInitialOrientation, collisionShapeSettings.mInitialPosition), collisionShape);
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

	mObjectParts.clear();

	delete mCompoundShape;
	mCompoundShape = NULL;
	delete mRigidBody;
	mRigidBody = NULL;

	SectorObject::destroy();
}

btRigidBody* StaticObject::createRigidBody(const btTransform& _startTransform, btCollisionShape* _shape, MyMotionState* _myMotionState, float _mass, const btVector3& _overrideInertia /* = btVector3(0.f, 0.f, 0.f) */)
{
	//btAssert(!_shape || _shape->getShapeType() != INVALID_SHAPE_PROXYTYPE);
	//btAssert(_myMotionState);

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