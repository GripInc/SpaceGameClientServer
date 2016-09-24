#include "view/PlayerCamera.h"

#include "OgreCamera.h"
#include "OgreViewPort.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreRenderWindow.h"

///Singleton
PlayerCamera* PlayerCamera::mInstance;

///Singleton
PlayerCamera& PlayerCamera::getInstance()
{
	if(!mInstance)
		mInstance = new PlayerCamera();

	return *mInstance;
}

void PlayerCamera::init(Ogre::SceneManager* _sceneManager, Ogre::RenderWindow* _renderWindow)
{
	//TODO check assert
	//btAssert(!mSceneManager ^ mRenderWindow);

	if(mSceneManager)
	{
		//TODO log already initated
		return;
	}

	mSceneManager = _sceneManager;
	mRenderWindow = _renderWindow;
}

void PlayerCamera::createCamera(const std::string& _name)
{
	if(!mSceneManager)
	{
		//TODO not initiated ducon
		return;
	}

	//Destruct any existing camera or camera node first
	//Node with name == "" means scene has been cleared
	if(mCameraNode && mCameraNode->getName() != "" && mSceneManager->hasSceneNode(mCameraNode->getName()))
		mSceneManager->destroySceneNode(mCameraNode);

	mSceneManager->destroyAllCameras();

	mCamera = mSceneManager->createCamera(_name);
	
	//Setup camera
    mCamera->setNearClipDistance(5);
	mCamera->setFarClipDistance(0);

	//Camera node
	mCameraNode = mSceneManager->createSceneNode(_name);
	//mCameraNode = mSceneManager->getRootSceneNode()->createChildSceneNode(_name);
	mCameraNode->attachObject(mCamera);
	//Default start position and orientation
	mCameraNode->setPosition(Ogre::Vector3(0,0,0));
	mCameraNode->lookAt(Ogre::Vector3(0,0,-1), Ogre::Node::TS_WORLD);

	//Remove camera previous viewport if any
	mRenderWindow->removeAllViewports();

	// Create one viewport, entire window
	Ogre::Viewport* viewport = mRenderWindow->addViewport(mCamera);
    viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
}