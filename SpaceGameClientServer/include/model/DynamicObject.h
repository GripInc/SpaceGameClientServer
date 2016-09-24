#ifndef _DYNAMIC_OBJECT_H_
#define _DYNAMIC_OBJECT_H_

#include "model/StaticObject.h"

#include "model/deserialized/DynamicObjectSettings.h"
#include "manager/StateManager.h"

class DynamicObject : public StaticObject
{
public:
	//static const float MAX_ACCEPTABLE_VECTOR_3D_DELTA;
	//static const float MAX_ACCEPTABLE_QUATERNION_DELTA;

	DynamicObject() {}
	DynamicObject(const DynamicObjectSettings* _dynamicObjectSettings, Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld, UniqueId _uniqueId) : StaticObject(_dynamicObjectSettings, _sceneManager, _dynamicWorld), mUniqueId(_uniqueId) {}

	virtual void destroy();

	virtual btVector3 getInertia() const
	{
		const DynamicObjectSettings* dynamicObjectSettings = static_cast<const DynamicObjectSettings*>(mObjectSettings);
		return dynamicObjectSettings->mLocalInertia;
	}

	//States management
	virtual void saveState(SectorTick _tick);
	//When set a state, all oldest states are discarded
	virtual void setState(SectorTick _tick);
	//Override a state for a futur rewind to this state
	virtual void overrideState(SectorTick _tick, const DynamicObjectState& _dynamicObjectState);

	//Return true if the check passed without need to fix
	//False if a fix was needed and done
	//virtual bool checkAndFixState(const DynamicObjectState& _state);

protected:
	virtual void instantiateCollisionObject();

	UniqueId mUniqueId = 0;

	//Return true if comparison was acceptable
	bool compareStates(const DynamicObjectState& _state1, const DynamicObjectState& _state2) const;

private:
	StateManager<DynamicObjectState> mStateManager;
};

#endif //_DYNAMIC_OBJECT_H_