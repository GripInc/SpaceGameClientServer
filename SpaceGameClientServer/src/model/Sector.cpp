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
#include "manager/LoggerManager.h"

#include "OgreSceneNode.h"

#include "BitStream.h"

namespace
{
	const std::string LOG_CLASS_TAG = "Sector";
}

UniqueId Sector::sUniqueId = -1;

//Epsilon
//Ogre::Real epsilon = std::numeric_limits<Ogre::Real>::epsilon();
const float Sector::epsilon = 0.001f;

//Custom overlapping filtering callback
//Allow parts of the ship to collide with parts of other ship but don't test collision with other part of its own ship
/*struct ShipPartsFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		//Basic filter and mask filtering
		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
		collides &= proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask;

		if(collides)
		{
			//Additional custom filtering
			//btCollisionObject* collisionObject0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
			//StaticObject* staticObject0 = static_cast<StaticObject*>(collisionObject0->getUserPointer());
			//btCollisionObject* collisionObject1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);
			//StaticObject* staticObject1 = static_cast<StaticObject*>(collisionObject1->getUserPointer());

			//Same id mean part of the same object
			//collides &= staticObject0->getUniqueId() != staticObject1->getUniqueId();
		}

		return collides;
	}
};*/

void MyTickCallback(btDynamicsWorld* _world, btScalar _timeStep)
{
	Sector* sector = (Sector*)_world->getWorldUserInfo();
	
	//Collisions damages 
	int numManifolds = sector->getDynamicWorldDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = sector->getDynamicWorldDispatcher()->getManifoldByIndexInternal(i);
		
		if(contactManifold->getNumContacts() > 0)
		{
			const btRigidBody* object0 = static_cast<const btRigidBody*>(contactManifold->getBody0());
			const btRigidBody* object1 = static_cast<const btRigidBody*>(contactManifold->getBody1());
			
			//We deal with compound shapes only
			const btCompoundShape* compound0 = static_cast<const btCompoundShape*>(contactManifold->getBody0()->getCollisionShape());
			const btCompoundShape* compound1 = static_cast<const btCompoundShape*>(contactManifold->getBody1()->getCollisionShape());
			
			if(compound0 && compound1 && compound0->getShapeType() == COMPOUND_SHAPE_PROXYTYPE && compound1->getShapeType() == COMPOUND_SHAPE_PROXYTYPE && contactManifold->getNumContacts() > 0)
			{
				const btCollisionShape* childShape0 = compound0->getChildShape(object0->getUserIndex());
				const btCollisionShape* childShape1 = compound1->getChildShape(object1->getUserIndex());

				ObjectPart* objectPart0 = (ObjectPart*)childShape0->getUserPointer();
				ObjectPart* objectPart1 = (ObjectPart*)childShape1->getUserPointer();

				for(int i = 0; i < contactManifold->getNumContacts(); ++i)
				{
					float damage = contactManifold->getContactPoint(i).getAppliedImpulse();
					if(damage > 1.f)
					{
						objectPart0->mHitPoints -= (int)contactManifold->getContactPoint(i).getAppliedImpulse();
						objectPart1->mHitPoints -= (int)contactManifold->getContactPoint(i).getAppliedImpulse();
					}
				}
			}
		}
	}
}

//Use mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK); to trigger this callback on a rigidBody
bool MyContactCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
	btCompoundShape* compound0 = (btCompoundShape*)colObj0Wrap->getCollisionObject()->getCollisionShape();
	btCompoundShape* compound1 = (btCompoundShape*)colObj1Wrap->getCollisionObject()->getCollisionShape();

	StaticObject* staticObject0 = (StaticObject*)compound0->getUserPointer();
	StaticObject* staticObject1 = (StaticObject*)compound1->getUserPointer();

	staticObject0->getRigidBody()->setUserIndex(index0);
	staticObject1->getRigidBody()->setUserIndex(index1);
	
	return true;
}

