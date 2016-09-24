#include "utils/XMLHelper.h"

#include "utils/StringUtils.h"
#include <tinyxml2.h>

bool XMLHelper::getFloatFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, float& _out, float _defaultValue)
{
	const char* temp = _node->Attribute(_attributeName.c_str());
	if(temp)
	{
		StringUtils::fromString(_out, temp, std::defaultfloat);
		return true;
	}
	else
	{
		_out = _defaultValue;
		return false;
	}
}
bool XMLHelper::getIntFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, int& _out, int _defaultValue)
{
	const char* temp = _node->Attribute(_attributeName.c_str());
	if(temp)
	{
		StringUtils::fromString(_out, temp, std::dec);
		return true;
	}
	else
	{
		_out = _defaultValue;
		return false;
	}
}
bool XMLHelper::getUnsignedIntFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, unsigned int& _out, unsigned int _defaultValue)
{
	const char* temp = _node->Attribute(_attributeName.c_str());
	if(temp)
	{
		StringUtils::fromString(_out, temp, std::dec);
		return true;
	}
	else
	{
		_out = _defaultValue;
		return false;
	}
}
bool XMLHelper::getStringFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, std::string& _out, std::string _defaultValue)
{
	const char* temp = _node->Attribute(_attributeName.c_str());
	if(temp)
	{
		StringUtils::fromString(_out, temp);
		return true;
	}
	else
	{
		_out = _defaultValue;
		return false;
	}
}

bool XMLHelper::getBoolFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, bool& _out, bool _defaultValue)
{
	const char* temp = _node->Attribute(_attributeName.c_str());
	if(temp)
	{
		std::string tempValue = "";
		StringUtils::fromString(tempValue, temp);
		if(tempValue == "true")
			_out = true;
		else if(tempValue == "false")
			_out = false;
		else
		{
			_out = _defaultValue;
			return false;
		}

		return true;
	}
	else
	{
		_out = _defaultValue;
		return false;
	}
}