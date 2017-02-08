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

//Epsilon
//Ogre::Real epsilon = std::numeric_limits<Ogre::Real>::epsilon();
const float Ship::epsilon = 0.001f;

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

//void Ship::saveState(SectorTick _tick)
//{
//	std::vector<std::pair<int, float> > hardpointsState;
//	int hardpointsSize = mHardPoints.size();
//	for (int i = 0; i < hardpointsSize; ++i)
//	{
//		if (mHardPoints[i].isUsed())
//		{
//			hardpointsState.push_back(std::make_pair(mHardPoints[i].getIndex(), mHardPoints[i].getWeapon().mElapsedTime));
//		}
//	}
//
//	mStateManager.saveState(_tick, ShipState(mRigidBody, mCurrentRollForce, mCurrentYawForce, mCurrentPitchForce, mEngine.mWantedThrust, mEngine.mRealThrust, hardpointsState));
//}

void Ship::updateView()
{
	Ogre::Quaternion orientation = convert(mRigidBody->getWorldTransform().getRotation());
	Ogre::Vector3 position = convert(mRigidBody->getWorldTransform().getOrigin());

	mSceneNode->setOrientation(orientation);
	mSceneNode->setPosition(position);
}

void Ship::updateView(SectorTick _sectorTick, float _elapsedTime, float _sectorUpdateRate, const StateManager& _stateManager)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "START at tick " + StringUtils::toStr(_sectorTick) + "; _elapsedTime is " + StringUtils::toStr(_elapsedTime) + "; _sectorUpdateRate is:" + StringUtils::toStr(_sectorUpdateRate), false);

	if (mLastTickViewed < _sectorTick)
	{
		mLastTickViewed = _sectorTick;
		mAccumulator = _elapsedTime;
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mLastTickViewed < _sectorTick : mLastTickViewed:" + StringUtils::toStr(mLastTickViewed) + "; mAccumulator:" + StringUtils::toStr(mAccumulator), false);
	}
	else
	{
		mAccumulator += _elapsedTime;
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mLastTickViewed <= _sectorTick : mAccumulator:" + StringUtils::toStr(mAccumulator), false);
	}

	ShipState shipSateFromTickMinusTwo, shipSateFromTickMinusOne, shipSateFromTick;
	_stateManager.getShipState(_sectorTick - 2, mUniqueId, shipSateFromTickMinusTwo);
	_stateManager.getShipState(_sectorTick - 1, mUniqueId, shipSateFromTickMinusOne);
	_stateManager.getShipState(_sectorTick, mUniqueId, shipSateFromTick);

	//Loosing time, no interpolation
	if (_elapsedTime >= _sectorUpdateRate)
	{
		mInterpolatedRotation = shipSateFromTick.mWorldTransform.getRotation();
		mInterpolatedPosition = shipSateFromTick.mWorldTransform.getOrigin();

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "_elapsedTime >= _sectorUpdateRate : taking _sectorTick position", false);
	}
	else
	{
		btQuaternion rotationAtTick;
		btVector3 positionAtTick;

		//Interpolate with rest of accumulator. We use former mInterpolatedRotation and mInterpolatedPosition
		//Should not happen more than once in a tick
		if (mAccumulator > _sectorUpdateRate)
		{
			mAccumulator -= _sectorUpdateRate;

			mInterpolatedRotation = shipSateFromTickMinusOne.mWorldTransform.getRotation();
			mInterpolatedPosition = shipSateFromTickMinusOne.mWorldTransform.getOrigin();

			rotationAtTick = shipSateFromTick.mWorldTransform.getRotation();
			positionAtTick = shipSateFromTick.mWorldTransform.getOrigin();

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mAccumulator > _sectorUpdateRate; mAccumulator is now " + StringUtils::toStr(mAccumulator), false);
		}
		else
		{
			//Here to interpolate we use tick minus one transform
			mInterpolatedRotation = shipSateFromTickMinusTwo.mWorldTransform.getRotation();
			mInterpolatedPosition = shipSateFromTickMinusTwo.mWorldTransform.getOrigin();

			rotationAtTick = shipSateFromTickMinusOne.mWorldTransform.getRotation();
			positionAtTick = shipSateFromTickMinusOne.mWorldTransform.getOrigin();

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mAccumulator <= _sectorUpdateRate", false);
		}

		float scalar = mAccumulator / _sectorUpdateRate;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "scalar:" + StringUtils::toStr(scalar), false);
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "before interpolation, mInterpolatedPosition.x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);

		mInterpolatedRotation = mInterpolatedRotation.slerp(rotationAtTick, scalar);
		mInterpolatedPosition = mInterpolatedPosition.lerp(positionAtTick, scalar);
	}

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "mInterpolatedPosition.x:" + StringUtils::toStr(mInterpolatedPosition.x()), false);

	Ogre::Quaternion orientation = convert(mInterpolatedRotation);
	Ogre::Vector3 position = convert(mInterpolatedPosition);

	mSceneNode->setOrientation(orientation);
	mSceneNode->setPosition(position);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateView", "END", false);
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

