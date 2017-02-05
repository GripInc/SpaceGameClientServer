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
	
	Sector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate);
	~Sector();

	//Init sector and static objects
	void instantiateObjects();

	//Getters
	std::vector<StaticObject>& getStaticObjects() { return mStaticObjects;}
	std::vector<DynamicObject>& getDynamicObjects() { return mDynamicObjects;}
	std::vector<Shot>& getShots() { return mShots;}
	std::vector<PlanetObject>& getPlanetObjects() { return mPlanetObjects;}
	
	//Add dynamic objects and instantiate them
	void addShotObjects(const std::list<ShotSettings>& _shots);

	//Set static objects visible
	void setStaticObjectsVisible(bool _value);

	//Debug view utils
	void switchDisplayDebug();
	void switchDisplay();

protected:
	///The Ogre scene manager
	Ogre::SceneManager* mSceneManager = nullptr;

	///The sector settings
	const SectorSettings* mSectorSettings = nullptr;

	//Sector objects
	std::vector<StaticObject> mStaticObjects;
	std::vector<DynamicObject> mDynamicObjects;
	std::vector<Shot> mShots;
	std::vector<PlanetObject> mPlanetObjects;
	std::vector<SectorObject> mGateObjects;
	std::map<RakNet::RakNetGUID, Ship*> mShips;

	//The physic world
	btDiscreteDynamicsWorld* mDynamicWorld = nullptr;
	//Physic simulation related
	btDbvtBroadphase* mBroadphase = nullptr;
    btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
    btCollisionDispatcher* mDispatcher = nullptr;
	btSequentialImpulseConstraintSolver* mConstraintSolver = nullptr;

	//Update rate
	float mSectorUpdateRate = 0.f;

	//Save all entities states (not the sector himself)
	void saveSectorState(SectorTick _tick);

	//Debug view utils
	BulletDebugDraw* mBulletDebugDraw = nullptr;
	bool mDisplayDebug = false;
	bool mDoDisplayWorld = true;
};

#endif //_SECTOR_H_