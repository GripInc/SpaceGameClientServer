#ifndef _SHIP_INPUT_HANDLER_H_
#define _SHIP_INPUT_HANDLER_H_

#include "OgreFrameListener.h"
#include "OgreVector3.h"
#include "OIS.h"

#include "model/InputState.h"
#include "SpaceGameTypes.h"

class ShipInputHandler : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	//In percent
	static const float MOUSE_DEAD_ZONE;
	static const float JOYSTICK_DEAD_ZONE;

	ShipInputHandler();
	~ShipInputHandler();

	InputState mInputState;

	//Discard send request if input didn't change
	void sendInputToServer(SectorTick _tick);

	bool getHasInputChanged() const { return mInputChanged; }

protected:
	bool mInputChanged;

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	// OIS::JoystickListener
	bool povMoved(const OIS::JoyStickEvent &e, int pov);
	bool axisMoved(const OIS::JoyStickEvent &e, int axis);
	bool sliderMoved(const OIS::JoyStickEvent &e, int sliderID);
	bool buttonPressed(const OIS::JoyStickEvent &e, int button);
	bool buttonReleased(const OIS::JoyStickEvent &e, int button);

	//OIS Input devices
	OIS::InputManager* mInputManager = nullptr;
	OIS::Mouse*    mMouse = nullptr;
	OIS::Keyboard* mKeyboard = nullptr;
};

#endif //_SHIP_INPUT_HANDLER_H_