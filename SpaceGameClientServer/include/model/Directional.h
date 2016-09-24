#ifndef _DIRECTIONAL_H_
#define _DIRECTIONAL_H_

#include <string>

class DirectionalSettings;

class Directional
{
public:
	void init(const DirectionalSettings& _settings);

	float getInertiaMultiplier() const { return mInertiaMultiplier; }
	float getTurnRateMultiplier() const { return mTurnRateMultiplier; }

protected:
	std::string mName;
	unsigned int mSpace = 0U;
	unsigned int mHitPoints = 0U;
	unsigned int mMass = 0U;
	float mTurnRateMultiplier = 0.f;
	float mInertiaMultiplier = 0.f;
};

#endif //_DIRECTIONAL_H_