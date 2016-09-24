#include "controller/UIController.h"

#include "controller/GameController.h"

UIController::UIController(GameController* _gameController, Ogre::RenderWindow* _renderWindow, OIS::Mouse* _mouse) : mGameController(_gameController)
{
	OgreBites::InputContext inputContext;
	inputContext.mMouse = _mouse;
	mTrayManager = new OgreBites::SdkTrayManager("MainInterface", _renderWindow, inputContext, this);

	mDebugPanel = mTrayManager->createParamsPanel(OgreBites::TL_TOPLEFT, "DebugValuesPanel", 200, 14);
}

UIController::~UIController()
{
	//Destroys all widgets
	delete mTrayManager;
	mTrayManager = NULL;
}

/// User entries handling ///
bool UIController::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool UIController::keyReleased( const OIS::KeyEvent &arg )
{
	/*switch(arg.key)
	{
		
		default:break;
	}*/

    return true;
}

bool UIController::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayManager->injectPointerMove(arg))
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

	return true;
}

bool UIController::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayManager->injectPointerDown(arg, id))
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

    return true;
}

bool UIController::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayManager->injectPointerUp(arg, id)) 
		return true; //Because Returns true if the event was consumed and should not be passed on to other handler

    return true;
}