#ifndef _PLANET_OBJECT_H_
#define _PLANET_OBJECT_H_

#include <string>

#include "model/SectorObject.h"

class PlanetObject : public SectorObject
{
public:
	static const std::string PLANET_SPHERE_MANUAL_OBJECT_ID;
	static bool sManualObjectIsReady;

protected:
	bool mManualObjectIsReady = false;

	//Create entity
	virtual Ogre::Entity* createEntity(const std::string& _name, const std::string& _mesh, const Ogre::Vector3& _scale) override;
};

#endif //_PLANET_OBJECT_H_