Sector::Sector(Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick)
	: mSectorTick(_startingSectorTick),
	mSceneManager(_sceneManager),
	mSectorUpdateRate(_sectorUpdateRate)
{
	//Create dynamic world
	mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mConstraintSolver = new btSequentialImpulseConstraintSolver();
 
	mDynamicWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mConstraintSolver, mCollisionConfiguration);
	mDynamicWorld->setGravity(btVector3(0,0,0));
	mDynamicWorld->setInternalTickCallback(MyTickCallback, this);

	/*ShipPartsFilterCallback* shipPartsFilterCallback = new ShipPartsFilterCallback();
	mDynamicWorld->getPairCache()->setOverlapFilterCallback(shipPartsFilterCallback);*/

	gContactAddedCallback = &MyContactCallback;

	//Init bullet debugger
	mBulletDebugDraw = new BulletDebugDraw(mSceneManager, mDynamicWorld);
	mBulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
	mDisplayDebug = false;
}

void Sector::instantiateObjects(const std::string& _sectorName)
{
	const SectorSettings* sectorSettings = GameSettings::getInstance().getSector(_sectorName);

	//Fill in objects
	if(sectorSettings)
	{
		//Static objects
		for(int i = 0; i < sectorSettings->mStaticObjects.size(); ++i)
		{
			mStaticObjects.push_back(new StaticObject(&sectorSettings->mStaticObjects[i], mSceneManager, mDynamicWorld));
			mStaticObjects.back()->instantiateObject();
		}

		//Planet objects
		for(int i = 0; i < sectorSettings->mPlanetObjects.size(); ++i)
		{
			mPlanetObjects.push_back(new PlanetObject(&sectorSettings->mPlanetObjects[i], mSceneManager));
			mPlanetObjects.back()->instantiateObject();
		}

		//Gate objects
		for(int i = 0; i < sectorSettings->mGateObjects.size(); ++i)
		{
			mGateObjects.push_back(new SectorObject(&sectorSettings->mGateObjects[i], mSceneManager));
			mGateObjects.back()->instantiateObject();
		}
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateObjects", "Sector settings not found for sector : " + _sectorName);
		assert(false);
	}
}

void Sector::addShotObject(const ShotSettings& _shotSettings)
{
	Shot* newShot = new Shot(&_shotSettings, mSceneManager);
	newShot->instantiateObject();
	mShots.push_back(newShot);
}

void Sector::instantiateShip(const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, RakNet::RakNetGUID _rakNetGUID)
{
	const ShipSettings* shipSettings = GameSettings::getInstance().getShip(_shipId);

	if(shipSettings)
	{
		Ship* newShip = new Ship();
		newShip->initShip(shipSettings);
		newShip->instantiateObject(mSceneManager, mDynamicWorld, getNextUniqueId());
		newShip->forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
		mShips[_rakNetGUID] = newShip;

		//Add neutral input
		mLastClientsInput[_rakNetGUID] = InputState();
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateShip", "No ship settings found for _shipId : " + _shipId);
		assert(false);
	}
}

void Sector::instantiatePlayerShip(Ship& _playerShip, const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID)
{
	mPlayerShip = &_playerShip;
	mPlayerShip->instantiateObject(mSceneManager, mDynamicWorld, _uniqueId);
	mPlayerShip->forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
	mShips[_rakNetGUID] = mPlayerShip;

	//Add neutral input
	mPlayerInputHistory[mSectorTick] = InputState();
}

void Sector::setStaticObjectsVisible(bool _value)
{
	for(size_t i = 0; i < mStaticObjects.size(); ++i)
	{
		mStaticObjects[i]->setVisible(_value);
	}
}

Sector::~Sector()
{
	for(size_t i = 0; i < mStaticObjects.size(); ++i)
	{
		mStaticObjects[i]->destroy();
	}

	for(size_t i = 0; i < mDynamicObjects.size(); ++i)
	{
		mDynamicObjects[i]->destroy();
	}

	for(size_t i = 0; i < mPlanetObjects.size(); ++i)
	{
		mPlanetObjects[i]->destroy();
	}

	for(size_t i = 0; i < mShots.size(); ++i)
	{
		mShots[i]->destroy();
	}

	for(std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->destroy();
	}

	mBulletDebugDraw->stop();
}

