#include "model/Ship.h"

#include "model/GameSettings.h"
#include "model/ObjectPart.h"
#include "model/HardPoint.h"

#include "OgreSceneNode.h"

#include "utils/OgreBulletConvert.h"
#include "utils/StringUtils.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "view/PlayerCamera.h"

#include "manager/LoggerManager.h"

void Ship::initShip(const ShipSettings* _shipSettings)
{
	mObjectSettings = _shipSettings;

	mMaxYawRate = _shipSettings->mMaxYawRate;
	mMaxPitchRate = _shipSettings->mMaxPitchRate;
	mMaxRollRate = _shipSettings->mMaxRollRate;
	mCargoSpace = _shipSettings->mCargoSpace;

	mHardPoints.resize(_shipSettings->mHardPoints.size());
	for(int i = 0; i < _shipSettings->mHardPoints.size(); ++i)
	{
		mHardPoints[_shipSettings->mHardPoints[i].mIndex] = new HardPoint(_shipSettings->mHardPoints[i].mIndex, _shipSettings->mHardPoints[i].mPosition, _shipSettings->mHardPoints[i].mRoll);
	}
}

void Ship::instantiateObject(Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld, UniqueId _uniqueId)
{
	mSceneManager = _sceneManager;
	mDynamicWorld = _dynamicWorld;

	DynamicObject(static_cast<const DynamicObjectSettings*>(mObjectSettings), mSceneManager, mDynamicWorld, _uniqueId);

	//We explicitely want the graphics only
	SectorObject::instantiateObject();

	for(int i = 0; i < mHardPoints.size(); ++i)
	{
		if(mHardPoints[i]->isUsed())
		{
			//Find the weapon attached to hardpoint
			const WeaponSettings& weaponSettings = mHardPoints[i]->getWeaponSettings();

			addSubSceneNode(convert(btQuaternion(btVector3(0.f, 0.f, 1.f), mHardPoints[i]->getRoll())), convert(weaponSettings.mHardPoint.mPosition + mHardPoints[i]->getPosition()), Ogre::Vector3(1.f ,1.f ,1.f), weaponSettings.mMesh, "hardpoint_" + StringUtils::toStr(i) + weaponSettings.mName, mSceneManager);
		}
	}

	instantiateObjectParts();

	instantiateCollisionObject();

	const ShipSettings* shipSettings = static_cast<const ShipSettings*>(mObjectSettings);

	//Attach camera node to ship node
	Ogre::SceneNode* cameraSceneNode = PlayerCamera::getInstance().getCameraNode();
	mSceneNode->addChild(cameraSceneNode);
	cameraSceneNode->setPosition(Ogre::Vector3(shipSettings->mHeadPosX, shipSettings->mHeadPosY, shipSettings->mHeadPosZ));
	cameraSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
}

void Ship::instantiateObjectParts()
{
	DynamicObject::instantiateObjectParts();

	//Loop on used hardpoints
	for(int i = 0; i < mHardPoints.size(); ++i)
	{
		if(mHardPoints[i]->isUsed())
		{
			mObjectParts.push_back(new ObjectPart());
			ObjectPartSettings objectPartSettings;
			objectPartSettings.mName = "hardPoint_" + StringUtils::toStr(mHardPoints[i]->getIndex());
			objectPartSettings.mHitPoints = mHardPoints[i]->getWeaponSettings().mHitPoints;
			mObjectParts[mObjectParts.size() - 1]->init(objectPartSettings);

			//Get the weapon attached to hardpoint
			const WeaponSettings& weaponSettings = mHardPoints[i]->getWeaponSettings();

			//Add shapes
			const btAlignedObjectArray<CollisionShapeSettings>& collisionShapesSettings = weaponSettings.mCollisionShapes;
			for(int j = 0; j < collisionShapesSettings.size(); ++j)
			{
				btCollisionShape* collisionShape = mObjectParts[mObjectParts.size() - 1]->createCollisionShape(collisionShapesSettings[j]);
				mCollisionShapes.push_back(collisionShape);
				mCompoundShape->addChildShape(btTransform(collisionShapesSettings[j].mInitialOrientation * btQuaternion(btVector3(0.f, 0.f, 1.f), mHardPoints[i]->getRoll()), collisionShapesSettings[j].mInitialPosition + weaponSettings.mHardPoint.mPosition + mHardPoints[i]->getPosition()), collisionShape);
			}
		}
	}
}

void Ship::addEngine(const EngineSettings& _engine)
{
	mEngine.init(_engine);
}

void Ship::addDirectional(const DirectionalSettings& _directional)
{
	mDirectional.init(_directional);
}

void Ship::addWeapon(const WeaponSettings& _weapon, int _index)
{
	mHardPoints[_index]->attachWeapon(_weapon);
}

