#ifndef _INPUT_STATE_H_
#define _INPUT_STATE_H_

class InputState
{
public:
	std::string getDebugString()
	{
		std::string result;

		result += "mAKeyPressed:";
		result += std::string(mAKeyPressed ? "true" : "false");

		result += "\nmQKeyPressed:";
		result += std::string(mQKeyPressed ? "true" : "false");

		result += "\nmWKeyPressed:";
		result += std::string(mWKeyPressed ? "true" : "false");

		result += "\nmXKeyPressed:";
		result += std::string(mXKeyPressed ? "true" : "false");

		result += "\nmSKeyPressed:";
		result += std::string(mSKeyPressed ? "true" : "false");

		result += "\nmZKeyPressed:";
		result += std::string(mZKeyPressed ? "true" : "false");

		result += "\nmCertified:";
		result += std::string(mCertified ? "true" : "false");

		return result;
	}

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

	//Used to know if the input state is a copy generated because of missing inputs, or an actually sent by client input.
	//Shouldn't be sent via network
	bool mCertified = false;
};

#endif //_INPUT_STATE_H_