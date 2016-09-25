#include "controller/GameController.h"

#include "model/GameSettings.h"

#include "controller/InputController.h"
#include "controller/SectorController.h"

#include "view/PlayerCamera.h"

#include "OgreRoot.h"

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