void Sector::updateSector(ShipInputHandler& _shipInputHandler)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	if(_shipInputHandler.getHasInputChanged())
		addPlayerInputInHistory(_shipInputHandler.mInputState);

	if(mDoNeedRewindData.mDoNeedRewindFlag)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Need rewind flag is up.", false);
		reSimulateWorldFromTick(mDoNeedRewindData.mLastTickReceived);
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Need rewind flag is down.", false);
		updateShipsSystems(mSectorUpdateRate, mSectorTick);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);
	}

	//Reset rewind flag
	mDoNeedRewindData.mDoNeedRewindFlag = false;
	mDoNeedRewindData.mLastTickReceived = 0;

	saveSectorState();
	
	//Send player inputs to server only in space mode and only if changed
	_shipInputHandler.sendInputToServer(mSectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "BEFORE Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);
	mSectorTick++;
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "AFTER Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	/////////////////////////////////
	//Old stuff

	//Update physics
	/*mTickAccumulator += _deltaTime;
	while( mTickAccumulator > WORLD_SIMULATION_RATE )
	{
		//updateShipsSystems(WORLD_SIMULATION_RATE);

		//TODO do it for every ship
		updateShipSystems(WORLD_SIMULATION_RATE, mPlayerShip);
		
		mDynamicWorld->stepSimulation(WORLD_SIMULATION_RATE, 10);

		saveSectorState();

		//Send player inputs to server only in space mode and only if changed
		_shipInputHandler.sendInputToServer(mSectorTick);

		mSectorTick++;
		mTickAccumulator -= WORLD_SIMULATION_RATE;
	}*/
}

void Sector::reSimulateWorldFromTick(SectorTick _tick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "reSimulateWorldFromTick", "START at tick " + StringUtils::toStr(_tick), false);

	//Set sector state to currentSectorTick
	setSectorState(_tick);

	while(_tick <= mSectorTick)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "reSimulateWorldFromTick", "ReSimulate tick " + StringUtils::toStr(_tick), false);

		updateShipsSystems(mSectorUpdateRate, _tick);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);

		_tick++;
	}
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

