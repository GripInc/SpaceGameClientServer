#include "model/ObjectPart.h"

#include "model/deserialized/ObjectPartSettings.h"

void ObjectPart::init(const ObjectPartSettings& _objectPartSettings)
{
	mName = _objectPartSettings.getName();
	mHitPoints = _objectPartSettings.getHitPoints();
}

void ObjectPart::init(const std::string& _name, int _hitPoints)
{
	mName = _name;
	mHitPoints = _hitPoints;
}