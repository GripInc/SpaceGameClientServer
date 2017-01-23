#include "model/Ship.h"

#include "model/GameSettings.h"
#include "model/ObjectPart.h"
#include "model/HardPoint.h"

#include "OgreSceneNode.h"

#include "utils/OgreBulletConvert.h"
#include "utils/BulletUtils.h"
#include "utils/StringUtils.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "view/PlayerCamera.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "Ship";
}

void Ship::initModel(const ShipSettings* _shipSettings)
{
	mObjectSettings = _shipSettings;

	mMaxYawRate = _shipSettings->mMaxYawRate;
	mMaxPitchRate = _shipSettings->mMaxPitchRate;
	mMaxRollRate = _shipSettings->mMaxRollRate;
	mCargoSpace = _shipSettings->mCargoSpace;

	const btAlignedObjectArray<HardPointSettings>& hardPointSettingsList = _shipSettings->mHardPoints;
	mHardPoints.resize(hardPointSettingsList.size());
	for (int i = 0; i < hardPointSettingsList.size(); ++i)
	{
		const HardPointSettings& hardPointSetting = hardPointSettingsList[i];
		mHardPoints[hardPointSetting.getIndex()].init(hardPointSetting.getIndex(), hardPointSetting.getPosition(), hardPointSetting.getRoll());
	}
}

void Ship::init(Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld, UniqueId _uniqueId)
{
	const ShipSettings* shipSettings = static_cast<const ShipSettings*>(mObjectSettings);

	DynamicObject::init(shipSettings, _sceneManager, _dynamicWorld, _uniqueId);
}

void Ship::instantiateObject()
{
	//Code from static object::instantiateObject
	instantiateObjectSceneNode(mObjectSettings->mInitialOrientation, mObjectSettings->mInitialPosition, mObjectSettings->mInitialScale, mObjectSettings->mMesh, mObjectSettings->getName());
	
	for (int i = 0; i < mHardPoints.size(); ++i)
	{
		HardPoint& hardPoint = mHardPoints[i];

		if (hardPoint.isUsed())
		{
			//Find the weapon attached to hardpoint
			const Weapon& weapon = hardPoint.getWeapon();

			addSubSceneNode(convert(btQuaternion(btVector3(0.f, 0.f, 1.f), hardPoint.getRoll())), convert(weapon.getHardPointPosition() + hardPoint.getPosition()), Ogre::Vector3(1.f, 1.f, 1.f), weapon.getMesh(), "hardpoint_" + StringUtils::toStr(i));
		}
	}

	instantiateObjectParts();
	
	instantiateCollisionObject();
}

void Ship::attachCamera(Ogre::SceneNode* _cameraSceneNode)
{
	const ShipSettings* shipSettings = static_cast<const ShipSettings*>(mObjectSettings);

	//Attach camera node to ship node
	mSceneNode->addChild(_cameraSceneNode);
	_cameraSceneNode->setPosition(Ogre::Vector3(shipSettings->mHeadPosX, shipSettings->mHeadPosY, shipSettings->mHeadPosZ));
	_cameraSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
}

void Ship::instantiateObjectParts()
{
	DynamicObject::instantiateObjectParts();

	//Loop on used hardpoints
	for(int i = 0; i < mHardPoints.size(); ++i)
	{
		HardPoint hardPoint = mHardPoints[i];

		if(hardPoint.isUsed())
		{
			mObjectParts.push_back(ObjectPart());
			ObjectPart& objectPart = mObjectParts[mObjectParts.size() - 1];

			//Get the weapon attached to hardpoint
			const Weapon& weapon = hardPoint.getWeapon();

			//Init object part
			objectPart.init("hardPoint_" + StringUtils::toStr(hardPoint.getIndex()), weapon.getBaseHitPoints());

			//Add shapes
			const btAlignedObjectArray<CollisionShapeSettings>& collisionShapesSettings = weapon.getCollisionShapes();
			for(int j = 0; j < collisionShapesSettings.size(); ++j)
			{
				btCollisionShape* collisionShape = createCollisionShape(collisionShapesSettings[j], &objectPart);
				mCollisionShapes.push_back(collisionShape);
				mCompoundShape->addChildShape(btTransform(collisionShapesSettings[j].mInitialOrientation * btQuaternion(btVector3(0.f, 0.f, 1.f), hardPoint.getRoll()), collisionShapesSettings[j].mInitialPosition + weapon.getHardPointPosition() + hardPoint.getPosition()), collisionShape);
			}
		}
	}
}

