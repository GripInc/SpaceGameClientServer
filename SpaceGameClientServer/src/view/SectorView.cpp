#include "view/SectorView.h"

#include "model/deserialized/SectorsSettings.h"
#include "model/GameSettings.h"

#include "view/PlayerCamera.h"

#include "OgreSceneManager.h"

namespace
{
	const char* SECTOR_MAIN_LIGHT_NAME = "MainLight";
	const char* SECTOR_PLAYER_CAMERA_NAME = "SectorPlayerCamera";
}

void SectorView::createView(const std::string& _sectorName)
{
	const SectorSettings* sectorSettings = GameSettings::getInstance().getSector(_sectorName);

	//Clear scene (not the camera)
	mSceneManager->clearScene();

	//Create camera (handles remove of previous)
	PlayerCamera::getInstance().createCamera(SECTOR_PLAYER_CAMERA_NAME);

	// Set the scene's ambient light
	mSceneManager->setAmbientLight(Ogre::ColourValue(0.05f, 0.05f, 0.05f));

	// Create a Light and set its position
	Ogre::Light* light = mSceneManager->createLight(SECTOR_MAIN_LIGHT_NAME);
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(sectorSettings->mLightVectorX, sectorSettings->mLightVectorY, sectorSettings->mLightVectorZ));

	//TODO set light from _sector

	//Sky
	//TODO get it from _sector
	mSceneManager->setSkyBox(true, "test/StormySkyBox");
}

SectorView::~SectorView()
{
	mSceneManager->clearScene();
}