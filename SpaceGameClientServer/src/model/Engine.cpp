#include "model/Engine.h"

#include "model/GameSettings.h"

void Engine::init(const EngineSettings& _engineSettings)
{
	mName = _engineSettings.mName;
	mReactivity = _engineSettings.mReactivity;
	mPower = _engineSettings.mPower;
	mMaxSpeed = _engineSettings.mMaxSpeed;
	mThrustSensitivity = _engineSettings.mThrustSensitivity;
	mThrustMaxValue = _engineSettings.mThrustMaxValue;

	mWantedThrust = 0.f;
	mRealThrust = 0.f;
}
