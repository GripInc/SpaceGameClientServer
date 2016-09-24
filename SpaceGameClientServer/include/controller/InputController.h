#ifndef InputController_H
#define InputController_H

#include <OISMouse.h>
#include <OISKeyboard.h>
#include <OISJoyStick.h>
#include <OISInputManager.h>

#include <OgreRenderWindow.h>

class InputController : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	virtual ~InputController(void);

	void initialise(Ogre::RenderWindow *renderWindow);
	void capture(void);

	void addKeyListener(OIS::KeyListener *keyListener, const std::string& instanceName);
	void addMouseListener(OIS::MouseListener *mouseListener, const std::string& instanceName);
	void addJoystickListener(OIS::JoyStickListener *joystickListener, const std::string& instanceName);

	void removeKeyListener(const std::string& instanceName);
	void removeMouseListener(const std::string& instanceName);
	void removeJoystickListener(const std::string& instanceName);

	void removeKeyListener(OIS::KeyListener *keyListener);
	void removeMouseListener(OIS::MouseListener *mouseListener);
	void removeJoystickListener(OIS::JoyStickListener *joystickListener);

	void removeAllListeners(void);
	void removeAllKeyListeners(void);
	void removeAllMouseListeners(void);
	void removeAllJoystickListeners(void);

	void setWindowExtents(int width, int height);

	OIS::Mouse*    getMouse(void);
	OIS::Keyboard* getKeyboard(void);
	OIS::JoyStick* getJoystick(unsigned int index);

	int getNumOfJoysticks(void);

	static InputController* getSingletonPtr(void);
private:
	InputController(void);
	InputController(const InputController&) { }

	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);

	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool povMoved(const OIS::JoyStickEvent &e, int pov);
	bool axisMoved(const OIS::JoyStickEvent &e, int axis);
	bool sliderMoved(const OIS::JoyStickEvent &e, int sliderID);
	bool buttonPressed(const OIS::JoyStickEvent &e, int button);
	bool buttonReleased(const OIS::JoyStickEvent &e, int button);

	OIS::Mouse        *mMouse = nullptr;
	OIS::Keyboard     *mKeyboard = nullptr;
	OIS::InputManager *mInputSystem = nullptr;

	std::vector<OIS::JoyStick*> mJoysticks;
	std::vector<OIS::JoyStick*>::iterator itJoystick;
	std::vector<OIS::JoyStick*>::iterator itJoystickEnd;

	std::map<std::string, OIS::KeyListener*> mKeyListeners;
	std::map<std::string, OIS::MouseListener*> mMouseListeners;
	std::map<std::string, OIS::JoyStickListener*> mJoystickListeners;

	std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
	std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListener;

	std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
	std::map<std::string, OIS::JoyStickListener*>::iterator itJoystickListenerEnd;

	static InputController *mInputController;
};
#endif

//Old version
/*
#ifndef _INPUT_CONTROLLER_H_
#define _INPUT_CONTROLLER_H_

#include "OgreFrameListener.h"
#include "OgreWindowEventUtilities.h"
#include "OIS.h"

namespace Ogre
{
	class RenderWindow;
	class Camera;
}

class InputController : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
	InputController(Ogre::RenderWindow *_window);
	~InputController();

	void start();

	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
	Ogre::RenderWindow* mWindow;

	//OIS Input devices
	OIS::InputController* mInputController;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	void windowResized(Ogre::RenderWindow* _renderWindow);
	void windowClosed(Ogre::RenderWindow* _renderWindow);

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

#endif //_INPUT_CONTROLLER_H_
*/