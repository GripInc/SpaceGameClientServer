#ifndef _ACCUMULATORS_SETTINGS_H_
#define _ACCUMULATORS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"

class AccumulatorSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		XMLHelper::getFloatFrom(_node, "generation", mCapacity);

		return success;
	}

	float mCapacity;
};

class AccumulatorsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* accumulatorNode = _node->FirstChildElement("Accumulator");
		while(accumulatorNode != NULL)
		{
			mAccumulators.push_back(AccumulatorSettings());
			if(mAccumulators[mAccumulators.size() - 1].deserialize(accumulatorNode))
				accumulatorNode = accumulatorNode->NextSiblingElement("Accumulator");
			else
				return false;
		}

		return true;
	}

	const AccumulatorSettings* getAccumulatorSettings(const std::string& _accumulator) const
	{
		for(int i = 0; i < mAccumulators.size(); ++i)
			if(mAccumulators[i].mName.compare(_accumulator) == 0)
				return &mAccumulators[i];

		return NULL;
	}

	btAlignedObjectArray<AccumulatorSettings> mAccumulators;
};

#endif //_ACCUMULATORS_SETTINGS_H_