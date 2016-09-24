#ifndef _OBJECT_PART_SETTINGS_H_
#define _OBJECT_PART_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"
#include "model/deserialized/CollisionShapeSettings.h"
#include "LinearMath/btAlignedObjectArray.h"

#include "utils/StringUtils.h"

#include <vector>

class ObjectPartSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		tinyxml2::XMLElement* collisionShape = _node->FirstChildElement("CollisionShape");
		while(collisionShape != NULL)
		{
			mCollisionShapes.push_back(CollisionShapeSettings());
			if(mCollisionShapes[mCollisionShapes.size() - 1].deserialize(collisionShape))
				collisionShape = collisionShape->NextSiblingElement("CollisionShape");
			else
				success &= false;
		}

		XMLHelper::getFloatFrom(_node, "armorSurface", mArmorSurface);

		//Getting allowed parts
		std::string tempString;
		XMLHelper::getStringFrom(_node, "allowedParts", tempString);
		std::vector<std::string> parts;
		StringUtils::split(tempString, ',', parts);
		for(size_t i = 0; i < parts.size(); ++i)
		{
			mAllowedParts.push_back(Types::getEquipementType(parts[i]));
		}

		return success;
	}

	float mArmorSurface = 0.f;

	std::vector<Types::EquipementType> mAllowedParts;
	btAlignedObjectArray<CollisionShapeSettings> mCollisionShapes;
};

#endif //_OBJECT_PART_SETTINGS_H_