void Ship::addEngine(const EngineSettings* _engine)
{
	mEngine.init(_engine);
}

void Ship::addDirectional(const DirectionalSettings* _directional)
{
	mDirectional.init(_directional);
}

void Ship::addWeapon(const WeaponSettings* _weapon, int _index)
{
	mHardPoints[_index].attachWeapon(_weapon);
}

void Ship::removeWeapon(int _index)
{
	mHardPoints[_index].detachWeapon();
}

void Ship::updateForces()
{
	const btTransform& shipTransform = mRigidBody->getWorldTransform();
	
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
		if(mHardPoints[i].isUsed())
			mHardPoints[i].update(_deltaTime);
	}
}

void Ship::saveState(SectorTick _tick)
{
	std::vector<std::pair<int, float> > hardpointsState;
	int hardpointsSize = mHardPoints.size();
	for (int i = 0; i < hardpointsSize; ++i)
	{
		if (mHardPoints[i].isUsed())
		{
			hardpointsState.push_back(std::make_pair(mHardPoints[i].getIndex(), mHardPoints[i].getWeapon().mElapsedTime));
		}
	}

	mStateManager.saveState(_tick, ShipState(mRigidBody, mCurrentRollForce, mCurrentYawForce, mCurrentPitchForce, mEngine.mWantedThrust, mEngine.mRealThrust, hardpointsState));
}

void Ship::updateView(SectorTick _sectorTick, float _elapsedTime, float _sectorUpdateRate)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "START at tick " + StringUtils::toStr(_sectorTick) + "; _elapsedTime is " + StringUtils::toStr(_elapsedTime) + "; _sectorUpdateRate is:" + StringUtils::toStr(_sectorUpdateRate), false);

	ShipState shipSateFromTickminusOne, shipSateFromTick;
	mStateManager.getState(_sectorTick - 1, shipSateFromTickminusOne);
	mStateManager.getState(_sectorTick, shipSateFromTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "start position:x:" + StringUtils::toStr(shipSateFromTickminusOne.mWorldTransform.getOrigin().x()), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "end position:x:" + StringUtils::toStr(shipSateFromTick.mWorldTransform.getOrigin().x()), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mAccumulatorRest:" + StringUtils::toStr(mAccumulatorRest), false);

	//Loosing time, no interpolation
	if (_elapsedTime >= _sectorUpdateRate)
	{
		mLastTickViewed = _sectorTick;

		mInterpolatedRotation = shipSateFromTickminusOne.mWorldTransform.getRotation();
		mInterpolatedPosition = shipSateFromTickminusOne.mWorldTransform.getOrigin();
	}
	else
	{
		if (mLastTickViewed < _sectorTick)
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "beginning of new tick", false);

			mLastTickViewed = _sectorTick;
			mAccumulator = _elapsedTime;

			mInterpolatedRotation = shipSateFromTickminusOne.mWorldTransform.getRotation();
			mInterpolatedPosition = shipSateFromTickminusOne.mWorldTransform.getOrigin();

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "start position:x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);

			btQuaternion rotationAtTick = shipSateFromTick.mWorldTransform.getRotation();
			btVector3& positionAtTick = shipSateFromTick.mWorldTransform.getOrigin();

			float scalar = mAccumulator / _sectorUpdateRate;

			mInterpolatedRotation = mInterpolatedRotation.slerp(rotationAtTick, scalar);
			mInterpolatedPosition = mInterpolatedPosition.lerp(positionAtTick, scalar);

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mAccumulator:" + StringUtils::toStr(mAccumulator), false);
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "scalar:" + StringUtils::toStr(scalar), false);
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mInterpolatedPosition after :x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);
		}
		else
		{
			mAccumulator += _elapsedTime;

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mAccumulator:" + StringUtils::toStr(mAccumulator), false);

			if (mAccumulator > _sectorUpdateRate)
			{
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "### mAccumulator >= _sectorUpdateRate", false);

				mAccumulator -= _sectorUpdateRate;

				float scalar = mAccumulator / _sectorUpdateRate;

				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mInterpolatedPosition before :x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);
				
				btQuaternion rotationAtTick = shipSateFromTick.mWorldTransform.getRotation();
				btVector3& positionAtTick = shipSateFromTick.mWorldTransform.getOrigin();
				
				mInterpolatedRotation = mInterpolatedRotation.slerp(rotationAtTick, scalar);
				mInterpolatedPosition = mInterpolatedPosition.lerp(positionAtTick, scalar);
				
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mInterpolatedPosition after :x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);
			}
			else
			{
				btQuaternion rotationAtTick = shipSateFromTick.mWorldTransform.getRotation();
				btVector3& positionAtTick = shipSateFromTick.mWorldTransform.getOrigin();

				mInterpolatedRotation = shipSateFromTickminusOne.mWorldTransform.getRotation();
				mInterpolatedPosition = shipSateFromTickminusOne.mWorldTransform.getOrigin();

				float scalar = mAccumulator / _sectorUpdateRate;

				mInterpolatedRotation = mInterpolatedRotation.slerp(rotationAtTick, scalar);
				mInterpolatedPosition = mInterpolatedPosition.lerp(positionAtTick, scalar);

				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "scalar:" + StringUtils::toStr(scalar), false);
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mInterpolatedPosition after :x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);
			}
		}
	}

	Ogre::Quaternion orientation = convert(mInterpolatedRotation);
	Ogre::Vector3 position = convert(mInterpolatedPosition);

	mSceneNode->setOrientation(orientation);
	mSceneNode->setPosition(position);
}

