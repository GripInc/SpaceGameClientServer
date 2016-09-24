#ifndef _XML_HELPER_H_
#define _XML_HELPER_H_

#include <string>

namespace tinyxml2
{
	class XMLElement;
}

class XMLHelper
{
public:
	static bool getFloatFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, float& _out, float _defaultValue = 0.f);
	static bool getIntFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, int& _out, int _defaultValue = 0);
	static bool getUnsignedIntFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, unsigned int& _out, unsigned int _defaultValue = 0);
	static bool getStringFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, std::string& _out, std::string _defaultValue = "");
	static bool getBoolFrom(tinyxml2::XMLElement* _node, const std::string& _attributeName, bool& _out, bool _defaultValue = false);
};

#endif //_XML_HELPER_H_