#ifndef _TRIANGLE_MESH_FACTORY_H_
#define _TRIANGLE_MESH_FACTORY_H_

#include <string>
#include <map>

#include "ogreprerequisites.h"

class btTriangleMesh;

class TriangleMeshFactory
{
public:
	static TriangleMeshFactory& getInstance();

	btTriangleMesh* getTriangleMesh(const std::string& _name) const;
	btTriangleMesh* addTriangleMesh(const std::string& _name, Ogre::MeshPtr _fromMesh);

protected:
	static TriangleMeshFactory* mInstance;
	TriangleMeshFactory() {}

	std::map<std::string, btTriangleMesh*> mTriangleMeshes;
};

#endif //_TRIANGLE_MESH_FACTORY_H_