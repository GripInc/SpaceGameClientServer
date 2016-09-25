#include "controller/CameraController.h"
#include "controller/GameController.h"
#include "controller/SectorController.h"

#include "view/PlayerCamera.h"
#include "OgreCamera.h"

#include "utils/OgreBulletConvert.h"

#include "OgreRenderWindow.h"
#include "OgreSceneNode.h"

float CameraController::MOUSE_DEAD_ZONE = 0.05f;
float CameraController::JOYSTICK_DEAD_ZONE = 0.06f;

Ogre::Real CameraController::sTopSpeed = 10;

CameraController::CameraController(Ogre::Camera* _camera)
    : mInputManager(NULL),
    mMouse(NULL),
    mKeyboard(NULL),
	mGoingForward(false),
	mGoingBack(false),
	mGoingLeft(false),
	mGoingRight(false),
	mGoingUp(false),
	mGoingDown(false),
	mFastMove(false),
	mFirePressed(false),
	mCamera(_camera),
	mVelocity(Ogre::Vector3(0,0,0))
{}

CameraController::~CameraController(void)
{
}

/// User entries handling ///
bool CameraController::keyPressed( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_UP) 
		mGoingForward = true;
	else if (arg.key == OIS::KC_DOWN) 
		mGoingBack = true;
	else if (arg.key == OIS::KC_LEFT) 
		mGoingLeft = true;
	else if (arg.key == OIS::KC_RIGHT)
		mGoingRight = true;
	else if (arg.key == OIS::KC_PGUP) 
		mGoingUp = true;
	else if (arg.key == OIS::KC_PGDOWN) 
		mGoingDown = true;
	else if (arg.key == OIS::KC_RSHIFT)
		mFastMove = true;

    return true;
}

bool CameraController::keyReleased( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_UP) 
		mGoingForward = false;
	else if (arg.key == OIS::KC_DOWN) 
		mGoingBack = false;
	else if (arg.key == OIS::KC_LEFT)
		mGoingLeft = false;
	else if (arg.key == OIS::KC_RIGHT) 
		mGoingRight = false;
	else if (arg.key == OIS::KC_PGUP)
		mGoingUp = false;
	else if (arg.key == OIS::KC_PGDOWN) 
		mGoingDown = false;
	else if (arg.key == OIS::KC_RSHIFT) 
		mFastMove = false;

    return true;
}

bool CameraController::mouseMoved( const OIS::MouseEvent &arg )
{
	mCamera->yaw(Ogre::Degree(-arg.state.X.rel * 0.15f));
	mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * 0.15f));

	return true;
}

bool CameraController::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	mFirePressed = true;

	return true;
}

bool CameraController::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	mFirePressed = false;

    return true;
}

bool CameraController::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// build our acceleration vector based on keyboard input composite
	Ogre::Vector3 accel = Ogre::Vector3::ZERO;
	if (mGoingForward) accel += mCamera->getDirection();
	if (mGoingBack) accel -= mCamera->getDirection();
	if (mGoingRight) accel += mCamera->getRight();
	if (mGoingLeft) accel -= mCamera->getRight();
	if (mGoingUp) accel += mCamera->getUp();
	if (mGoingDown) accel -= mCamera->getUp();

	// if accelerating, try to reach top speed in a certain time
	Ogre::Real topSpeed = mFastMove ? sTopSpeed * 20 : sTopSpeed;
	if (accel.squaredLength() != 0)
	{
		accel.normalise();
		mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	// if not accelerating, try to stop in a certain time
	else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

	Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

	// keep camera velocity below top speed and above epsilon
	if (mVelocity.squaredLength() > topSpeed * topSpeed)
	{
		mVelocity.normalise();
		mVelocity *= topSpeed;
	}
	else if (mVelocity.squaredLength() < tooSmall * tooSmall)
		mVelocity = Ogre::Vector3::ZERO;

	if (mVelocity != Ogre::Vector3::ZERO) mCamera->move(mVelocity * evt.timeSinceLastFrame);

	return true;
}