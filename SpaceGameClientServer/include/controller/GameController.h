#ifndef _GAME_CONTROLLER_H_
#define _GAME_CONTROLLER_H_

#include "OgreFrameListener.h"
#include "OgreTimer.h"

#include "OIS.h"

class NetworkLayer;
class InputController;
class UIController;

class GameController : public Ogre::FrameListener, public OIS::KeyListener
{
public:

	///Initialize game controller
	virtual void init(const std::string& _gameSettingsFilePath, Ogre::Root* _root, Ogre::RenderWindow* _renderWindow, Ogre::SceneManager* _sceneManager, NetworkLayer& _networkLayer);

protected:
	static const float GAME_UPDATE_RATE;
	float mGameUpdateAccumulator = 0.f;
	Ogre::Timer mLoopTimer;

	Ogre::RenderWindow* mRenderWindow = nullptr;
	Ogre::Root* mRoot = nullptr;

	Ogre::SceneManager* mSceneManager = nullptr;

	InputController* mInputController = nullptr;
	UIController* mUIController = nullptr;

	/// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) = 0;

	//DEBUG
	float mDebugPanelLastRefresh = 0.f;
	static const float sDebugPanelRefreshRate;

	long mLaggyValue = 0;
};

#endif //_GAME_CONTROLLER_H_