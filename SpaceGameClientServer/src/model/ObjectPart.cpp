#include "model/ObjectPart.h"

#include "model/GameSettings.h"

#include "factories/TriangleMeshFactory.h"

#include "utils/OgreBulletConvert.h"
#include <tinyxml2.h>
#include "utils/XMLHelper.h"
#include "utils/OgreUtils.h"

#include "OgreSceneNode.h"
#include "OgreMeshManager.h"

#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

void ObjectPart::init(const ObjectPartSettings& _objectPartSettings)
{
	mName = _objectPartSettings.mName;
	mHitPoints = _objectPartSettings.mHitPoints;
}

btCollisionShape* ObjectPart::createCollisionShape(const CollisionShapeSettings& _collisionShapeSettings)
{
	btCollisionShape* result;

	if(_collisionShapeSettings.mMesh != "")
	{
		btTriangleMesh* triangleMesh = TriangleMeshFactory::getInstance().getTriangleMesh(_collisionShapeSettings.mMesh);
		if(!triangleMesh)
			triangleMesh = TriangleMeshFactory::getInstance().addTriangleMesh(_collisionShapeSettings.mMesh, Ogre::MeshManager::getSingleton().load(_collisionShapeSettings.mMesh, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));

		if(_collisionShapeSettings.mNeedConvexHullShape)
		{
			btConvexShape* tmpConvexShape = new btConvexTriangleMeshShape(triangleMesh);
	
			//create a hull approximation
			btShapeHull* hull = new btShapeHull(tmpConvexShape);
			btScalar margin = tmpConvexShape->getMargin();
			hull->buildHull(margin);
			tmpConvexShape->setUserPointer(hull);
		
			btConvexHullShape* convexShape = new btConvexHullShape();

			for (int i = 0; i < hull->numVertices(); i++)
			{
				convexShape->addPoint(hull->getVertexPointer()[i], false);	
			}
			convexShape->recalcLocalAabb();

			convexShape->initializePolyhedralFeatures();

			delete tmpConvexShape;
			delete hull;

			result = convexShape;
		}
		else
		{
			result = new btConvexTriangleMeshShape(triangleMesh);
		}
	}
	else
	{
		result = new btBoxShape(_collisionShapeSettings.mInitialScale / 2.f); // /2.f because bullet use half the size
	}

	result->setUserPointer(this);
	return result;
}
