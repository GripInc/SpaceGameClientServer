#ifndef _BULLET_DEBUG_DRAW_H_
#define _BULLET_DEBUG_DRAW_H_

#include "LinearMath/btIDebugDraw.h"

#include "OgreFrameListener.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"

namespace Ogre
{
	class SceneManager;
	class ManualObject;
}

class btDiscreteDynamicsWorld;

class BulletDebugDraw : public btIDebugDraw, public Ogre::FrameListener
{
public:
	BulletDebugDraw(Ogre::SceneManager* _sceneManager, btDiscreteDynamicsWorld* _dynamicWorld);
    ~BulletDebugDraw();

    virtual void drawLine (const btVector3& from, const btVector3& to, const btVector3& color);
    virtual void drawTriangle (const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar);
    virtual void drawContactPoint (const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
    virtual void reportErrorWarning (const char* warningString);
    virtual void draw3dText (const btVector3& location, const char* textString);
    virtual void setDebugMode (int debugMode);
    virtual int getDebugMode () const;

	/** to be called after the viewport is instantiated */
	void start();
	void stop();

protected:
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);

private:
    struct ContactPoint
	{
        Ogre::Vector3 from;
        Ogre::Vector3 to;
        Ogre::ColourValue color;
        size_t dieTime;
    };
    int mDebugModes;
	Ogre::ManualObject* mLines;    
    Ogre::ManualObject* mTriangles;
    std::list<ContactPoint> mContactPoints;
    /*std::vector<ContactPoint> mContactPoints1;
    std::vector<ContactPoint> mContactPoints2;*/

	//DEBUG
	Ogre::SceneManager* mSceneManager;
	btDiscreteDynamicsWorld* mCollisionWorld;

	//DEBUG
	int mSectionIndex;

	void initSections();

	//////
	bool mHasBeenStarted;

	Ogre::SceneNode* mDrawDebugSceneNode;
};

#endif //_BULLET_DEBUG_DRAW_H_