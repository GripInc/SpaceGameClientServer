#include "controller/ClientSectorController.h"

#include "controller/ShipInputHandler.h"

#include "network/ClientNetworkService.h"

#include "model/ClientSector.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ClientSectorController";
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

void ClientSectorController::instanciateSectorObjects()
{
	mCurrentSector->instantiateObjects();
}

void ClientSectorController::instantiatePlayerShip(Ship& _playerShip, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode)
{
	mCurrentSector->instantiatePlayerShip(_playerShip, _orientation, _position, _uniqueId, _rakNetGUID, _cameraSceneNode);
}

void ClientSectorController::updateSector(const ShipInputHandler& _shipInputHandler)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START", false);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "mLastAcknowledgedInput:" + StringUtils::toStr(mLastAcknowledgedInput), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "mLastSimulatedInput:" + StringUtils::toStr(mLastSimulatedInput), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "mSectorTick:" + StringUtils::toStr(mSectorTick), false);

	///Input handling
	//Remove acked inputs
	while (!mPlayerInputHistory.empty() && mPlayerInputHistory.front().mTick < mLastSimulatedInput)
		mPlayerInputHistory.pop_front();

	//Add last input
	mPlayerInputHistory.push_back(_shipInputHandler.mInputState);
	mPlayerInputHistory.back().mTick = mSectorTick;

	//Send input batch
	ClientNetworkService::getInstance().sendInput(mPlayerInputHistory, mLastAcknowledgedInput);

	//Finally Update sector
	mCurrentSector->updateSector(mSectorTick, mPlayerInputHistory);

	mSectorTick++;

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END tick is now " + StringUtils::toStr(mSectorTick), false, true);
}

void ClientSectorController::updateSectorView(float _elapsedTime)
{
	mCurrentSector->updateSectorView(_elapsedTime, mSectorTick - 1);
}

void ClientSectorController::receivedSectorState(const std::map<RakNet::RakNetGUID, ShipState>& _shipStates, SectorTick _lastAcknowledgedInput, SectorTick _lastSimulatedInput)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "START", false);

	if (mLastSimulatedInput < _lastSimulatedInput)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "mLastSimulatedInput(" + StringUtils::toStr(mLastSimulatedInput) + ") < _lastSimulatedInput(" + StringUtils::toStr(_lastSimulatedInput) + ") using sector state", false);

		mLastAcknowledgedInput = _lastAcknowledgedInput;
		mLastSimulatedInput = _lastSimulatedInput;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "mLastAcknowledgedInput is now " + StringUtils::toStr(mLastAcknowledgedInput), false);

		mCurrentSector->storeReceivedSectorState(_shipStates, mLastSimulatedInput);
	}
	else
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "mLastSimulatedInput(" + StringUtils::toStr(mLastSimulatedInput) + ") >= _lastSimulatedInput(" + StringUtils::toStr(_lastSimulatedInput) + ") dropped sector state", false);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "END", false);
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