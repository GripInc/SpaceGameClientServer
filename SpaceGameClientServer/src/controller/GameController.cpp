#include "controller/GameController.h"

#include "model/GameSettings.h"
#include "SpaceGameTypes.h"

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
const float GameController::GAME_UPDATE_RATE = 1.f / 20.f;

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

bool GameController::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	//DeltaTime
	if (evt.timeSinceLastFrame == 0.f)
		return true;

	mGameUpdateAccumulator += evt.timeSinceLastFrame;

	mLoopTimer.reset();
	unsigned long loopDurationTime = mLoopTimer.getMilliseconds();
	while (mGameUpdateAccumulator > GAME_UPDATE_RATE)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "frameRenderingQueued", "Entering update loop.", false);

		//Update sector
		updateSector();

		//Process network buffer. In server, add clients input in history
		processNetworkBuffer();

		//Capture input and pass it to all registered controllers
		mInputController->capture();

		mGameUpdateAccumulator -= GAME_UPDATE_RATE;
	}
	loopDurationTime = mLoopTimer.getMilliseconds() - loopDurationTime;

	if (loopDurationTime > GAME_UPDATE_RATE * 1000)
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "frameRenderingQueued", "loopDurationTime was " + StringUtils::toStr(loopDurationTime) + " : Simulation is getting late!");

	//Debug panel
	updateDebugPanel(evt.timeSinceLastFrame);
	
	//DEBUG
	for (int i = 0; i < mLaggyValue; i++)
	{
		mLaggyValue = mLaggyValue;
	}

	//Handle switching if needed (client only)
	handleSwitching();

	return true;
}

void GameController::handleSwitching()
{
}