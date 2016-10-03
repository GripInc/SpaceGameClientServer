#include "controller/SectorController.h"

#include "manager/LoggerManager.h"
#include "view/SectorView.h"

void SectorController::createSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
{
	LoggerManager::getInstance().logI("SectorController", "createSector", "", false);

	// Create the Scene
	mSectorView = new SectorView(_sceneManager);
	mSectorView->createView(_sectorName);

	initSector(_sectorName, _sceneManager, _sectorUpdateRate);
	instanciateSectorObjects();
}