#include "controller/ClientSectorController.h"

#include "model/ClientSector.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "SectorController";
}

void ClientSectorController::initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
{
	if (mCurrentSector != NULL)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "Deleting sector because it was not NULL", false);
		delete mCurrentSector;
	}

	mCurrentSector = new ClientSector(_sectorName, _sceneManager, _sectorUpdateRate);
}

void ClientSectorController::setFirstTick(SectorTick _firstTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "setFirstTick", "_firstTick is : " + StringUtils::toStr(_firstTick) , false);

	mCurrentSector->setFirstTick(_firstTick); 
}

void ClientSectorController::instanciateSectorObjects()
{
	mCurrentSector->instantiateObjects();
}

void ClientSectorController::instantiatePlayerShip(Ship& _playerShip, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode)
{
	mCurrentSector->instantiatePlayerShip(_playerShip, _orientation, _position, _uniqueId, _rakNetGUID, _cameraSceneNode);
}

void ClientSectorController::updateSector(ShipInputHandler& _shipInputHandler)
{
	mCurrentSector->updateSector(_shipInputHandler);
}

void ClientSectorController::updateSectorView(float _elapsedTime)
{
	mCurrentSector->updateSectorView(_elapsedTime);
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