void Sector::updateShipsSystems(float _deltaTime, SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "", false);

	std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin();
	const std::map<RakNet::RakNetGUID, Ship*>::iterator shipItEnd = mShips.end();
	for(; shipIt != shipItEnd; ++shipIt)
	{
		Ship* clientShip = (*shipIt).second;
		InputState& clientInput = InputState();

		if(clientShip == mPlayerShip)
		{
			getPlayerInputAtTick(_sectorTick, clientInput);
		}
		else
		{
			RakNet::RakNetGUID clientId = (*shipIt).first;
			std::map<RakNet::RakNetGUID, InputState>::iterator foundClientInput = mLastClientsInput.find(clientId);
			if(foundClientInput != mLastClientsInput.end())
			{
				clientInput = (*foundClientInput).second;
			}
			else
			{
				LoggerManager::getInstance().logE(LOG_CLASS_TAG, "updateShipsSystems", "No input found for clientId : " + std::string(clientId.ToString()));
				assert(false);
			}
		}

		//What was that for??
		//if(!clientShip)
		//	continue;

		///////////////////
		//Handling thrust//
		///////////////////
		if(clientInput.mZKeyPressed)
			clientShip->getEngine().mWantedThrust += clientShip->getEngine().getThrustSensitivity() * _deltaTime;
		
		if(clientInput.mSKeyPressed)
			clientShip->getEngine().mWantedThrust -= clientShip->getEngine().getThrustSensitivity() * _deltaTime;
	
		if(clientInput.mAKeyPressed)
			clientShip->getEngine().mWantedThrust = clientShip->getEngine().getThrustMaxValue();
		
		if(clientInput.mQKeyPressed)
			clientShip->getEngine().mWantedThrust = 0;

		//Checking thrust bounds values
		if(clientShip->getEngine().mWantedThrust > clientShip->getEngine().getThrustMaxValue())
			clientShip->getEngine().mWantedThrust = clientShip->getEngine().getThrustMaxValue();
		if(clientShip->getEngine().mWantedThrust < 0)
			clientShip->getEngine().mWantedThrust = 0;

		//Adding or removing thrust
		float deltaThrust = clientShip->getEngine().mWantedThrust - clientShip->getEngine().mRealThrust;
		float thrustToAdd = 0.f;
		if(std::fabs(deltaThrust) < clientShip->getEngine().getReactivity())
		{
			thrustToAdd = (std::fabs(deltaThrust) / 2.f);
		}
		else
		{
			thrustToAdd = clientShip->getEngine().getReactivity();
		}

		if(deltaThrust > epsilon)
		{
			clientShip->getEngine().mRealThrust += thrustToAdd * _deltaTime;
		}
		else
		{
			clientShip->getEngine().mRealThrust = clientShip->getEngine().mWantedThrust;
		}
	
		///////////////////////////////////
		//Applying mouse movement on ship//
		///////////////////////////////////
		//Yaw
		if(!clientInput.mWKeyPressed && !clientInput.mXKeyPressed)
			clientShip->mCurrentYawForce = 0.f;
		else if(clientInput.mWKeyPressed)
			clientShip->mCurrentYawForce = -clientShip->getMaxYawRate() * clientShip->getDirectional().getTurnRateMultiplier();
		else
			clientShip->mCurrentYawForce = clientShip->getMaxYawRate() * clientShip->getDirectional().getTurnRateMultiplier();

		//Pitch
		if(clientInput.mMouseYAbs != 0.f)
		{
			//Mouse control
			clientShip->mCurrentPitchForce = clientInput.mMouseYAbs * clientShip->getMaxPitchRate() * clientShip->getDirectional().getTurnRateMultiplier();
		}
		else
		{
			//Keyboard control
			if(!clientInput.mUpKeyPressed && !clientInput.mDownKeyPressed)
				clientShip->mCurrentPitchForce = 0.f;
			else if(clientInput.mUpKeyPressed)
				clientShip->mCurrentPitchForce = -clientShip->getMaxPitchRate() * clientShip->getDirectional().getTurnRateMultiplier();
			else
				clientShip->mCurrentPitchForce = clientShip->getMaxPitchRate() * clientShip->getDirectional().getTurnRateMultiplier();
		}

		//Roll
		if(clientInput.mMouseXAbs != 0.f)
		{
			//Mouse control
			clientShip->mCurrentRollForce = clientInput.mMouseXAbs * clientShip->getMaxRollRate() * clientShip->getDirectional().getTurnRateMultiplier();
		}
		else
		{
			//Keyboard control
			if(!clientInput.mLeftKeyPressed && !clientInput.mRightKeyPressed)
				clientShip->mCurrentRollForce = 0.f;
			else if(clientInput.mLeftKeyPressed)
				clientShip->mCurrentRollForce = -clientShip->getMaxRollRate() * clientShip->getDirectional().getTurnRateMultiplier();
			else
				clientShip->mCurrentRollForce = clientShip->getMaxRollRate() * clientShip->getDirectional().getTurnRateMultiplier();
		}
	
		//Add shot
		if(clientInput.mFirePressed)
		{
			btAlignedObjectArray<HardPoint*> hardPoints = clientShip->getHardPoints();
			for(int i = 0; i < hardPoints.size(); ++i)
			{
				const WeaponSettings* weaponSettings = &hardPoints[i]->getWeaponSettings();
				if(hardPoints[i]->isUsed() && hardPoints[i]->mElapsedTime > weaponSettings->mFireRate)
				{
					hardPoints[i]->mElapsedTime = 0.f;
					ShotSettings shotSettings = hardPoints[i]->getShotSettings(); //Create a copy to be able to modify it
					shotSettings.mInitialOrientation = clientShip->getSceneNode()->getOrientation();
					shotSettings.mInitialPosition = clientShip->getRelativePosition(convert(hardPoints[i]->getWeaponSettings().mNoslePosition + hardPoints[i]->getPosition()));
					addShotObject(shotSettings);
				}
			}
		}

		clientShip->updateHardPoints(_deltaTime);

		/////////////////////////////////////////
		//Applying ship engine power and thrust//
		/////////////////////////////////////////
		clientShip->mEnginePotentialForce = -clientShip->getEngine().mRealThrust;
		clientShip->mEnginePotentialForce *= clientShip->getEngine().getPower();

		clientShip->updateForces();
	}
}

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

