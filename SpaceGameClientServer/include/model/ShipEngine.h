#ifndef _SHIP_ENGINE_H_
#define _SHIP_ENGINE_H_

#include <string>

class EngineSettings;

class Engine
{
public:
	void init(const EngineSettings& _engineSettings);

	///Getters
	const std::string& getName() const { return mName; }
	float getReactivity() const { return mReactivity; }
	float getPower() const { return mPower; }
	float getMaxSpeed() const { return mMaxSpeed; }
	float getThrustSensitivity() const { return mThrustSensitivity; }
	float getThrustMaxValue() const { return mThrustMaxValue; }

	float mWantedThrust = 0.f;
	float mRealThrust = 0.f;

protected:
	std::string mName;
	///The rate real thrust approach wanted thrust
	float mReactivity = 0.f;
	///
	float mPower = 0.f;
	///Max gaz ejection speed
	float mMaxSpeed = 0.f;
	///The rate the player can add power in engine
	float mThrustSensitivity = 0.f;
	float mThrustMaxValue = 0.f;
};

#endif //_SHIP_ENGINE_H_