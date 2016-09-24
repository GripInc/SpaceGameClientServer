#include "model/MyMotionState.h"
#include "OgreSceneNode.h"
#include "utils/OgreBulletConvert.h"

MyMotionState::MyMotionState(Ogre::SceneNode* _sceneNode, const btTransform& _startTransform, const btTransform& _centerOfMassOffset)
	: mSceneNode(_sceneNode),
	btDefaultMotionState(_startTransform, _centerOfMassOffset)
{
}

void MyMotionState::setWorldTransform(const btTransform& _centerOfMassWorldTransform)
{
	btDefaultMotionState::setWorldTransform(_centerOfMassWorldTransform);

	Ogre::Quaternion orientation = convert(m_graphicsWorldTrans.getRotation());
	Ogre::Vector3 position = convert(m_graphicsWorldTrans.getOrigin());

	mSceneNode->setPosition(position);
	mSceneNode->setOrientation(orientation);
}