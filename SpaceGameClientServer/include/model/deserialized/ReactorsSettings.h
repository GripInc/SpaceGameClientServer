#ifndef _REACTORS_SETTINGS_H_
#define _REACTORS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class ReactorSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		std::string tempType;
		XMLHelper::getStringFrom(_node, "type", tempType);
		mType = Types::getReactorType(tempType);

		XMLHelper::getFloatFrom(_node, "generation", mGeneration);

		return success;
	}

	float mGeneration = 0.f;
	Types::ReactorType mType = Types::RT_CHIMICAL;
};

class ReactorsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* reactorNode = _node->FirstChildElement("Reactor");
		while(reactorNode != NULL)
		{
			mReactors.push_back(ReactorSettings());
			if(mReactors[mReactors.size() - 1].deserialize(reactorNode))
				reactorNode = reactorNode->NextSiblingElement("Reactor");
			else
				return false;
		}

		return true;
	}

	const ReactorSettings* getReactorSettings(const std::string& _reactor) const
	{
		for(int i = 0; i < mReactors.size(); ++i)
			if(mReactors[i].mName.compare(_reactor) == 0)
				return &mReactors[i];

		return NULL;
	}

	btAlignedObjectArray<ReactorSettings> mReactors;
};

#endif //_REACTORS_SETTINGS_H_