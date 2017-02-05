#ifndef _SECTOR_OBJECT_H_
#define _SECTOR_OBJECT_H_

#include <string>

#include "OgreMath.h"
#include "OgreVector3.h"

#include "LinearMath/btTransform.h"

class SectorObjectSettings;

class SectorObject
{
public:

	void init(const SectorObjectSettings* _sectorObjectSettings, Ogre::SceneManager* _sceneManager)
	{
		mSceneManager = _sceneManager;
		mObjectSettings = _sectorObjectSettings;
	}

	virtual ~SectorObject();

	virtual void instantiateObject();
	
	Ogre::SceneNode* getSceneNode() { return mSceneNode; }
	const std::string& getName() const { return mName; }
	
	void setVisible(bool _value);

	virtual void destroy();

protected:
	//Ogre scene relatives objects
    Ogre::SceneNode* mSceneNode = nullptr;
	Ogre::SceneManager* mSceneManager = nullptr;
	
	const SectorObjectSettings* mObjectSettings = nullptr;

	std::string mName;

	bool mIsInstantiated = false;

	//Create entity from either mesh, or cube prefab
	virtual Ogre::Entity* createEntity(const std::string& _name, const std::string& _mesh, const Ogre::Vector3& _scale);

	void addSubSceneNode(const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, const Ogre::Vector3& _scale, const std::string& _mesh, const std::string& _name);

	void instantiateObjectSceneNode(const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, const Ogre::Vector3& _scale, const std::string& _mesh, const std::string& _name);
};

#endif //_SECTOR_OBJECT_H_