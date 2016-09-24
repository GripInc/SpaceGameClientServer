#ifndef _MY_MOTION_STATE_H
#define _MY_MOTION_STATE_H

#include "LinearMath/btDefaultMotionState.h"

namespace Ogre
{
	class SceneNode;
}

//Link of Ogre sceneNode and bullet transform (that is quite the same)
class MyMotionState : public btDefaultMotionState
{
public:
    MyMotionState(Ogre::SceneNode* _sceneNode, const btTransform& _startTransform = btTransform::getIdentity(), const btTransform& _centerOfMassOffset = btTransform::getIdentity());

    virtual ~MyMotionState() {}

    virtual void setWorldTransform(const btTransform& _centerOfMassWorldTransform);
 
protected:
    Ogre::SceneNode* mSceneNode = nullptr;
};

#endif //_MY_MOTION_STATE_H