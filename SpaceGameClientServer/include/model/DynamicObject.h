#ifndef _DYNAMIC_OBJECT_H_
#define _DYNAMIC_OBJECT_H_

#include "model/StaticObject.h"

#include "model/deserialized/DynamicObjectSettings.h"
#include "manager/StateManager.h"

class DynamicObject : public StaticObject
{
public:

	void init(const DynamicObjectSettings* _dynamicObjectSettings, Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld, UniqueId _uniqueId)
	{
		StaticObject::init(_dynamicObjectSettings, _sceneManager, _dynamicWorld);

		mUniqueId = _uniqueId;
	}

	virtual void destroy();

	virtual btVector3 getInertia() const
	{
		const DynamicObjectSettings* dynamicObjectSettings = static_cast<const DynamicObjectSettings*>(mObjectSettings);
		return dynamicObjectSettings->mLocalInertia;
	}

	virtual void updateView(SectorTick _sectorTick);

	//States management
	virtual void saveState(SectorTick _tick);

protected:
	virtual void instantiateCollisionObject();

	UniqueId mUniqueId = 0;

private:
	StateManager<DynamicObjectState> mStateManager;
};

#endif //_DYNAMIC_OBJECT_H_