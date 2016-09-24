#ifndef _POST_COMBUSTIONS_SETTINGS_H_
#define _POST_COMBUSTIONS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class PostCombustionSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "powerMultiplier", mPowerMultiplier);
		XMLHelper::getFloatFrom(_node, "maxSpeedMultiplier", mMaxSpeedMultiplier);
		XMLHelper::getFloatFrom(_node, "consumption", mConsumption);

		std::string tempType;
		XMLHelper::getStringFrom(_node, "type", tempType);
		mType = Types::getEngineType(tempType);

		return success;
	}

	float mConsumption = 0.f;
	float mPowerMultiplier = 0.f;
	float mMaxSpeedMultiplier = 0.f;
	Types::EngineType mType = Types::ET_CHIMICAL;
};

class PostCombustionsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* postCombustionNode = _node->FirstChildElement("PostCombustion");
		while(postCombustionNode != NULL)
		{
			mPostCombustions.push_back(PostCombustionSettings());
			if(mPostCombustions[mPostCombustions.size() - 1].deserialize(postCombustionNode))
				postCombustionNode = postCombustionNode->NextSiblingElement("PostCombustion");
			else
				return false;
		}

		return true;
	}

	const PostCombustionSettings* getPostCombustionSettings(const std::string& _postCombustion) const
	{
		for(int i = 0; i < mPostCombustions.size(); ++i)
			if(mPostCombustions[i].mName.compare(_postCombustion) == 0)
				return &mPostCombustions[i];

		return NULL;
	}

	btAlignedObjectArray<PostCombustionSettings> mPostCombustions;
};

#endif //_POST_COMBUSTIONS_SETTINGS_H_