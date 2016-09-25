#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include "OgreFrameListener.h"
#include "OgreVector3.h"
#include "OIS.h"

class GameController;

class CameraController : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
	//In percent
	static float MOUSE_DEAD_ZONE;
	static float JOYSTICK_DEAD_ZONE;

	static Ogre::Real sTopSpeed;

	CameraController(Ogre::Camera* _camera);
	~CameraController();

	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mGoingUp;
	bool mGoingDown;
	bool mFastMove;

	bool mFirePressed;

protected:
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	//OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	//Camera
	Ogre::Camera* mCamera;

	Ogre::Vector3 mVelocity;
};

#endif //_CAMERA_CONTROLLER_H_