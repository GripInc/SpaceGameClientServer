#include "controller/ClientSectorController.h"

#include "model/ClientSector.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "SectorController";
}

void ClientSectorController::createSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "", false);

	//Init sector
	if (mCurrentSector != NULL)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "Deleting sector because it was not NULL", false);
		delete mCurrentSector;
	}

	mCurrentSector = new ClientSector(_sectorName, _sceneManager, _sectorUpdateRate, _startingSectorTick);

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
	mCurrentSector->instantiateObjects();
}

void ClientSectorController::instantiatePlayerShip(Ship& _playerShip, const std::string& _shipId, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode)
{
	mCurrentSector->instantiatePlayerShip(_playerShip, _shipId, _orientation, _position, _uniqueId, _rakNetGUID, _cameraSceneNode);
}

void ClientSectorController::updateSector(ShipInputHandler& _shipInputHandler)
{
	mCurrentSector->updateSector(_shipInputHandler); 
}

void ClientSectorController::receivedSectorState(RakNet::BitStream& _data) const
{
	mCurrentSector->receivedSectorState(_data);
}

const Ship* ClientSectorController::getPlayerShipConst() const
{
	return mCurrentSector->getPlayerShip(); 
}

Ship* ClientSectorController::getPlayerShip()
{
	return mCurrentSector->getPlayerShip(); 
}

void ClientSectorController::switchDisplayDebug()
{ 
	mCurrentSector->switchDisplayDebug(); 
}

void ClientSectorController::switchDisplay()
{
	mCurrentSector->switchDisplay(); 
}