const WeaponSettings& Ship::removeWeapon(int _index)
{
	const WeaponSettings& result = mHardPoints[_index]->getWeaponSettings();
	mHardPoints[_index]->detachWeapon();

	return result;
}

void Ship::updateForces()
{
	btTransform shipTransform;
	mMyMotionState->getWorldTransform(shipTransform);
	
	//Compute thrust force
	float currentSpeed = mRigidBody->getLinearVelocity().length();
	float force = 0.f;
	if(currentSpeed < mEngine.getMaxSpeed())
		force = mEnginePotentialForce * std::log(mEngine.getMaxSpeed() + 1.f - currentSpeed) / std::log(mEngine.getMaxSpeed() + 1.f);

	//DEBUG
	mEngineRealForce = force;
	
	//Apply thrust
	btVector3 thrustForce(0.f, 0.f, force);
	thrustForce = rotate(thrustForce, shipTransform.getRotation());
	mRigidBody->applyCentralForce(thrustForce);

	//TODO try less instructions
	//Apply rotations
	btVector3 torque;
	torque.setValue(mCurrentPitchForce, 0.f, 0.f);
	torque = rotate(torque, shipTransform.getRotation());
	mRigidBody->applyTorque(torque);

	torque.setValue(0.f, -mCurrentYawForce, 0.f);
	torque = rotate(torque, shipTransform.getRotation());
	mRigidBody->applyTorque(torque);

	torque.setValue(0.f, 0.f, -mCurrentRollForce);
	torque = rotate(torque, shipTransform.getRotation());
	mRigidBody->applyTorque(torque);
}

const btVector3& Ship::getLinearVelocity() const
{
	return mRigidBody->getLinearVelocity();
}

void Ship::destroy()
{
	DynamicObject::destroy();
}

void Ship::updateHardPoints(float _deltaTime)
{
	for(int i = 0; i < mHardPoints.size(); ++i)
	{
		if(mHardPoints[i]->isUsed())
			mHardPoints[i]->mElapsedTime += _deltaTime;
	}
}

void Ship::saveState(SectorTick _tick)
{
	std::vector<std::pair<int, float> > hardpointsState;
	int hardpointsSize = mHardPoints.size();
	for(int i = 0; i < hardpointsSize; ++i)
	{
		if(mHardPoints[i]->isUsed())
		{
			hardpointsState.push_back(std::make_pair(mHardPoints[i]->getIndex(), mHardPoints[i]->mElapsedTime));
		}
	}

	mStateManager.saveState(_tick, ShipState(mRigidBody, mCurrentRollForce, mCurrentYawForce, mCurrentPitchForce, mEnginePotentialForce, hardpointsState));
}

void Ship::overrideSavedState(SectorTick _tick, const ShipState& _shipState)
{
	mStateManager.saveState(_tick, _shipState);
}

void Ship::setState(SectorTick _tick)
{
	ShipState shipState;

	if(!mStateManager.getState(_tick, shipState))
		return;

	//Rigid body
	mRigidBody->setWorldTransform(shipState.mWorldTransform);
	mRigidBody->setLinearVelocity(shipState.mLinearVelocity);
	mRigidBody->setAngularVelocity(shipState.mAngularVelocity);
	mRigidBody->applyCentralForce(shipState.mTotalForce);
	mRigidBody->applyTorque(shipState.mTotalTorque);

	//Systems
	mCurrentRollForce = shipState.mCurrentRollForce;
	mCurrentYawForce = shipState.mCurrentYawForce;
	mCurrentPitchForce = shipState.mCurrentPitchForce;

	mEnginePotentialForce = shipState.mEnginePotentialForce;

	//Hardpoints
	const std::vector<std::pair<int, float> >& harpointsState = shipState.mHarpointsState;
	size_t hardpointsSize = harpointsState.size();
	for (size_t i = 0; i < hardpointsSize; ++i)
	{
		int index = harpointsState[i].first;
		if(mHardPoints[index]->isUsed())
		{
			mHardPoints[index]->mElapsedTime = harpointsState[i].second;
		}
	}
}

//bool Ship::checkAndFixState(const ShipState& _state)
//{
//	ShipState currentState;
	/*mSateManager.

	bool result = compareStates(currentState, _state);

	//If too much difference we fix it
	if(!result)
	{
		mRigidBody->clearForces();
		mRigidBody->setLinearVelocity(_state.mLinearVelocity);
		mRigidBody->setAngularVelocity(_state.mAngularVelocity);
		mRigidBody->applyCentralForce(_state.mTotalForce);
		mRigidBody->applyTorque(_state.mTotalTorque);
		this->forceWorldTransform(_state.mWorldTransform);
	}*/
	
//	return true;//result;
//}