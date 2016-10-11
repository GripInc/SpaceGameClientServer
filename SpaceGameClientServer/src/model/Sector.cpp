#include "model/Sector.h"

#include "model/StaticObject.h"
#include "model/DynamicObject.h"
#include "model/GameSettings.h"
#include "model/ObjectPart.h"
#include "model/HardPoint.h"

#include "controller/SectorController.h"
#include "controller/ShipInputHandler.h"

#include "network/NetworkService.h"

#include "utils/StringUtils.h"
#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"
#include "utils/BulletDebugDraw.h"
#include "utils/BulletUtils.h"

#include "manager/LoggerManager.h"

#include "OgreSceneNode.h"

#include "BitStream.h"

namespace
{
	const std::string LOG_CLASS_TAG = "Sector";
}

//Epsilon
//Ogre::Real epsilon = std::numeric_limits<Ogre::Real>::epsilon();
const float Sector::epsilon = 0.001f;

Sector::Sector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
	: mSceneManager(_sceneManager),
	mSectorUpdateRate(_sectorUpdateRate)
{
	mSectorSettings = GameSettings::getInstance().getSector(_sectorName);

	//Create dynamic world
	mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mConstraintSolver = new btSequentialImpulseConstraintSolver();
 
	mDynamicWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mConstraintSolver, mCollisionConfiguration);
	mDynamicWorld->setGravity(btVector3(0,0,0));
	mDynamicWorld->setInternalTickCallback(MyTickCallback, mDynamicWorld->getDispatcher());

	/*ShipPartsFilterCallback* shipPartsFilterCallback = new ShipPartsFilterCallback();
	mDynamicWorld->getPairCache()->setOverlapFilterCallback(shipPartsFilterCallback);*/

	gContactAddedCallback = &MyContactCallback;

	//Init bullet debugger
	mBulletDebugDraw = new BulletDebugDraw(mSceneManager, mDynamicWorld);
	mBulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
	mDisplayDebug = false;
}

void Sector::instantiateObjects()
{
	//Fill in objects
	if(mSectorSettings)
	{
		//Static objects
		mStaticObjects.resize(mSectorSettings->mStaticObjects.size());
		for(int i = 0; i < mSectorSettings->mStaticObjects.size(); ++i)
		{
			mStaticObjects[i].init(&mSectorSettings->mStaticObjects[i], mSceneManager, mDynamicWorld);
			mStaticObjects[i].instantiateObject();
		}

		//Planet objects
		mPlanetObjects.resize(mSectorSettings->mPlanetObjects.size());
		for(int i = 0; i < mSectorSettings->mPlanetObjects.size(); ++i)
		{
			mPlanetObjects[i].init(&mSectorSettings->mPlanetObjects[i], mSceneManager);
			mPlanetObjects[i].instantiateObject();
		}

		//Gate objects
		mGateObjects.resize(mSectorSettings->mGateObjects.size());
		for(int i = 0; i < mSectorSettings->mGateObjects.size(); ++i)
		{
			mGateObjects[i].init(&mSectorSettings->mGateObjects[i], mSceneManager);
			mGateObjects[i].instantiateObject();
		}
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateObjects", "Sector settings not found for sector : " + mSectorSettings->getName());
		assert(false);
	}
}

void Sector::addShotObject(const ShotSettings& _shotSettings)
{
	mShots.push_back(Shot());
	mShots.back().init(&_shotSettings, mSceneManager);
	mShots.back().instantiateObject();
}

void Sector::setStaticObjectsVisible(bool _value)
{
	for(size_t i = 0; i < mStaticObjects.size(); ++i)
	{
		mStaticObjects[i].setVisible(_value);
	}
}

Sector::~Sector()
{
	for(size_t i = 0; i < mStaticObjects.size(); ++i)
	{
		mStaticObjects[i].destroy();
	}

	for(size_t i = 0; i < mDynamicObjects.size(); ++i)
	{
		mDynamicObjects[i].destroy();
	}

	for(size_t i = 0; i < mPlanetObjects.size(); ++i)
	{
		mPlanetObjects[i].destroy();
	}

	for(size_t i = 0; i < mShots.size(); ++i)
	{
		mShots[i].destroy();
	}

	for(std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->destroy();
	}

	mBulletDebugDraw->stop();
}

/*void Sector::updateShots(float _deltaTime)
{
	Shot* shot;
	btVector3 oldPosition, newPosition;
	std::vector<Shot*>& shotsList = mCurrentSector->getShots();
	for(size_t i = 0; i < shotsList.size();)
	{
		shot = shotsList[i];

		//Update life period
		shotsList[i]->mTimeElapsed += _timeSinceLastFrame;
		//Auto remove after life period
		if(shotsList[i]->mTimeElapsed > shotsList[i]->getLifeTime())
		{
			shotsList[i]->destroy();
			delete shotsList[i];
			shotsList.erase(shotsList.begin() + i);
			continue;
		}

		oldPosition = convert(shot->getSceneNode()->getPosition());
		newPosition = convert(shot->getSceneNode()->getPosition()) + convert(shot->mSpeed * _timeSinceLastFrame);

		btDiscreteDynamicsWorld::ClosestRayResultCallback closestRayResultCallback(oldPosition, newPosition);
		mDynamicWorld->rayTest(oldPosition, newPosition, closestRayResultCallback);
		
		if(closestRayResultCallback.hasHit())
		{
			StaticObject* shotObject = (StaticObject*)closestRayResultCallback.m_collisionObject->getCollisionShape()->getUserPointer();
			//mLastShotTarget = shotObject->getName();
			shotsList[i]->destroy();
			delete shotsList[i];
			shotsList.erase(shotsList.begin() + i);
		}
		else
		{
			shot->getSceneNode()->setPosition(convert(newPosition));
			++i;
		}
	}
}*/

