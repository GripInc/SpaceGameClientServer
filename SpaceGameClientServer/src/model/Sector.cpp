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
#include "utils/BulletCallBacks.h"

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
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateObjects", "Sector settings not found for sector : " + mSectorSettings->mName);
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

void Sector::saveSectorState()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "saveSectorState", "mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	for (std::pair<RakNet::RakNetGUID, Ship*> UIdShipPair : mShips)
	{
		UIdShipPair.second->saveState(mSectorTick);
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
	///////////////////
	//Handling thrust//
	///////////////////
	if (_input.mZKeyPressed)
		_ship->getEngine().mWantedThrust += _ship->getEngine().getThrustSensitivity() * _deltaTime;

	if (_input.mSKeyPressed)
		_ship->getEngine().mWantedThrust -= _ship->getEngine().getThrustSensitivity() * _deltaTime;

	if (_input.mAKeyPressed)
		_ship->getEngine().mWantedThrust = _ship->getEngine().getThrustMaxValue();

	if (_input.mQKeyPressed)
		_ship->getEngine().mWantedThrust = 0;

	//Checking thrust bounds values
	if (_ship->getEngine().mWantedThrust > _ship->getEngine().getThrustMaxValue())
		_ship->getEngine().mWantedThrust = _ship->getEngine().getThrustMaxValue();
	if (_ship->getEngine().mWantedThrust < 0)
		_ship->getEngine().mWantedThrust = 0;

	//Adding or removing thrust
	float deltaThrust = _ship->getEngine().mWantedThrust - _ship->getEngine().mRealThrust;
	float thrustToAdd = 0.f;
	if (std::fabs(deltaThrust) < _ship->getEngine().getReactivity())
	{
		thrustToAdd = (std::fabs(deltaThrust) / 2.f);
	}
	else
	{
		thrustToAdd = _ship->getEngine().getReactivity();
	}

	if (deltaThrust > epsilon)
	{
		_ship->getEngine().mRealThrust += thrustToAdd * _deltaTime;
	}
	else
	{
		_ship->getEngine().mRealThrust = _ship->getEngine().mWantedThrust;
	}

	///////////////////////////////////
	//Applying mouse movement on ship//
	///////////////////////////////////
	//Yaw
	if (!_input.mWKeyPressed && !_input.mXKeyPressed)
		_ship->mCurrentYawForce = 0.f;
	else if (_input.mWKeyPressed)
		_ship->mCurrentYawForce = -_ship->getMaxYawRate() * _ship->getDirectional().getTurnRateMultiplier();
	else
		_ship->mCurrentYawForce = _ship->getMaxYawRate() * _ship->getDirectional().getTurnRateMultiplier();

	//Pitch
	if (_input.mMouseYAbs != 0.f)
	{
		//Mouse control
		_ship->mCurrentPitchForce = _input.mMouseYAbs * _ship->getMaxPitchRate() * _ship->getDirectional().getTurnRateMultiplier();
	}
	else
	{
		//Keyboard control
		if (!_input.mUpKeyPressed && !_input.mDownKeyPressed)
			_ship->mCurrentPitchForce = 0.f;
		else if (_input.mUpKeyPressed)
			_ship->mCurrentPitchForce = -_ship->getMaxPitchRate() * _ship->getDirectional().getTurnRateMultiplier();
		else
			_ship->mCurrentPitchForce = _ship->getMaxPitchRate() * _ship->getDirectional().getTurnRateMultiplier();
	}

	//Roll
	if (_input.mMouseXAbs != 0.f)
	{
		//Mouse control
		_ship->mCurrentRollForce = _input.mMouseXAbs * _ship->getMaxRollRate() * _ship->getDirectional().getTurnRateMultiplier();
	}
	else
	{
		//Keyboard control
		if (!_input.mLeftKeyPressed && !_input.mRightKeyPressed)
			_ship->mCurrentRollForce = 0.f;
		else if (_input.mLeftKeyPressed)
			_ship->mCurrentRollForce = -_ship->getMaxRollRate() * _ship->getDirectional().getTurnRateMultiplier();
		else
			_ship->mCurrentRollForce = _ship->getMaxRollRate() * _ship->getDirectional().getTurnRateMultiplier();
	}

	//Add shot
	if (_input.mFirePressed)
	{
		btAlignedObjectArray<HardPoint>& hardPoints = _ship->getHardPoints();
		for (int i = 0; i < hardPoints.size(); ++i)
		{
			HardPoint& hardPoint = hardPoints[i];

			if (hardPoint.isUsed() && hardPoint.mElapsedTime > hardPoint.getWeaponSettings().mFireRate)
			{
				hardPoint.mElapsedTime = 0.f;
				ShotSettings shotSettings = hardPoint.getShotSettings(); //Create a copy to be able to modify it
				shotSettings.mInitialOrientation = _ship->getSceneNode()->getOrientation();
				shotSettings.mInitialPosition = _ship->getRelativePosition(convert(hardPoint.getWeaponSettings().mNoslePosition + hardPoint.getPosition()));
				addShotObject(shotSettings);
			}
		}
	}

	_ship->updateHardPoints(_deltaTime);

	/////////////////////////////////////////
	//Applying ship engine power and thrust//
	/////////////////////////////////////////
	_ship->mEnginePotentialForce = -_ship->getEngine().mRealThrust;
	_ship->mEnginePotentialForce *= _ship->getEngine().getPower();

	_ship->updateForces();
}