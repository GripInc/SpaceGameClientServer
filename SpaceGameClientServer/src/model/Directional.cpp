#include "model/Directional.h"

#include "model/GameSettings.h"

void Directional::init(const DirectionalSettings* _settings)
{
	mName = _settings->getName();
	mSpace = _settings->getSpace();
	mHitPoints = _settings->getHitPoints();
	mMass = _settings->getMass();
	mTurnRateMultiplier = _settings->getTurnRateMultiplier();
	mInertiaMultiplier = _settings->getInertiaMultiplier();
}