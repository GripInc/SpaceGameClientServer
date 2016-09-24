#include "controller/GameController.h"
#include "model/Sector.h"
#include "model/ObjectPart.h"
#include "model/GameSettings.h"

#include "controller/InputController.h"
#include "controller/UIController.h"
#include "controller/SectorController.h"
#include "controller/StationController.h"
#include "controller/ShipInputHandler.h"

#include "view/PlayerCamera.h"

#include "RakString.h"
#include "OgreRoot.h"
#include "OgreSceneNode.h"

#include "utils/StringUtils.h"
#include "utils/OgreBulletConvert.h"
#include "utils/OgreUtils.h"
#include "utils/BulletDebugDraw.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "GameController";
}

const float GameController::sDebugPanelRefreshRate = 0.1f;
const float GameController::GAME_UPDATE_RATE = 1.f / 30.f;

/** Init */
void GameController::init(const std::string& _gameSettingsFilePath, Ogre::Root* _root, Ogre::RenderWindow* _renderWindow, Ogre::SceneManager* _sceneManager, NetworkLayer& _networkLayer)
{
	_networkLayer.registerConnectionReadyListener(this);

	NetworkService::getInstance().init(&_networkLayer, this);
	_networkLayer.registerNetworkService(&NetworkService::getInstance());

	mRenderWindow = _renderWindow;

	mRoot = _root;
	mRoot->addFrameListener(this);
	
	mSceneManager = _sceneManager;

	//Init camera
	PlayerCamera::getInstance().init(mSceneManager, mRenderWindow);

	//Create input controller
	mInputController = InputController::getSingletonPtr();
	mInputController->initialise(mRenderWindow);

	//Read game xml files
	GameSettings::getInstance().init(_gameSettingsFilePath);

	//DEBUG
	mDebugPanelLastRefresh = 0.f;
	mLaggyValue = 0;
}

void GameController::notifyIsConnected(bool _value)
{
	if(_value)
		NetworkService::getInstance().getPlayerData();
}

void GameController::startGame(const RakNet::RakString& _data)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "startGame", "", false);

	//Fill playerData from server data
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(_data.C_String(), root);

	if(parsingSuccessful)
	{
		mPlayerData.deserializeFromJsonNode(root);
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "startGame", "Cannot parse player data received from server. Data was: DATA_START###" + std::string(_data.C_String()) + "###DATA_END");
		assert(false);
		return;
	}

	switchToStationMode(mPlayerData.mLastStation);
}

void GameController::prepareSwitchToStationMode(const std::string& _stationName)
{
	mSwitchToStationData = new SwitchToStationData();
	mSwitchToStationData->mStationName = _stationName;
}

void GameController::prepareSwitchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick)
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

void GameController::switchToStationMode(const std::string& _stationName)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "switchToStationMode", "_stationName is : " + _stationName, false);

	//TODO
	//Clean sector if needed
	//Remove input listeners
	mInputController->removeAllListeners();

	if(!mStationController)
		mStationController = new StationController(this);

	mStationController->init(*GameSettings::getInstance().getStation(_stationName), mRenderWindow, mInputController->getMouse(), mSceneManager);
	mInputController->addMouseListener(mStationController, "StationUIController");
}

void GameController::switchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "switchToInSpaceMode", "_sectorName is : " + _sectorName + "; _uniqueId is : " + StringUtils::toStr(_uniqueId) + "; _rakNetGUID is : " + std::string(_rakNetGUID.ToString()) + "; _sectorTick is : " + StringUtils::toStr(_sectorTick), false);

	//Remove input listeners
	mInputController->removeAllListeners();

	//Clean station
	if(mStationController)
		mStationController->clean();

	//Link gameController to input controller
	mInputController->addKeyListener(this, "GameController"); //May be removed in the future if not needed

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
	mSectorController = new SectorController();
	mSectorController->createSector(_sectorName, mSceneManager, GAME_UPDATE_RATE, _sectorTick);
	mSectorController->instantiatePlayerShip(mPlayerData.mPlayerShip, mPlayerData.mShipId, _position, _orientation, _uniqueId, _rakNetGUID);
}

