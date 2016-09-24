#ifndef _INPUT_STATE_H_
#define _INPUT_STATE_H_

class InputState
{
public:
	bool mAKeyPressed = false;
	bool mQKeyPressed = false;
	bool mWKeyPressed = false;
	bool mXKeyPressed = false;
	bool mSKeyPressed = false;
	bool mZKeyPressed = false;

	float mJoystickXAbs = 0.f;
	float mJoystickYAbs = 0.f;
	float mMouseXAbs = 0.f;
	float mMouseYAbs = 0.f;

	bool mUpKeyPressed = false;
	bool mDownKeyPressed = false;
	bool mLeftKeyPressed = false;
	bool mRightKeyPressed = false;

	bool mFirePressed = false;
};

#endif //_INPUT_STATE_H_