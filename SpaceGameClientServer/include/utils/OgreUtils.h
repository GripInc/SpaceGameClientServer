#ifndef _OGRE_UTILS_H_
#define _OGRE_UTILS_H_

#include "OgreSceneManager.h"
#include "OgreVector3.h"
#include "OgreLight.h"

#include <string>

namespace Ogre
{
	class Quaternion;
	class SceneNode;
}

class OgreUtils
{
public:
	static enum Ogre::SceneManager::PrefabType primitiveNameToPrefabEnum(const std::string& _name);
	
	static enum Ogre::Light::LightTypes lightTypeNameToTypeEnum(const std::string& _name);

	static void getMeshInformation(const Ogre::Mesh* const mesh, size_t& vertex_count, Ogre::Vector3* &vertices, size_t &index_count, unsigned long* &indices, const Ogre::Vector3& position, const Ogre::Quaternion& orient, const Ogre::Vector3& scale);

	static Ogre::Vector3 multiply(const Ogre::Quaternion& quat, const Ogre::Vector3& vec);

	static void createSphere(Ogre::SceneManager& sceneManager, const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16);

	static void destroySceneNode(Ogre::SceneNode* node);

	static void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
};

#endif //_OGRE_UTILS_H_