bool GameController::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	//DeltaTime
	if(evt.timeSinceLastFrame == 0.f)
		return true;

	mGameUpdateAccumulator += evt.timeSinceLastFrame;

	mLoopTimer.reset();
	unsigned long loopDurationTime = mLoopTimer.getMilliseconds();
	while(mGameUpdateAccumulator > GAME_UPDATE_RATE)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "frameRenderingQueued", "Entering update loop.", false);

		//Network
		//Write input in mClientsInput if any in buffer
		//Receive launch requests
		NetworkService::getInstance().processNetworkBuffer();

		if(mSectorController)
		{
			//Sector update for next sector tick
			mSectorController->updateSector(mShipInputHandler);
		}
		
		//Capture input and pass it to all registered controllers
		mInputController->capture();

		mGameUpdateAccumulator -= GAME_UPDATE_RATE;
	}
	loopDurationTime = mLoopTimer.getMilliseconds() - loopDurationTime;

	if (loopDurationTime > GAME_UPDATE_RATE * 1000)
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "frameRenderingQueued", "loopDurationTime was " + StringUtils::toStr(loopDurationTime) + " : Simulation is getting late!");

	//DEBUG PANEL
	if(mSectorController)
	{
		if(mUIController->getDebugPanel()->getAllParamNames().size() == 0)
		{
			Ogre::StringVector paramNames;
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[0]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[1]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[2]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[3]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[4]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[5]->getName());
			paramNames.push_back(mSectorController->getPlayerShip()->getObjectParts()[6]->getName());
			paramNames.push_back("LaggyValue");
			paramNames.push_back("PosX");
			paramNames.push_back("PosY");
			paramNames.push_back("PosZ");
			paramNames.push_back("engineForce");
			paramNames.push_back("pitch");
			paramNames.push_back("yaw");

			mUIController->getDebugPanel()->setAllParamNames(paramNames);
		}
		mDebugPanelLastRefresh += evt.timeSinceLastFrame;
		if(mDebugPanelLastRefresh > sDebugPanelRefreshRate)
		{
			mUIController->getDebugPanel()->setParamValue(0, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[0]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(1, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[1]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(2, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[2]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(3, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[3]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(4, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[4]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(5, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[5]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(6, StringUtils::toStr(mSectorController->getPlayerShip()->getObjectParts()[6]->mHitPoints));
			mUIController->getDebugPanel()->setParamValue(7, StringUtils::toStr(mLaggyValue));
			mUIController->getDebugPanel()->setParamValue(8, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().x));
			mUIController->getDebugPanel()->setParamValue(9, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().y));
			mUIController->getDebugPanel()->setParamValue(10, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getPosition().z));
			mUIController->getDebugPanel()->setParamValue(11, StringUtils::toStr(mSectorController->getPlayerShip()->mEngineRealForce));
			mUIController->getDebugPanel()->setParamValue(12, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getOrientation().getPitch()));
			mUIController->getDebugPanel()->setParamValue(13, StringUtils::toStr(mSectorController->getPlayerShip()->getSceneNode()->getOrientation().getYaw()));
			mDebugPanelLastRefresh = 0.f;
		}
	}

	//DEBUG
	for(int i = 0; i < mLaggyValue; i++)
	{
		mLaggyValue = mLaggyValue;
	}

	//Switch
	if(mSwitchToSpaceData)
	{
		switchToInSpaceMode(mSwitchToSpaceData->mPosition, mSwitchToSpaceData->mOrientation, mSwitchToSpaceData->mSectorName, mSwitchToSpaceData->mUniqueId, mSwitchToSpaceData->mRakNetGUID, mSwitchToSpaceData->mSectorTick);
		mSwitchToSpaceData = NULL;
	}
	else if(mSwitchToStationData)
	{
		switchToStationMode(mSwitchToStationData->mStationName);
		mSwitchToStationData = NULL;
	}

	return true;
}

//TODO move to UIController
/// User entries handling ///
bool GameController::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

//TODO move to UIController
bool GameController::keyReleased( const OIS::KeyEvent &arg )
{
	switch(arg.key)
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
		default:break;
	}

	return true;
}

void GameController::receivedSectorState(RakNet::BitStream& _data) const
{
	if(mSectorController)
		mSectorController->receivedSectorState(_data);
}
