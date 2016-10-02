#ifndef _NAMED_SETTINGS_H_
#define _NAMED_SETTINGS_H_

#include "utils/XMLHelper.h"
#include <tinyxml2.h>

class NamedSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= XMLHelper::getStringFrom(_node, "name", mName);

		return success;
	}

	const std::string& getName() const { return mName; }

protected:
	std::string mName;
};

#endif //_NAMED_SETTINGS_H_