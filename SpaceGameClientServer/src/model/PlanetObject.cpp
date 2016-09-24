#include "model/PlanetObject.h"

#include "model/GameSettings.h"

#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"

#include "utils/OgreUtils.h"
#include "utils/StringUtils.h"

const std::string PlanetObject::PLANET_SPHERE_MANUAL_OBJECT_ID = "PlanetSphere";
bool PlanetObject::sManualObjectIsReady = false;

Ogre::Entity* PlanetObject::createEntity(const std::string& _name, const std::string& _mesh, const Ogre::Vector3& _scale, Ogre::SceneManager* _sceneManager)
{
	Ogre::Entity* newEntity = NULL;

	if(!sManualObjectIsReady)
	{
		OgreUtils::createSphere(*_sceneManager, PLANET_SPHERE_MANUAL_OBJECT_ID, 1.f, 128, 128);
		sManualObjectIsReady = true;
	}
	
	newEntity = _sceneManager->createEntity(PLANET_SPHERE_MANUAL_OBJECT_ID);

	return newEntity;
}