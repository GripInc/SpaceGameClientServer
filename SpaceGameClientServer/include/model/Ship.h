#ifndef _SHIP_H_
#define _SHIP_H_

#include "model/Engine.h"
#include "model/Directional.h"
#include "model/DynamicObject.h"
#include "model/deserialized/ShipsSettings.h"
#include "model/InputState.h"
#include "model/HardPoint.h"

#include "manager/StateManager.h"

class btRigidBody;
class EngineSettings;
class DirectionalSettings;
class WeaponSettings;

class Ship : public DynamicObject
{
public:
	static const float epsilon;

	//Init model for in station
	void initModel(const ShipSettings* _shipSettings);

	//Init for in space future instanciation
	void init(Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld, UniqueId _uniqueId);

	//Instanciate in world
	virtual void instantiateObject() override;

	void attachCamera(Ogre::SceneNode* _cameraSceneNode);

	void addEngine(const EngineSettings* _engine);
	void addDirectional(const DirectionalSettings* _directional);
	void addWeapon(const WeaponSettings* _weapon, int _index);
	void removeWeapon(int _index);
	
	///Getters
	float getMaxYawRate() const { return mMaxYawRate; }
	float getMaxPitchRate() const { return mMaxPitchRate; }
	float getMaxRollRate() const { return mMaxRollRate; }
	virtual btVector3 getInertia() const
	{
		const ShipSettings* shipSettings = static_cast<const ShipSettings*>(mObjectSettings);
		return shipSettings->mLocalInertia * mDirectional.getInertiaMultiplier();
	}

	btAlignedObjectArray<HardPoint>& getHardPoints() { return mHardPoints; }

	Engine& getEngine() { return mEngine; }
	Directional& getDirectional() { return mDirectional; }
	
	float mCurrentRollForce = 0.f;
	float mCurrentYawForce = 0.f;
	float mCurrentPitchForce = 0.f;

	float mEnginePotentialForce = 0.f;

	void updateForces();
	void updateSystems(const InputState& _input, float _deltaTime, std::list<ShotSettings>& _outputShots);
	
	void destroy();

	UniqueId getUniqueId() const { return mUniqueId; }

	//DEBUG
	const btVector3& getLinearVelocity() const;
	const btRigidBody* getRigidBody() { return mRigidBody; }
	float mDebugValue = 0.f;
	float mEngineRealForce = 0.f;

	//States management
	virtual void setState(const ShipState& _shipState);

	//Update view with interpolation (used by client)
	void updateView(SectorTick _sectorTick, float _elapsedTime, float _sectorUpdateRate, const StateManager& _stateManager);
	//Update view using current model (used by server)
	void updateView();

	void fillState(ShipState& _shipState) const;

protected:
	///Ship properties from XML
	float mMaxYawRate = 0.f;
	float mMaxPitchRate = 0.f;
	float mMaxRollRate = 0.f;
	unsigned int mCargoSpace = 0U;

	virtual void instantiateObjectParts();

	btAlignedObjectArray<HardPoint> mHardPoints;

	//View stuff
	btVector3 mInterpolatedPosition;
	btQuaternion mInterpolatedRotation;
	SectorTick mLastTickViewed = 0;
	float mAccumulator = 0.f;
	float mAccumulatorRest = 0.f;

	///Engine
	Engine mEngine;
	///Directional system
	Directional mDirectional;

	//Update hardpoints elapsed time
	void updateHardPoints(float _deltaTime);
};

#endif //_SHIP_H_
