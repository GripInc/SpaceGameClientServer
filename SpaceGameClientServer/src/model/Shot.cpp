#include "model/Shot.h"
#include "model/deserialized/ShotsSettings.h"

void Shot::instantiateObject()
{
	instantiateObjectSceneNode(mObjectSettings->mInitialOrientation, mObjectSettings->mInitialPosition, mObjectSettings->mInitialScale, mObjectSettings->mMesh, mObjectSettings->getName());

	const ShotSettings* shotSettings =  static_cast<const ShotSettings*>(mObjectSettings);

	mSpeed = Ogre::Vector3(0.f, 0.f, -shotSettings->getSpeed());
	mSpeed = shotSettings->mInitialOrientation * mSpeed;

	mLifeTime = shotSettings->getLifeTime();
}

Shot::~Shot()
{
	SectorObject::~SectorObject();
}