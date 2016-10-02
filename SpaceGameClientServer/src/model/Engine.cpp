#include "model/Engine.h"

#include "model/GameSettings.h"

void Engine::init(const EngineSettings* _engineSettings)
{
	mName = _engineSettings->getName();
	mReactivity = _engineSettings->getReactivity();
	mPower = _engineSettings->getPower();
	mMaxSpeed = _engineSettings->getMaxSpeed();
	mThrustSensitivity = _engineSettings->getThrustSensitivity();
	mThrustMaxValue = _engineSettings->getThrustMaxValue();

	mWantedThrust = 0.f;
	mRealThrust = 0.f;
}
