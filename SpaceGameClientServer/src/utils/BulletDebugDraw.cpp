#include "utils/BulletDebugDraw.h"
#include "utils/OgreBulletConvert.h"

#include "btBulletDynamicsCommon.h"

#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreRoot.h"
#include "WIN32/OgreTimerImp.h"
#include "OgreLogManager.h"

namespace
{
	std::string DEBUG_MATERIAL_NAME = "BaseWhiteNoLighting";
	std::string DRAW_DEBUG_SCENE_NODE_NAME = "mDrawDebugSceneNode";
}

BulletDebugDraw::BulletDebugDraw(Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _collisionWorld) 
	: mSectionIndex(0), 
	mSceneManager(_sceneManager), 
	mCollisionWorld(_collisionWorld), 
	mHasBeenStarted(false)
{
	mCollisionWorld->setDebugDrawer(this);
}
 
BulletDebugDraw::~BulletDebugDraw()
{
    Ogre::Root::getSingleton().removeFrameListener(this);
    delete mLines;
    delete mTriangles;
}

void BulletDebugDraw::start()
{
	if(!mHasBeenStarted)
	{
		if(!mSceneManager->hasSceneNode(DRAW_DEBUG_SCENE_NODE_NAME))
			mDrawDebugSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(DRAW_DEBUG_SCENE_NODE_NAME);

		if(!mSceneManager->hasManualObject("physicsLines"))
		{
			mLines = mSceneManager->createManualObject("physicsLines");
			mDrawDebugSceneNode->createChildSceneNode()->attachObject(mLines);
		}

		if(!mSceneManager->hasManualObject("physicsTriangles"))
		{
			mTriangles = mSceneManager->createManualObject("physicsTriangles");
			mDrawDebugSceneNode->createChildSceneNode()->attachObject(mTriangles);
		}

		mLines->setDynamic(true);
		mTriangles->setDynamic(true);

		mLines->estimateVertexCount(1000);
		mLines->estimateIndexCount(1);
		mTriangles->estimateVertexCount(10000);
		mTriangles->estimateIndexCount(1);
 
		initSections();
 
		Ogre::Root::getSingleton().addFrameListener(this);

		//We do this because input capture is called on frameRenderingQueued, so we reach frameEnded first
		mLines->beginUpdate(mSectionIndex);
		mTriangles->beginUpdate(mSectionIndex);

		mHasBeenStarted = true;
	}
	else
	{
		//TODO error msg
	}
}

void BulletDebugDraw::stop()
{
	if(mHasBeenStarted)
	{
		Ogre::Root::getSingleton().removeFrameListener(this);

		//We do this because input capture is called on frameRenderingQueued, so we never reach frameEnded here
		mLines->end();
		mTriangles->end();

		mLines->clear();
		mTriangles->clear();

		mHasBeenStarted = false;
	}
	else
	{
		//TODO error msg
	}
}

void BulletDebugDraw::initSections()
{
	mSectionIndex = 0;

	mLines->begin( DEBUG_MATERIAL_NAME, Ogre::RenderOperation::OT_LINE_LIST );
	mLines->position(Ogre::Vector3::ZERO);
    mLines->colour( Ogre::ColourValue::Blue);
    mLines->position(Ogre::Vector3::UNIT_SCALE);
	mLines->colour( Ogre::ColourValue::Blue );
	mLines->end();
	
	mTriangles->begin( DEBUG_MATERIAL_NAME, Ogre::RenderOperation::OT_TRIANGLE_LIST );
	mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour( Ogre::ColourValue::Blue );
    mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour( Ogre::ColourValue::Blue );
    mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour(Ogre::ColourValue::Blue );
	mTriangles->end();
}

void BulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	Ogre::ColourValue colorValue(color.getX(), color.getY(), color.getZ());  
	colorValue.saturate();
	mLines->position(convert(from));
	mLines->colour(colorValue);
	mLines->position(convert(to));
	mLines->colour(colorValue);
}
 
void BulletDebugDraw::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar alpha)
{
	Ogre::ColourValue colorValue( color.getX(), color.getY(), color.getZ(), alpha);  
	colorValue.saturate();
	mTriangles->position(convert(v0));
	mTriangles->colour(colorValue);
	mTriangles->position(convert(v1));
	mTriangles->colour(colorValue);
	mTriangles->position( convert(v2));
	mTriangles->colour(colorValue);
}
 
void BulletDebugDraw::drawContactPoint( const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color )
{
    ContactPoint p;
    p.from = convert( PointOnB );
    p.to = p.from + convert( normalOnB ) * distance;
    p.dieTime = Ogre::Root::getSingleton().getTimer()->getMilliseconds() + lifeTime;
    p.color.r = color.x();
    p.color.g = color.y();
    p.color.b = color.z();
	mContactPoints.push_back(p);
}
 
bool BulletDebugDraw::frameStarted( const Ogre::FrameEvent& evt )
{
    mLines->beginUpdate(mSectionIndex);
	mTriangles->beginUpdate(mSectionIndex);

	size_t now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
	while(mContactPoints.size() > 0 && mContactPoints.front().dieTime < now)
	{
		mContactPoints.pop_front();
	}
	for(std::list<ContactPoint>::iterator it = mContactPoints.begin(); it != mContactPoints.end(); ++it)
	{
		drawLine(convert((*it).from), convert((*it).to), convert(Ogre::Vector3((*it).color.r, (*it).color.g, (*it).color.b)));
	}

	if(mHasBeenStarted)
	{
		mCollisionWorld->debugDrawWorld();
	}

    return true;
}
 
bool BulletDebugDraw::frameEnded( const Ogre::FrameEvent& evt )
{
	mLines->end();
	mTriangles->end();

	return true;
}
 
void BulletDebugDraw::reportErrorWarning( const char *warningString )
{
    Ogre::LogManager::getSingleton().getDefaultLog()->logMessage( warningString );
}
 
void BulletDebugDraw::draw3dText( const btVector3 &location, const char *textString )
{
 
}
 
void BulletDebugDraw::setDebugMode( int debugMode )
{
    mDebugModes = debugMode;
}
 
int BulletDebugDraw::getDebugMode() const
{
    return mDebugModes;
}