void Sector::receivedSectorState(RakNet::BitStream& _data)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "START", false);

	SectorTick sectorTick;
	_data.Read(sectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "sectorTick is : " + StringUtils::toStr(sectorTick), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "mDoNeedRewindData.mLastTickReceived is : " + StringUtils::toStr(mDoNeedRewindData.mLastTickReceived), false);

	//We just drop out of date sector dumps
	if(mDoNeedRewindData.mLastTickReceived >= sectorTick)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "Dropping state.", false);
		return;
	}
	else
	{
		mDoNeedRewindData.mLastTickReceived = sectorTick;
		mDoNeedRewindData.mDoNeedRewindFlag = true;
	}
	
	//Each client last input
	//mLastClientsInput.clear();
	//_data.Read(mLastClientsInput);

	//Each ship
	size_t shipsSize;
	_data.Read(shipsSize);

	//We write directly in ship history their state for tick mLastTickReceived
	RakNet::RakNetGUID rakNetId;
	for(size_t i = 0; i < shipsSize; ++i)
	{
		//Read client unique id
		_data.Read(rakNetId);

		//Read ship unique id
		UniqueId uniqueId;
		_data.Read(uniqueId);

		//Read ship state
		ShipState shipState;
		shipState.deserialize(_data);

		//Find the ship in sector
		std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundShip = mShips.find(rakNetId);
		if(foundShip != mShips.end())
		{
			Ship* ship = (*foundShip).second;
			ship->overrideSavedState(sectorTick, shipState);
		}
		else
		{
			LoggerManager::getInstance().logW(LOG_CLASS_TAG, "receivedSectorState", "No ship found for raknet id : " + std::string(rakNetId.ToString()));
		}
	}

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "END", false);
}

void Sector::saveSectorState()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "saveSectorState", "mSectorTick is " + StringUtils::toStr(mSectorTick), false);

	//Save all ships state
	const std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipsItEnd = mShips.end();
	for(std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipsIt = mShips.begin(); shipsIt != shipsItEnd; ++shipsIt)
	{
		(*shipsIt).second->saveState(mSectorTick);
	}
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

void Sector::addPlayerInputInHistory(const InputState& _inputState)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addPlayerInputInHistory", "mSectorTick is " + StringUtils::toStr(mSectorTick), false);

	mPlayerInputHistory[mSectorTick] = _inputState;

	//Check that we don't have too much history
	/*if((*mPlayerInputHistory.begin()).first < mSectorTick - (mMaxInputRewind * 2))
	{
		SectorTick oldestAllowedInput = mSectorTick - mMaxInputRewind;
		while((*mPlayerInputHistory.begin()).first < oldestAllowedInput)
			mPlayerInputHistory.erase(mPlayerInputHistory.begin());
	}*/
}

void Sector::getPlayerInputAtTick(SectorTick _tick, InputState& _inputState)
{
	//Get last valid player input from _tick
	std::map<SectorTick, InputState>::const_iterator foundInput = mPlayerInputHistory.find(_tick);
	if(foundInput != mPlayerInputHistory.end())
		_inputState = (*foundInput).second;
	else
	{
		std::map<SectorTick, InputState>::const_reverse_iterator clientsInputByTickIt(foundInput); //Converting it to reverse_it point to the next element in reverse view it = 5 -> reverseIt = 4
		if(clientsInputByTickIt != mPlayerInputHistory.rend())
			_inputState = (*clientsInputByTickIt).second;
		else
		{
			LoggerManager::getInstance().logE(LOG_CLASS_TAG, "getPlayerInputAtTick", "No valid player input found from _tick " + StringUtils::toStr(_tick));
			assert(false);
		}
	}
}