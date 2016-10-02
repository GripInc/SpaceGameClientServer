#include "model/Shot.h"
#include "model/deserialized/ShotsSettings.h"

void Shot::instantiateObject()
{
	instantiateObjectSceneNode(mObjectSettings->mInitialOrientation, mObjectSettings->mInitialPosition, mObjectSettings->mInitialScale, mObjectSettings->mMesh, mObjectSettings->mName);

	const ShotSettings* shotSettings =  static_cast<const ShotSettings*>(mObjectSettings);

	mSpeed = Ogre::Vector3(0.f, 0.f, -shotSettings->mSpeed);
	mSpeed = shotSettings->mInitialOrientation * mSpeed;

	mLifeTime = shotSettings->mLifeTime;
}

Shot::~Shot()
{
	SectorObject::~SectorObject();
}