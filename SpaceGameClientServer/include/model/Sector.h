#ifndef _SECTOR_H_
#define _SECTOR_H_

#include "OgreVector3.h"

#include "model/StaticObject.h"
#include "model/PlanetObject.h"
#include "model/Shot.h"
#include "model/DynamicObject.h"
#include "model/Ship.h"

#include "btBulletDynamicsCommon.h"

#include <string>
#include <list>

#include "SpaceGameTypes.h"

class BulletDebugDraw;

class SectorSettings;
class ShipSettings;
class ShipInputHandler;

namespace Ogre
{
	class SceneManager;
}

namespace RakNet
{
	class BitStream;
}

class Sector
{
public:
	static const float epsilon;

	Sector(Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick);
	~Sector();

	//Init sector and static objects
	void instantiateObjects(const std::string& _sectorName);

	//Getters
	std::vector<StaticObject*>& getStaticObjects() { return mStaticObjects;}
	std::vector<DynamicObject*>& getDynamicObjects() { return mDynamicObjects;}
	std::vector<Shot*>& getShots() { return mShots;}
	std::vector<PlanetObject*>& getPlanetObjects() { return mPlanetObjects;}
	std::map<RakNet::RakNetGUID, Ship*>& getShips() { return mShips; }
	
	//Add dynamic objects and instantiate them
	void addShotObject(const ShotSettings& _shotSettings);
	void instantiateShip(const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, RakNet::RakNetGUID _rakNetGUID);
	void instantiatePlayerShip(Ship& _playerShip, const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID);

	//Set static objects visible
	void setStaticObjectsVisible(bool _value);

	//Update function
	void updateSector(ShipInputHandler& _shipInputHandler);

	//Collision physic related
	btDispatcher* getDynamicWorldDispatcher() const { return mDynamicWorld->getDispatcher(); }

	//Debug view utils
	void switchDisplayDebug();
	void switchDisplay();

	//For debug only
	const Ship* getPlayerShip() const { return mPlayerShip; }
	Ship* getPlayerShip() { return mPlayerShip; }

	//Unique id generator
	//Client regenrated unique ids are negative value, until server tells client wich unique it should use (positive one)
	UniqueId getNextUniqueId() const { return sUniqueId--; }

	void receivedSectorState(RakNet::BitStream& _data);

protected:
	static UniqueId sUniqueId;

	Ogre::SceneManager* mSceneManager = nullptr;

	//Sector objects
	Ship* mPlayerShip = nullptr; ///< Usefull to test special cases
	std::vector<StaticObject*> mStaticObjects;
	std::vector<DynamicObject*> mDynamicObjects;
	std::vector<Shot*> mShots;
	std::vector<PlanetObject*> mPlanetObjects;
	std::vector<SectorObject*> mGateObjects;
	std::map<RakNet::RakNetGUID, Ship*> mShips;

	//The physic world
	btDiscreteDynamicsWorld* mDynamicWorld = nullptr;
	//Physic simulation related
	btDbvtBroadphase* mBroadphase = nullptr;
    btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
    btCollisionDispatcher* mDispatcher = nullptr;
	btSequentialImpulseConstraintSolver* mConstraintSolver = nullptr;

	//Updates
	void updateShipsSystems(float _deltaTime, SectorTick _sectorTick);

	//Game tick
	SectorTick mSectorTick = 0;
	//Update rate
	float mSectorUpdateRate = 0.f;

	//Performs a rewind on server objects and resimulate
	void reSimulateWorldFromTick(SectorTick _tick);

	//Last sector dump tick received from server with flag for rewind or not
	struct DoNeedRewindData
	{
		SectorTick mLastTickReceived = 0;
		bool mDoNeedRewindFlag = false;
	} mDoNeedRewindData;

	//Set all sector's objects to the state they were at tick
	void setSectorState(SectorTick _tick);
	//Save all entities states (not the sector himself)
	void saveSectorState();

	//Input handling
	std::map<RakNet::RakNetGUID, InputState> mLastClientsInput;
	std::map<SectorTick, InputState> mPlayerInputHistory;
	void addPlayerInputInHistory(const InputState& _inputState);
	void getPlayerInputAtTick(SectorTick _tick, InputState& _inputState);
	unsigned int mMaxInputRewind = 0;

	//Debug view utils
	BulletDebugDraw* mBulletDebugDraw = nullptr;
	bool mDisplayDebug = false;
	bool mDoDisplayWorld = true;
};

#endif //_SECTOR_H_