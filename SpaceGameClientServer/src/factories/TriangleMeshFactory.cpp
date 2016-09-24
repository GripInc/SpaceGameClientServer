#include "factories/TriangleMeshFactory.h"

#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"

#include "OgreMeshManager.h"

#include "btBulletCollisionCommon.h"

///Singleton
TriangleMeshFactory* TriangleMeshFactory::mInstance;

TriangleMeshFactory& TriangleMeshFactory::getInstance()
{
	if(!mInstance)
		mInstance = new TriangleMeshFactory();
	
	return *mInstance;
}

btTriangleMesh* TriangleMeshFactory::getTriangleMesh(const std::string& _name) const
{
	std::map<std::string, btTriangleMesh*>::const_iterator found = mTriangleMeshes.find(_name);
	if(found != mTriangleMeshes.end())
		return (*found).second;
	else
		return NULL;
}

btTriangleMesh* TriangleMeshFactory::addTriangleMesh(const std::string& _name, Ogre::MeshPtr _fromMesh)
{
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	//Create complex shape
	unsigned int vertexCount, indexCount;
	Ogre::Vector3* vertices;
	unsigned long* indices;
		
	OgreUtils::getMeshInformation(mesh.get(), vertexCount, vertices, indexCount, indices, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, Ogre::Vector3(1.f, 1.f, 1.f)/* TODO _scale */);

	btTriangleMesh* triangleMesh = new btTriangleMesh();
	for(unsigned int i = 0; i < indexCount; i += 3)
	{
		triangleMesh->addTriangle(convert(vertices[indices[i+0]]), convert(vertices[indices[i+1]]), convert(vertices[indices[i+2]]));
	}

	mTriangleMeshes[_name] = triangleMesh;

	return triangleMesh;
}