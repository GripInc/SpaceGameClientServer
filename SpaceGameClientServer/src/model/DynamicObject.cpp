#include "model/DynamicObject.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "OgreSceneNode.h"

#include "utils/OgreBulletConvert.h"

void DynamicObject::instantiateCollisionObject()
{
	if(mCompoundShape != NULL)
	{
		const DynamicObjectSettings* dynamicObjectSettings = static_cast<const DynamicObjectSettings*>(mObjectSettings);

		btTransform startTransform = btTransform(convert(dynamicObjectSettings->mInitialOrientation), convert(dynamicObjectSettings->mInitialPosition));
		
		mRigidBody = createRigidBody(startTransform, mCompoundShape, dynamicObjectSettings->mMass, getInertia());
		mRigidBody->setRestitution(DEFAULT_RESTITUTION_VALUE);
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
		mRigidBody->setDamping(dynamicObjectSettings->mLinearDamping, dynamicObjectSettings->mAngularDamping);
		mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK); //CF_CUSTOM_MATERIAL_CALLBACK allows the trigger of MyContactCallback on contact added
		mDynamicWorld->addRigidBody(mRigidBody);

		forceWorldTransform(startTransform);
	}
}

void DynamicObject::destroy()
{
	//StaticObject::destroy();
}

void DynamicObject::updateView(SectorTick _sectorTick)
{
	DynamicObjectState output;
	mStateManager.getState(_sectorTick, output);

	Ogre::Quaternion orientation = convert(output.mWorldTransform.getRotation());
	Ogre::Vector3 position = convert(output.mWorldTransform.getOrigin());

	mSceneNode->setPosition(position);
	mSceneNode->setOrientation(orientation);
}

void DynamicObject::saveState(SectorTick _tick)
{
	mStateManager.saveState(_tick, DynamicObjectState(mRigidBody));
}
