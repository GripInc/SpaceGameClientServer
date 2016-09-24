#include "model/Directional.h"

#include "model/GameSettings.h"

void Directional::init(const DirectionalSettings& _settings)
{
	mName = _settings.mName;
	mSpace = _settings.mSpace;
	mHitPoints = _settings.mHitPoints;
	mMass = _settings.mMass;
	mTurnRateMultiplier = _settings.mTurnRateMultiplier;
	mInertiaMultiplier = _settings.mInertiaMultiplier;
}