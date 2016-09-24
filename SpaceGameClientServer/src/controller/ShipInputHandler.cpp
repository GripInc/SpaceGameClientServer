#include "controller/ShipInputHandler.h"

#include "controller/GameController.h"
#include "controller/SectorController.h"

#include "utils/OgreBulletConvert.h"
#include "manager/LoggerManager.h"

#include "OgreRenderWindow.h"
#include "OgreSceneNode.h"

const float ShipInputHandler::MOUSE_DEAD_ZONE = 0.05f;
const float ShipInputHandler::JOYSTICK_DEAD_ZONE = 0.06f;

ShipInputHandler::ShipInputHandler()
    : mInputManager(NULL),
    mMouse(NULL),
    mKeyboard(NULL),
	mInputChanged(false)
{}

ShipInputHandler::~ShipInputHandler(void)
{
}

void ShipInputHandler::sendInputToServer(SectorTick _tick)
{
	if(mInputChanged)
	{
		LoggerManager::getInstance().logI("ShipInputHandler", "sendInputToServer", "Send input change to server. Tick is " + StringUtils::toStr(_tick), false);
		NetworkService::getInstance().sendShipInput(_tick, mInputState);
		mInputChanged = false;
	}
}

/// User entries handling ///
bool ShipInputHandler::keyPressed( const OIS::KeyEvent &arg )
{
	mInputChanged = true;

	if (arg.key == OIS::KC_UP) 
		mInputState.mUpKeyPressed = true;
	else if (arg.key == OIS::KC_DOWN) 
		mInputState.mDownKeyPressed = true;
	else if (arg.key == OIS::KC_LEFT) 
		mInputState.mLeftKeyPressed = true;
	else if (arg.key == OIS::KC_RIGHT) 
		mInputState.mRightKeyPressed = true;
	else if (arg.key == OIS::KC_A) 
		mInputState.mAKeyPressed = true;
	else if (arg.key == OIS::KC_Q)
		mInputState.mQKeyPressed = true;
	else if (arg.key == OIS::KC_Z) 
		mInputState.mWKeyPressed = true;
	else if (arg.key == OIS::KC_X)
		mInputState.mXKeyPressed = true;
	else if (arg.key == OIS::KC_S) 
		mInputState.mSKeyPressed = true;
	else if (arg.key == OIS::KC_Z)
		mInputState.mZKeyPressed = true;

    return true;
}

bool ShipInputHandler::keyReleased( const OIS::KeyEvent &arg )
{
	mInputChanged = true;

	if (arg.key == OIS::KC_UP) 
		mInputState.mUpKeyPressed = false;
	else if (arg.key == OIS::KC_DOWN)
		mInputState.mDownKeyPressed = false;
	else if (arg.key == OIS::KC_LEFT) 
		mInputState.mLeftKeyPressed = false;
	else if (arg.key == OIS::KC_RIGHT) 
		mInputState.mRightKeyPressed = false;
	else if (arg.key == OIS::KC_A) 
		mInputState.mAKeyPressed = false;
	else if (arg.key == OIS::KC_Q)
		mInputState.mQKeyPressed = false;
	else if (arg.key == OIS::KC_Z) 
		mInputState.mWKeyPressed = false;
	else if (arg.key == OIS::KC_X)
		mInputState.mXKeyPressed = false;
	else if (arg.key == OIS::KC_S) 
		mInputState.mSKeyPressed = false;
	else if (arg.key == OIS::KC_Z)
		mInputState.mZKeyPressed = false;

    return true;
}

bool ShipInputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
	float oldMouseXValue = mInputState.mMouseXAbs;
	float oldMouseYValue = mInputState.mMouseYAbs;

	mInputState.mMouseXAbs = ((float)arg.state.X.abs - (float)arg.state.width / 2.f) / ((float)arg.state.width / 2.f);
	if(std::fabs(mInputState.mMouseXAbs) < MOUSE_DEAD_ZONE)
		mInputState.mMouseXAbs = 0.f;

	mInputState.mMouseYAbs = ((float)arg.state.Y.abs - (float)arg.state.height / 2.f) / ((float)arg.state.height / 2.f);
	if(std::fabs(mInputState.mMouseYAbs) < MOUSE_DEAD_ZONE)
		mInputState.mMouseYAbs = 0.f;

	mInputChanged = oldMouseXValue != mInputState.mMouseXAbs || oldMouseYValue != mInputState.mMouseYAbs;

    return true;
}

bool ShipInputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	mInputChanged = true;

	mInputState.mFirePressed = true;

	return true;
}

bool ShipInputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	mInputChanged = true;

	mInputState.mFirePressed = false;

    return true;
}

bool ShipInputHandler::povMoved( const OIS::JoyStickEvent &e, int pov )
{
    return true;
}
bool ShipInputHandler::axisMoved( const OIS::JoyStickEvent &e, int axis )
{
	float oldJoystickXValue = mInputState.mJoystickXAbs;
	float oldJoystickYValue = mInputState.mJoystickYAbs;

	mInputState.mJoystickXAbs = (float)e.state.mAxes[2].abs / 32767.f;
	if(mInputState.mJoystickXAbs < JOYSTICK_DEAD_ZONE)
		mInputState.mJoystickXAbs = 0.f;

	mInputState.mJoystickYAbs = (float)e.state.mAxes[1].abs / 32767.f;
	if(mInputState.mJoystickYAbs < JOYSTICK_DEAD_ZONE)
		mInputState.mJoystickYAbs = 0.f;

	mInputChanged = oldJoystickXValue != mInputState.mJoystickXAbs || oldJoystickYValue != mInputState.mJoystickYAbs;

	return true;
}
bool ShipInputHandler:: sliderMoved( const OIS::JoyStickEvent &e, int sliderID )
{
    return true;
}
bool ShipInputHandler::buttonPressed( const OIS::JoyStickEvent &e, int button )
{
	mInputChanged = true;

	mInputState.mFirePressed = true;

    return true;
}
bool ShipInputHandler::buttonReleased( const OIS::JoyStickEvent &e, int button )
{
	mInputChanged = true;

	mInputState.mFirePressed = false;

    return true;
}