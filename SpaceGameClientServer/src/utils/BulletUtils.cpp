#include "utils/BulletUtils.h"

#include "BulletDynamics/btBulletDynamicsCommon.h"
#include "BulletCollision/btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "factories/TriangleMeshFactory.h"

#include "OgreMeshManager.h"

#include "model/ObjectPart.h"
#include "model/StaticObject.h"

//Custom overlapping filtering callback
//Allow parts of the ship to collide with parts of other ship but don't test collision with other part of its own ship
/*struct ShipPartsFilterCallback : public btOverlapFilterCallback
{
// return true when pairs need collision
virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
//Basic filter and mask filtering
bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
collides &= proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask;

if(collides)
{
//Additional custom filtering
//btCollisionObject* collisionObject0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
//StaticObject* staticObject0 = static_cast<StaticObject*>(collisionObject0->getUserPointer());
//btCollisionObject* collisionObject1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);
//StaticObject* staticObject1 = static_cast<StaticObject*>(collisionObject1->getUserPointer());

//Same id mean part of the same object
//collides &= staticObject0->getUniqueId() != staticObject1->getUniqueId();
}

return collides;
}
};*/

void MyTickCallback(btDynamicsWorld* _world, btScalar _timeStep)
{
	btDispatcher* dispatcher = (btDispatcher*)_world->getWorldUserInfo();

	//Collisions damages 
	int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);

		if (contactManifold->getNumContacts() > 0)
		{
			const btRigidBody* object0 = static_cast<const btRigidBody*>(contactManifold->getBody0());
			const btRigidBody* object1 = static_cast<const btRigidBody*>(contactManifold->getBody1());

			//We deal with compound shapes only
			const btCompoundShape* compound0 = static_cast<const btCompoundShape*>(contactManifold->getBody0()->getCollisionShape());
			const btCompoundShape* compound1 = static_cast<const btCompoundShape*>(contactManifold->getBody1()->getCollisionShape());

			if (compound0 && compound1 && compound0->getShapeType() == COMPOUND_SHAPE_PROXYTYPE && compound1->getShapeType() == COMPOUND_SHAPE_PROXYTYPE && contactManifold->getNumContacts() > 0)
			{
				const btCollisionShape* childShape0 = compound0->getChildShape(object0->getUserIndex());
				const btCollisionShape* childShape1 = compound1->getChildShape(object1->getUserIndex());

				ObjectPart* objectPart0 = (ObjectPart*)childShape0->getUserPointer();
				ObjectPart* objectPart1 = (ObjectPart*)childShape1->getUserPointer();

				for (int i = 0; i < contactManifold->getNumContacts(); ++i)
				{
					float damage = contactManifold->getContactPoint(i).getAppliedImpulse();
					if (damage > 1.f)
					{
						objectPart0->mHitPoints -= (int)contactManifold->getContactPoint(i).getAppliedImpulse();
						objectPart1->mHitPoints -= (int)contactManifold->getContactPoint(i).getAppliedImpulse();
					}
				}
			}
		}
	}
}

//Use mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK); to trigger this callback on a rigidBody
bool MyContactCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
	btCompoundShape* compound0 = (btCompoundShape*)colObj0Wrap->getCollisionObject()->getCollisionShape();
	btCompoundShape* compound1 = (btCompoundShape*)colObj1Wrap->getCollisionObject()->getCollisionShape();

	StaticObject* staticObject0 = (StaticObject*)compound0->getUserPointer();
	StaticObject* staticObject1 = (StaticObject*)compound1->getUserPointer();

	staticObject0->getRigidBody()->setUserIndex(index0);
	staticObject1->getRigidBody()->setUserIndex(index1);

	return true;
}

btCollisionShape* createCollisionShape(const CollisionShapeSettings& _collisionShapeSettings, void* _userPointer)
{
	btCollisionShape* result;

	if (_collisionShapeSettings.mMesh != "")
	{
		btTriangleMesh* triangleMesh = TriangleMeshFactory::getInstance().getTriangleMesh(_collisionShapeSettings.mMesh);
		if (!triangleMesh)
			triangleMesh = TriangleMeshFactory::getInstance().addTriangleMesh(_collisionShapeSettings.mMesh, Ogre::MeshManager::getSingleton().load(_collisionShapeSettings.mMesh, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));

		if (_collisionShapeSettings.mNeedConvexHullShape)
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

	result->setUserPointer(_userPointer);
	return result;
}