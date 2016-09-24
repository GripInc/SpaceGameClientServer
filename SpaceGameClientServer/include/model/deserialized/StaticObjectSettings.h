#ifndef _STATIC_OBJECT_SETTINGS_H_
#define _STATIC_OBJECT_SETTINGS_H_

#include "model/deserialized/SectorObjectSettings.h"
#include "model/deserialized/ObjectPartSettings.h"

class StaticObjectSettings : public SectorObjectSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SectorObjectSettings::deserialize(_node);
	
		//ObjectParts
		tinyxml2::XMLElement* objectPartNode = _node->FirstChildElement("ObjectPart");
		while(objectPartNode != NULL)
		{
			mObjectParts.push_back(ObjectPartSettings());
			if(mObjectParts[mObjectParts.size() - 1].deserialize(objectPartNode))
			{
				objectPartNode = objectPartNode->NextSiblingElement("ObjectPart");
			}
			else
				success = false;
		}

		return success;
	}

	btAlignedObjectArray<ObjectPartSettings> mObjectParts;
};

#endif //_STATIC_OBJECT_SETTINGS_H_