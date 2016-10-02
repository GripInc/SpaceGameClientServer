#include "model/SectorObject.h"

#include "model/deserialized/SectorObjectSettings.h"

#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreMesh.h"

#include "utils/StringUtils.h"
#include "utils/XMLHelper.h"
#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "SectorObject";
}

void SectorObject::instantiateObject()
{
	instantiateObjectSceneNode(mObjectSettings->mInitialOrientation, mObjectSettings->mInitialPosition, mObjectSettings->mInitialScale, mObjectSettings->mMesh, mObjectSettings->mName);
}

void SectorObject::instantiateObjectSceneNode(const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, const Ogre::Vector3& _scale, const std::string& _mesh, const std::string& _name)
{
	mName = _name;

	if(mIsInstantiated)
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateObjectSceneNode", "mIsInstantiated was not false.");
		assert(false);
	}

	//Create ogre scene node
	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();

	//Create ogre entity and attach entity to scene node
	mSceneNode->attachObject(createEntity(_name, _mesh, _scale));

	//Set up start pos and orientation
	mSceneNode->setPosition(_position);
	mSceneNode->setOrientation(_orientation);
	mSceneNode->setScale(_scale);

	mIsInstantiated = true;
}

Ogre::Entity* SectorObject::createEntity(const std::string& _name, const std::string& _mesh, const Ogre::Vector3& _scale)
{
	Ogre::Entity* newEntity = NULL;
	if(_mesh.empty())
	{
		newEntity = mSceneManager->createEntity(Ogre::SceneManager::PT_CUBE);

		//DEBUG
		//mEntity->setMaterialName("Examples/SphereMappedRustySteel");
		newEntity->setMaterialName("test/GrassFloor");
	}
	else
	{
		newEntity = mSceneManager->createEntity(_mesh);
	}

	return newEntity;
}

void SectorObject::addSubSceneNode(const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, const Ogre::Vector3& _scale, const std::string& _mesh, const std::string& _name)
{
	if(!mIsInstantiated)
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "addSubSceneNode", "mIsInstantiated is NULL.");
		assert(false);
	}

	//Create ogre scene node
	Ogre::SceneNode* newSceneNode = mSceneNode->createChildSceneNode();

	//Create ogre entity and attach entity to scene node
	newSceneNode->attachObject(createEntity(_name, _mesh, _scale));

	//Set up start pos and orientation
	newSceneNode->setPosition(_position);
	newSceneNode->setOrientation(_orientation);
	newSceneNode->setScale(_scale);
}

void SectorObject::destroy()
{
	OgreUtils::destroySceneNode(mSceneNode);

	mSceneNode = nullptr;
}

SectorObject::~SectorObject()
{
	
}

void SectorObject::setVisible(bool _value)
{ 
	mSceneNode->setVisible(_value);
}

Ogre::Vector3 SectorObject::getRelativePosition(const Ogre::Vector3& _originalPosition)
{
	return mSceneNode->getOrientation() * _originalPosition + mSceneNode->getPosition();
}