void Ship::setState(const ShipState& _shipState)
{
	//Rigid body
	mRigidBody->setWorldTransform(_shipState.mWorldTransform);
	mRigidBody->setLinearVelocity(_shipState.mLinearVelocity);
	mRigidBody->setAngularVelocity(_shipState.mAngularVelocity);
	mRigidBody->applyCentralForce(_shipState.mTotalForce);
	mRigidBody->applyTorque(_shipState.mTotalTorque);
	
	//Systems
	mCurrentRollForce = _shipState.mCurrentRollForce;
	mCurrentYawForce = _shipState.mCurrentYawForce;
	mCurrentPitchForce = _shipState.mCurrentPitchForce;
	
	mEngine.mWantedThrust = _shipState.mEngineWantedThrust;
	mEngine.mRealThrust = _shipState.mEngineRealThrust;
	
	//Hardpoints
	const std::vector<std::pair<int, float> >& harpointsState = _shipState.mHarpointsState;
	size_t hardpointsSize = harpointsState.size();
	for (size_t i = 0; i < hardpointsSize; ++i)
	{
		int index = harpointsState[i].first;
		if(mHardPoints[index].isUsed())
		{
			mHardPoints[index].getWeapon().mElapsedTime = harpointsState[i].second;
		}
	}
}

void Ship::serialize(RakNet::BitStream& _bitStream) const
{
	//Unique id
	_bitStream.Write(mUniqueId);

	//State
	std::vector<std::pair<int, float> > hardpointsState;
	int hardpointsSize = mHardPoints.size();
	for (int i = 0; i < hardpointsSize; ++i)
	{
		if (mHardPoints[i].isUsed())
		{
			hardpointsState.push_back(std::make_pair(mHardPoints[i].getIndex(), mHardPoints[i].getWeapon().mElapsedTime));
		}
	}

	ShipState shipState(mRigidBody, mCurrentRollForce, mCurrentYawForce, mCurrentPitchForce, mEngine.mWantedThrust, mEngine.mRealThrust, hardpointsState);
	shipState.serialize(_bitStream);
}