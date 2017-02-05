#include "controller/ClientGameController.h"

#include "controller/InputController.h"
#include "controller/StationController.h"
#include "controller/UIController.h"
#include "controller/ClientSectorController.h"

#include "view/PlayerCamera.h"

//Used for debug includes
#include "model/Ship.h"
#include "model/ObjectPart.h"
#include "OgreSceneNode.h"

#include "network/ClientNetworkService.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ClientGameController";
}

void ClientGameController::init(const std::string& _gameSettingsFilePath, Ogre::Root* _root, Ogre::RenderWindow* _renderWindow, Ogre::SceneManager* _sceneManager, NetworkLayer& _networkLayer)
{
	ClientNetworkService::getInstance().init(&_networkLayer, this);
	_networkLayer.registerNetworkService(&ClientNetworkService::getInstance());

	GameController::init(_gameSettingsFilePath, _root, _renderWindow, _sceneManager, _networkLayer);

	_networkLayer.registerConnectionReadyListener(this);
}

void ClientGameController::notifyIsConnected(bool _value)
{
	if (_value)
		ClientNetworkService::getInstance().requestPlayerData();
}

void ClientGameController::startGame(const RakNet::RakString& _playerData)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "startGame", "", false);

	//Fill playerData from server data
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(_playerData.C_String(), root);

	if (parsingSuccessful)
	{
		mPlayerData.deserializeFromJsonNode(root);
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "startGame", "Cannot parse player data received from server. Data was: DATA_START###" + std::string(_playerData.C_String()) + "###DATA_END");
		assert(false);
		return;
	}

	//DEBUG
	//Initialize player ship model
	mPlayerData.mPlayerShip.initModel(GameSettings::getInstance().getShip(mPlayerData.mShipId));

	//DEBUG
	mPlayerData.mPlayerShip.addEngine(GameSettings::getInstance().getEngine("FirstEngine"));
	mPlayerData.mPlayerShip.addDirectional(GameSettings::getInstance().getDirectional("SecondDirectional"));
	mPlayerData.mPlayerShip.addWeapon(GameSettings::getInstance().getWeapon("Gun1"), 0);
	mPlayerData.mPlayerShip.addWeapon(GameSettings::getInstance().getWeapon("Gun1"), 1);
	mPlayerData.mPlayerShip.addWeapon(GameSettings::getInstance().getWeapon("Gun1"), 2);
	mPlayerData.mPlayerShip.addWeapon(GameSettings::getInstance().getWeapon("Gun1"), 3);

	switchToStationMode(mPlayerData.mLastStation);
}

void ClientGameController::prepareSwitchToStationMode(const std::string& _stationName)
{
	mSwitchToStationData = new SwitchToStationData();
	mSwitchToStationData->mStationName = _stationName;
}

void ClientGameController::prepareSwitchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick)
{
	mSwitchToSpaceData = new SwitchToSpaceData();
	mSwitchToSpaceData->mPosition = _position;
	mSwitchToSpaceData->mOrientation = _orientation;
	mSwitchToSpaceData->mSectorName = _sectorName;
	mSwitchToSpaceData->mUniqueId = _uniqueId;
	mSwitchToSpaceData->mRakNetGUID = _rakNetGUID;
	mSwitchToSpaceData->mSectorTick = _sectorTick;

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "prepareSwitchToInSpaceMode", "_sectorName is : " + _sectorName + "; _uniqueId is : " + StringUtils::toStr(_uniqueId) + "; _rakNetGUID is : " + std::string(_rakNetGUID.ToString()) + "; _sectorTick is : " + StringUtils::toStr(_sectorTick), false);
}

void ClientGameController::switchToStationMode(const std::string& _stationName)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "switchToStationMode", "_stationName is : " + _stationName, false);

	//TODO
	//Clean sector if needed
	//Remove input listeners
	mInputController->removeAllListeners();

	if (!mStationController)
		mStationController = new StationController();

	mStationController->init(*GameSettings::getInstance().getStation(_stationName), mRenderWindow, mInputController->getMouse(), mSceneManager);
	mInputController->addMouseListener(mStationController, "StationUIController");
}

void ClientGameController::switchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "switchToInSpaceMode", "_sectorName is : " + _sectorName + "; _uniqueId is : " + StringUtils::toStr(_uniqueId) + "; _rakNetGUID is : " + std::string(_rakNetGUID.ToString()) + "; _sectorTick is : " + StringUtils::toStr(_sectorTick), false);

	//Remove input listeners
	mInputController->removeAllListeners();

	//Clean station
	if (mStationController)
		mStationController->clean();

	//Link gameController to input controller
	mInputController->addKeyListener(this, "ClientGameController"); //May be removed in the future if not needed

	//UIController
	mUIController = new UIController(this, mRenderWindow, mInputController->getMouse()); //For now, displays debug panel only

	//Link uiController to input controller (useless for now)
	mInputController->addKeyListener(mUIController, "UIController");
	mInputController->addMouseListener(mUIController, "UIController");

	//Attach shipController to input manager
	mInputController->addKeyListener(&mShipInputHandler, "ShipController");
	mInputController->addMouseListener(&mShipInputHandler, "ShipController");
	mInputController->addJoystickListener(&mShipInputHandler, "ShipController");

	//Create sector
	mSectorController = new ClientSectorController();
	mSectorController->createSector(_sectorName, mSceneManager, GAME_UPDATE_RATE);
	mSectorController->instantiatePlayerShip(mPlayerData.mPlayerShip, _position, _orientation, _uniqueId, _rakNetGUID, PlayerCamera::getInstance().getCameraNode());
}