void Sector::switchDisplayDebug()
{
	mDisplayDebug = !mDisplayDebug;
	if(mDisplayDebug)
		mBulletDebugDraw->start();
	else
		mBulletDebugDraw->stop();
}

void Sector::switchDisplay()
{
	mDoDisplayWorld = !mDoDisplayWorld;
	setStaticObjectsVisible(mDoDisplayWorld);
}

void Sector::saveSectorState(SectorTick _tick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "saveSectorState", "_tick is : " + StringUtils::toStr(_tick), false);

	for (std::pair<RakNet::RakNetGUID, Ship*> UIdShipPair : mShips)
	{
		UIdShipPair.second->saveState(_tick);
	}

	//TODO other kind of entities
}

void Sector::setSectorState(SectorTick _tick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "setSectorState", "_tick is " + StringUtils::toStr(_tick), false);

	for(std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->setState(_tick);
	}

	//TODO other kind of entities
}

void Sector::updateShipSystems(const InputState& _input, Ship* _ship, float _deltaTime)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipSystems", "", false);

	Engine& shipEngine = _ship->getEngine();

	///////////////////
	//Handling thrust//
	///////////////////
	float& wantedThrust = shipEngine.mWantedThrust;
	float& realThrust = shipEngine.mRealThrust;
	const float thrustSensitivity = shipEngine.getThrustSensitivity();
	const float thrustMaxValue = shipEngine.getThrustMaxValue();
	const float reactivity = shipEngine.getReactivity();

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
	const float turnRateMultiplier = _ship->getDirectional().getTurnRateMultiplier();
	//Yaw
	float& currentYawForce = _ship->mCurrentYawForce;
	if (!_input.mWKeyPressed && !_input.mXKeyPressed)
		currentYawForce = 0.f;
	else if (_input.mWKeyPressed)
		currentYawForce = -_ship->getMaxYawRate() * turnRateMultiplier;
	else
		currentYawForce = _ship->getMaxYawRate() * turnRateMultiplier;

	//Pitch
	float& currentPitchForce = _ship->mCurrentPitchForce;
	const float maxPitchRate = _ship->getMaxPitchRate();
	if (_input.mMouseYAbs != 0.f)
	{
		//Mouse control
		currentPitchForce = _input.mMouseYAbs * maxPitchRate * turnRateMultiplier;
	}
	else
	{
		//Keyboard control
		if (!_input.mUpKeyPressed && !_input.mDownKeyPressed)
			currentPitchForce = 0.f;
		else if (_input.mUpKeyPressed)
			currentPitchForce = -maxPitchRate * turnRateMultiplier;
		else
			currentPitchForce = maxPitchRate * turnRateMultiplier;
	}

	//Roll
	float& currentRollForce = _ship->mCurrentRollForce;
	const float maxRollRate = _ship->getMaxRollRate();
	if (_input.mMouseXAbs != 0.f)
	{
		//Mouse control
		currentRollForce = _input.mMouseXAbs * maxRollRate * turnRateMultiplier;
	}
	else
	{
		//Keyboard control
		if (!_input.mLeftKeyPressed && !_input.mRightKeyPressed)
			currentRollForce = 0.f;
		else if (_input.mLeftKeyPressed)
			currentRollForce = -maxRollRate * turnRateMultiplier;
		else
			currentRollForce = maxRollRate * turnRateMultiplier;
	}

	//Add shot
	if (_input.mFirePressed)
	{
		btAlignedObjectArray<HardPoint>& hardPoints = _ship->getHardPoints();
		for (int i = 0; i < hardPoints.size(); ++i)
		{
			HardPoint& hardPoint = hardPoints[i];

			if (hardPoint.isUsed() && hardPoint.getWeapon().mElapsedTime > hardPoint.getWeapon().getFireRate())
			{
				hardPoint.getWeapon().mElapsedTime = 0.f;
				ShotSettings shotSettings = *hardPoint.getWeapon().getShotSettings(); //Create a copy to be able to modify it
				shotSettings.mInitialOrientation = _ship->getSceneNode()->getOrientation();
				shotSettings.mInitialPosition = _ship->getRelativePosition(convert(hardPoint.getWeapon().getNoslePosition() + hardPoint.getPosition()));
				addShotObject(shotSettings);
			}
		}
	}

	_ship->updateHardPoints(_deltaTime);

	/////////////////////////////////////////
	//Applying ship engine power and thrust//
	/////////////////////////////////////////
	_ship->mEnginePotentialForce = -shipEngine.mRealThrust;
	_ship->mEnginePotentialForce *= shipEngine.getPower();

	_ship->updateForces();
}