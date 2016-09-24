#include "controller/StationController.h"

#include "controller/GameController.h"

#include "model/GameSettings.h"

#include "view/StationView.h"

#include "manager/LoggerManager.h"

const std::string StationController::TARGET_SPECIAL_LAUNCH = "special_launch";

void StationController::init(const StationSettings& _stationSettings, Ogre::RenderWindow* _renderWindow, OIS::Mouse* _mouse, Ogre::SceneManager* _sceneManager)
{
	OgreBites::InputContext inputContext;
	inputContext.mMouse = _mouse;
	mTrayManager = new OgreBites::SdkTrayManager("StationInterface", _renderWindow, inputContext, this);
	mNextScreenLabel = mTrayManager->createLabel(OgreBites::TL_BOTTOM, "nextScreenName", "", 200.f);

	mStationSettings = &_stationSettings;
	mStationScreenSettings = GameSettings::getInstance().getStationScreen(_stationSettings.mScreenModel);

	mStationView = new StationView();
	mStationView->init(_sceneManager);

	gotoScreen(mStationScreenSettings->mFirstScreen);
}

//Based on StationSettings
bool StationController::isScreenAvailable(const std::string& _screen)
{
	//TODO
	return true;
}

void StationController::clean()
{
	//View
	delete mStationView;
	mStationView = NULL;

	//Tray manager
	delete mTrayManager;
	mTrayManager = NULL;
}

StationController::~StationController()
{
	delete mTrayManager;
	mTrayManager = NULL;
}

bool StationController::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}
bool StationController::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}
bool StationController::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayManager->injectPointerMove(arg))
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

	std::string nextScreen = "";
	int mouseX = arg.state.X.abs;
	int mouseY = arg.state.Y.abs;
	for(int i = 0; i < mCurrentScreen->mClickZones.size(); ++i)
	{
		if(mouseX > mCurrentScreen->mClickZones[i].mX && mouseX < mCurrentScreen->mClickZones[i].mX + mCurrentScreen->mClickZones[i].mWidth &&
			mouseY > mCurrentScreen->mClickZones[i].mY && mouseY < mCurrentScreen->mClickZones[i].mY + mCurrentScreen->mClickZones[i].mHeight)
		{
			nextScreen = mCurrentScreen->mClickZones[i].mTarget;
			break;
		}
	}

	mNextScreenLabel->setCaption(nextScreen);
	return true;
}
bool StationController::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayManager->injectPointerDown(arg, id))
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

	return true;
}
bool StationController::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayManager->injectPointerUp(arg, id))
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

	std::string nextScreen = "";
	int mouseX = arg.state.X.abs;
	int mouseY = arg.state.Y.abs;
	for(int i = 0; i < mCurrentScreen->mClickZones.size(); ++i)
	{
		if(mouseX > mCurrentScreen->mClickZones[i].mX && mouseX < mCurrentScreen->mClickZones[i].mX + mCurrentScreen->mClickZones[i].mWidth &&
			mouseY > mCurrentScreen->mClickZones[i].mY && mouseY < mCurrentScreen->mClickZones[i].mY + mCurrentScreen->mClickZones[i].mHeight &&
			isScreenAvailable(mCurrentScreen->mClickZones[i].mName))
		{
			nextScreen = mCurrentScreen->mClickZones[i].mTarget;
			break;
		}
	}

	mNextScreenLabel->setCaption("");

	if(!nextScreen.empty())
	{
		if(nextScreen == TARGET_SPECIAL_LAUNCH)
		{
			NetworkService::getInstance().requireLaunchFromStation();
		}
		else
		{
			gotoScreen(nextScreen);
		}
	}
	
	return true;
}

void StationController::gotoScreen(const std::string& _screen)
{
	mCurrentScreen = mStationScreenSettings->getScreen(_screen);

	mStationView->createView(*mCurrentScreen);
}