void Ship::updateSystems(const InputState& _input, float _deltaTime, std::list<ShotSettings>& _outputShots)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "START with input with tick " + StringUtils::toStr(_input.mTick), false);

	///////////////////
	//Handling thrust//
	///////////////////
	float& wantedThrust = mEngine.mWantedThrust;
	float& realThrust = mEngine.mRealThrust;
	const float thrustSensitivity = mEngine.getThrustSensitivity();
	const float thrustMaxValue = mEngine.getThrustMaxValue();
	const float reactivity = mEngine.getReactivity();

	if (_input.mZKeyPressed)
		wantedThrust += thrustSensitivity * _deltaTime;

	if (_input.mSKeyPressed)
		wantedThrust -= thrustSensitivity * _deltaTime;

	if (_input.mAKeyPressed)
		wantedThrust = thrustMaxValue;

	if (_input.mQKeyPressed)
		wantedThrust = 0;

	//Checking thrust bounds values
	if (wantedThrust > thrustMaxValue)
		wantedThrust = thrustMaxValue;
	if (wantedThrust < 0)
		wantedThrust = 0;

	//Adding or removing thrust
	float deltaThrust = wantedThrust - realThrust;
	float thrustToAdd = 0.f;
	if (std::fabs(deltaThrust) < reactivity)
	{
		thrustToAdd = (std::fabs(deltaThrust) / 2.f);
	}
	else
	{
		thrustToAdd = reactivity;
	}

	if (deltaThrust > epsilon)
	{
		realThrust += thrustToAdd * _deltaTime;
	}
	else
	{
		realThrust = wantedThrust;
	}

	///////////////////////////////////
	//Applying mouse movement on ship//
	///////////////////////////////////
	const float turnRateMultiplier = mDirectional.getTurnRateMultiplier();
	//Yaw
	float& currentYawForce = mCurrentYawForce;
	if (!_input.mWKeyPressed && !_input.mXKeyPressed)
		currentYawForce = 0.f;
	else if (_input.mWKeyPressed)
		currentYawForce = -mMaxYawRate * turnRateMultiplier;
	else
		currentYawForce = mMaxYawRate * turnRateMultiplier;

	//Pitch
	float& currentPitchForce = mCurrentPitchForce;
	if (_input.mMouseYAbs != 0.f)
	{
		//Mouse control
		currentPitchForce = _input.mMouseYAbs * mMaxPitchRate * turnRateMultiplier;
	}
	else
	{
		//Keyboard control
		if (!_input.mUpKeyPressed && !_input.mDownKeyPressed)
			currentPitchForce = 0.f;
		else if (_input.mUpKeyPressed)
			currentPitchForce = -mMaxPitchRate * turnRateMultiplier;
		else
			currentPitchForce = mMaxPitchRate * turnRateMultiplier;
	}

	//Roll
	float& currentRollForce = mCurrentRollForce;
	if (_input.mMouseXAbs != 0.f)
	{
		//Mouse control
		currentRollForce = _input.mMouseXAbs * mMaxRollRate * turnRateMultiplier;
	}
	else
	{
		//Keyboard control
		if (!_input.mLeftKeyPressed && !_input.mRightKeyPressed)
			currentRollForce = 0.f;
		else if (_input.mLeftKeyPressed)
			currentRollForce = -mMaxRollRate * turnRateMultiplier;
		else
			currentRollForce = mMaxRollRate * turnRateMultiplier;
	}

	//Add shot
	if (_input.mFirePressed)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "START HARDPOINT UPDATE _input.mFirePressed == true", false);

		for (int i = 0; i < mHardPoints.size(); ++i)
		{
			HardPoint& hardPoint = mHardPoints[i];
			Weapon& weapon = hardPoint.getWeapon();

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "hardPoint.getWeapon().mElapsedTime:" + StringUtils::toStr(weapon.mElapsedTime) + "; hardPoint.getWeapon().getFireRate():" + StringUtils::toStr(hardPoint.getWeapon().getFireRate()), false);

			if (hardPoint.isUsed() && weapon.mElapsedTime > weapon.getFireRate())
			{
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "Adding a shot", false);

				weapon.mElapsedTime = 0.f;
				_outputShots.push_back(*weapon.getShotSettings()); //Push back a copy, to modify initial transform
				ShotSettings& shotSettings = _outputShots.back();
				shotSettings.mInitialOrientation = convert(mRigidBody->getWorldTransform().getRotation());
				shotSettings.mInitialPosition = convert(getRelativePosition(weapon.getNoslePosition() + hardPoint.getPosition()));
			}
		}

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "END HARDPOINT UPDATE", false);
	}

	updateHardPoints(_deltaTime);

	/////////////////////////////////////////
	//Applying ship engine power and thrust//
	/////////////////////////////////////////
	mEnginePotentialForce = -mEngine.mRealThrust;
	mEnginePotentialForce *= mEngine.getPower();

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "END", false);
}

void Ship::fillState(ShipState& _shipState) const
{
	_shipState.mWorldTransform = mRigidBody->getWorldTransform();
	_shipState.mLinearVelocity = mRigidBody->getLinearVelocity();
	_shipState.mAngularVelocity = mRigidBody->getAngularVelocity();
	_shipState.mTotalForce = mRigidBody->getTotalForce();
	_shipState.mTotalTorque = mRigidBody->getTotalTorque();

	_shipState.mCurrentRollForce = mCurrentRollForce;
	_shipState.mCurrentYawForce = mCurrentYawForce;
	_shipState.mCurrentPitchForce = mCurrentPitchForce;
	_shipState.mEngineWantedThrust = mEngine.mWantedThrust;
	_shipState.mEngineRealThrust = mEngine.mRealThrust;
	_shipState.mHarpointsState.clear();

	std::vector<std::pair<int, float> > hardpointsState;
	int hardpointsSize = mHardPoints.size();
	for (int i = 0; i < hardpointsSize; ++i)
	{
		if (mHardPoints[i].isUsed())
			_shipState.mHarpointsState.push_back(std::make_pair(mHardPoints[i].getIndex(), mHardPoints[i].getWeapon().mElapsedTime));
	}

	_shipState.mUniqueId = mUniqueId;
}