void ClientGameController::receivedSectorState(RakNet::BitStream& _data) const
{
	//Deserialize sector state
	//Each ship
	std::map<RakNet::RakNetGUID, ShipState> ships;
	ships.clear();
	size_t shipsSize;
	_data.Read(shipsSize);

	RakNet::RakNetGUID rakNetId;
	for (size_t i = 0; i < shipsSize; ++i)
	{
		//Read client unique id
		_data.Read(rakNetId);

		//Read ship unique id
		UniqueId uniqueId;
		_data.Read(uniqueId);

		//Read ship state
		ShipState& shipState = ships[rakNetId];
		shipState.deserialize(_data);
	}

	//Last ackonwledged input
	SectorTick lastAcknowledgedInput;
	_data.Read(lastAcknowledgedInput);

	//Last simulated input
	SectorTick lastSimulatedInput;
	_data.Read(lastSimulatedInput);

	if (mSectorController)
		mSectorController->receivedSectorState(ships, lastAcknowledgedInput, lastSimulatedInput);
}

void ClientGameController::processNetworkBuffer()
{
	ClientNetworkService::getInstance().processNetworkBuffer();
}

void ClientGameController::updateGame()
{
	if (mSectorController)
	{
		//Sector update for next sector tick
		mSectorController->updateSector(mShipInputHandler);
	}
}

void ClientGameController::updateSectorView(float _elapsedTime)
{
	if (mSectorController)
	{
		//Sector update for next sector tick
		mSectorController->updateSectorView(_elapsedTime);
	}
}

void ClientGameController::updateDebugPanel(Ogre::Real _timeSinceLastFrame)
{
	if (mSectorController)
	{
		if (mUIController->getDebugPanel()->getAllParamNames().size() == 0)
		{
			Ogre::StringVector paramNames;
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[0].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[1].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[2].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[3].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[4].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[5].getName());
			//paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[6].getName());
			//paramNames.push_back("LaggyValue");
			//paramNames.push_back("PosX");
			//paramNames.push_back("PosY");
			//paramNames.push_back("PosZ");
			//paramNames.push_back("engineForce");
			//paramNames.push_back("pitch");
			//paramNames.push_back("yaw");
			paramNames.push_back("frame:");

			mUIController->getDebugPanel()->setAllParamNames(paramNames);
		}
		mDebugPanelLastRefresh += _timeSinceLastFrame;
		if (mDebugPanelLastRefresh > sDebugPanelRefreshRate)
		{
			//mUIController->getDebugPanel()->setParamValue(0, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[0].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(1, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[1].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(2, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[2].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(3, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[3].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(4, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[4].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(5, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[5].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(6, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[6].mHitPoints));
			//mUIController->getDebugPanel()->setParamValue(7, StringUtils::toStr(mLaggyValue));
			//mUIController->getDebugPanel()->setParamValue(8, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().x));
			//mUIController->getDebugPanel()->setParamValue(9, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().y));
			//mUIController->getDebugPanel()->setParamValue(10, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().z));
			//mUIController->getDebugPanel()->setParamValue(11, StringUtils::toStr(mSectorController->getPlayerShip()->mEngineRealForce));
			//mUIController->getDebugPanel()->setParamValue(12, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getOrientation().getPitch()));
			//mUIController->getDebugPanel()->setParamValue(13, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getOrientation().getYaw()));
			mUIController->getDebugPanel()->setParamValue(0, LoggerManager::getInstance().getDate());
			mDebugPanelLastRefresh = 0.f;
		}
	}
}

void ClientGameController::handleSwitching()
{
	if (mSwitchToSpaceData)
	{
		switchToInSpaceMode(mSwitchToSpaceData->mPosition, mSwitchToSpaceData->mOrientation, mSwitchToSpaceData->mSectorName, mSwitchToSpaceData->mUniqueId, mSwitchToSpaceData->mRakNetGUID, mSwitchToSpaceData->mSectorTick);
		mSwitchToSpaceData = NULL;
	}
	else if (mSwitchToStationData)
	{
		switchToStationMode(mSwitchToStationData->mStationName);
		mSwitchToStationData = NULL;
	}
}

//TODO move to UIController
/// User entries handling ///
bool ClientGameController::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

//TODO move to UIController
bool ClientGameController::keyReleased(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
	case OIS::KC_F1:
		mSectorController->switchDisplayDebug();
		break;
	case OIS::KC_F2:
		mSectorController->switchDisplay();
		break;
	case OIS::KC_F3:
		mLaggyValue *= 2;
		break;
	case OIS::KC_F4:
		mLaggyValue /= 2;
		break;
	case OIS::KC_ESCAPE:
		mDoQuitApplication = true;
		break;
	default:break;
	}

	return true;
}