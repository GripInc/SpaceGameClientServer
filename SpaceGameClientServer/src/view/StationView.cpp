#include "view/StationView.h"

#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "view/PlayerCamera.h"

namespace
{
	const char* STATION_MAIN_LIGHT_NAME = "MainLight";
	const char* STATION_PLAYER_CAMERA_NAME = "StationPlayerCamera";
}

void StationView::init(Ogre::SceneManager* _sceneManager)
{
	mSceneManager = _sceneManager;
}

void StationView::createView(const ScreenSettings& _screen)
{
	//Clear scene (not the camera)
	mSceneManager->clearScene();

	//Create camera (handles remove of previous)
	PlayerCamera::getInstance().createCamera(STATION_PLAYER_CAMERA_NAME);
	mSceneManager->getRootSceneNode()->addChild(PlayerCamera::getInstance().getCameraNode());

	// Set the scene's ambient light
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.05f, 0.05f, 0.05f));

	// Create a Light and set its position
	Ogre::Light* light = mSceneManager->createLight(STATION_MAIN_LIGHT_NAME);
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	//light->setDirection(_screen.getLightDirection());

	//TODO set light from _sector

	//Sky
	//TODO get it from _sector
	mSceneManager->setSkyBox(true, "test/StormySkyBox");
}

StationView::~StationView()
{
	mSceneManager->clearScene();
}