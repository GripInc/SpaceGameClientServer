#include "controller/SectorController.h"
#include "controller/ShipInputHandler.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "SectorController";
}

void SectorController::createSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "", false);

	//Init sector
	if(mCurrentSector != NULL)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "Deleting sector because it was not NULL", false);
		delete mCurrentSector;
	}

	mCurrentSector = new Sector(_sceneManager, _sectorUpdateRate, _startingSectorTick);

	// Create the Scene
	mSectorView = new SectorView(_sceneManager);
	mSectorView->createView(_sectorName);

	//DEBUG memory monitoring
	/*for(int i = 0; i < 10000; ++i)
	{
		mCurrentSector->instantiateObjects(GameSettings::getInstance().getSector(_sectorName), mSceneManager, mDynamicWorld);
		//mCurrentSector->addShip(getNextId(), mSceneManager, mDynamicWorld, "FirstShip", Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO);
		delete mCurrentSector;
		mCurrentSector = new Sector();
	}*/

	//Instantiate sector objects
	mCurrentSector->instantiateObjects(_sectorName);
}

void SectorController::receivedSectorState(RakNet::BitStream& _data) const
{
	mCurrentSector->receivedSectorState(_data);
}