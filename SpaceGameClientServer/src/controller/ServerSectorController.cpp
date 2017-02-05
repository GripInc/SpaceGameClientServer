#include "controller/ServerSectorController.h"

#include "model/ServerSector.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

const unsigned int ServerSectorController::SERVER_INPUT_BUFFER_LENGTH = 4;

namespace
{
	const std::string LOG_CLASS_TAG = "ServerSectorController";
}

void ServerSectorController::initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
{
	if (mCurrentSector != NULL)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "Deleting sector because it was not NULL", false);
		delete mCurrentSector;
	}

	mCurrentSector = new ServerSector(_sectorName, _sceneManager, _sectorUpdateRate);
}

void ServerSectorController::instanciateSectorObjects()
{
	mCurrentSector->instantiateObjects();
}

void ServerSectorController::instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId& _shipUniqueId)
{
	return mCurrentSector->instantiateClientShip(_id, _ship, _orientation, _position, _shipUniqueId);
}

void ServerSectorController::updateSector()
{
	mCurrentSector->updateSector(mClientsInput);
	
	mClientsInput.incrementNextInputToUse();

	mCurrentSector->updateSectorView();

	mSectorTick++;
}

void ServerSectorController::addInputs(const RakNet::RakNetGUID& _id, const std::list<InputState>& _clientInputs)
{
	//TODO retrieve the sector the client is in
	//Add input in this sector input history

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "START", false);

	//If it is first input received, we also fill mNextInputToUse
	if (mClientsInput.addInputs(_id, _clientInputs))
	{
		SectorTick firstInputToUse = _clientInputs.front().mTick - SERVER_INPUT_BUFFER_LENGTH;
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "setting first input:" + StringUtils::toStr(firstInputToUse), false);
		mClientsInput.setFirstInputToUse(_id, firstInputToUse);
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Added input was not the first one", false);
	}

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "END", false);
}

void ServerSectorController::switchDisplayDebug()
{
	mCurrentSector->switchDisplayDebug(); 
}

void ServerSectorController::switchDisplay()
{
	mCurrentSector->switchDisplay(); 
}