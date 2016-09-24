#ifndef _I_JSON_SERIALIZABLE_H_
#define _I_JSON_SERIALIZABLE_H_

#include <string>

#include "utils/json/json.h"

/**
* Export or import from a JSON value.
*/
class IJsonNodeSerializable
{
public:

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	virtual void serializeToJsonNode(Json::Value& _node) const = 0;

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	virtual bool deserializeFromJsonNode(const Json::Value& _node) = 0;

	/**
	* Virtual destructor
	*/
	virtual ~IJsonNodeSerializable() {}

protected:
	/**
	* Returns a node containing an array
	* @param _array array.
	* @param _node node to edit.
	*/
	template <typename T> static void serializeToJsonNode(const std::vector<T>& _array, Json::Value& _node)
	{
		unsigned int size = _array.size();
		_node.resize(size);
		for (unsigned int i = 0; i < size; i++)
			_node[i] = _array[i];
	}

	/**
	* Returns a node containing an associative array
	* @param _array associative array.
	* @param _node node to edit.
	*/
	//template <typename T> static Json::Value serializeToJsonNode(const std::map<std::string, T>& _array, Json::Value& _node)
	//{
	// assert(_node.isObject());
	// int size = _array.size();
	// std::map<std::string, T>::const_iterator it = _array.begin();
	// std::map<std::string, T>::const_iterator itEnd = _array.end();
	// for (; it != itEnd; ++it)
	// _node[(*it).first] = (*it).second;
	//}

	/**
	* Deserialize an array of int.
	* @param _node node.
	* @param _result builded int array (it will be cleared).
	* @return true if successfully finished.
	*/
	static bool deserializeFromJsonNode(const Json::Value& _node, std::vector<int>& _result)
	{
		if (!_node.isArray())
			return false;

		_result.assign(_node.size(), 0);
		unsigned int i = 0;
		for (Json::Value::const_iterator it = _node.begin(), itEnd = _node.end(); it != itEnd; ++it, i++)
		{
			const Json::Value& intNode = *it;
			if (intNode.isIntegral())
				_result[i] = intNode.asInt();
			else
				return false;
		}
		return true;
	}

	/**
	* Deserialize a string array.
	* @param _node node.
	* @param _result builded string array (it will be cleared).
	* @return true if successfully finished.
	*/
	static bool deserializeFromJsonNode(const Json::Value& _node, std::vector<std::string>& _result)
	{
		if (!_node.isArray())
			return false;

		_result.clear();
		_result.reserve(_node.size());
		for (Json::Value::const_iterator it = _node.begin(), itEnd = _node.end(); it != itEnd; ++it)
		{
			const Json::Value& stringNode = *it;
			if (stringNode.isString())
				_result.push_back(stringNode.asString());
			else
				return false;
		}
		return true;
	}

};

/**
* Export to JSON string format or import from JSON string.
*/
class IJsonSerializable : public IJsonNodeSerializable
{
public:

	/**
	* Export to JSON string format.
	* @return JSON string
	*/
	std::string serializeJson() const
	{
		Json::StyledWriter jsonWriter;
		Json::Value root;
		serializeToJsonNode(root);
		return jsonWriter.write(root);
	}

	/**
	* Load from JSON string.
	* @param _serializedObject JSON string
	* @return true if successfully finished, false otherwise.
	*/
	bool deserializeJson(const std::string& _serializedObject)
	{
		const char *begin = _serializedObject.c_str();
		const char *end = begin + _serializedObject.length();
		return deserializeJson(begin, end);
	}

	/**
	* Load from JSON string.
	* @param _beginString begin of JSON string.
	* @param _endString end of JSON string.
	* @return true if successfully finished, false otherwise.
	*/
	bool deserializeJson(const char* _beginString, const char* _endString)
	{
		Json::Value root;
		Json::Reader reader;

		if (!reader.parse(_beginString, _endString, root))
		{
			return false;
		}

		return deserializeFromJsonNode(root);
	}
};

/**
* Contain a boolean value.
*/
class BoolJsonSerializable : public IJsonSerializable
{
private:
	bool mValue;

public:
	/**
	* Constructor.
	*/
	BoolJsonSerializable(bool _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { _node = mValue; }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::booleanValue)
			mValue = _node.asBool();
		else
			return false;
		return true;
	}
};

/**
* Contain an int value.
*/
class IntJsonSerializable : public IJsonSerializable
{
private:
	int mValue;

public:
	/**
	* Constructor.
	*/
	IntJsonSerializable(int _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { _node = mValue; }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::intValue) // TODO: tester un retour de parsing
			mValue = _node.asInt();
		else
			return false;
		return true;
	}
};

/**
* Contain an unsigned int value.
*/
class UIntJsonSerializable : public IJsonSerializable
{
private:
	unsigned int mValue;

public:
	/**
	* Constructor.
	*/
	UIntJsonSerializable(unsigned int _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { _node = mValue; }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::uintValue) // TODO: tester un retour de parsing
			mValue = _node.asUInt();
		else
			return false;
		return true;
	}
};

/**
* Contain a double value.
*/
class DoubleJsonSerializable : public IJsonSerializable
{
private:
	double mValue;

public:
	/**
	* Constructor.
	*/
	DoubleJsonSerializable(double _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { _node = mValue; }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::realValue)
			mValue = _node.asDouble();
		else
			return false;
		return true;
	}
};

/**
* Contain a string value.
*/
class StringJsonSerializable : public IJsonSerializable
{
private:
	std::string mValue;

public:
	/**
	* Constructor.
	*/
	StringJsonSerializable(const std::string& _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { _node = mValue; }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::stringValue)
			mValue = _node.asString();
		else
			return false;
		return true;
	}
};

/**
* Contain an array of int values.
*/
class IntArrayJsonSerializable : public IJsonSerializable
{
private:
	std::vector<int> mValue;

public:
	/**
	* Constructor.
	*/
	IntArrayJsonSerializable(const std::vector<int>& _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { IJsonNodeSerializable::serializeToJsonNode(mValue, _node); }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::arrayValue)
			IJsonNodeSerializable::deserializeFromJsonNode(_node, mValue);
		else
			return false;
		return true;
	}
};

/**
* Contain a string array.
*/
class StringArrayJsonSerializable : public IJsonSerializable
{
private:
	std::vector<std::string> mValue;

public:
	/**
	* Constructor.
	*/
	StringArrayJsonSerializable(const std::vector<std::string>& _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const { IJsonNodeSerializable::serializeToJsonNode(mValue, _node); }

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::arrayValue)
			IJsonNodeSerializable::deserializeFromJsonNode(_node, mValue);
		else
			return false;
		return true;
	}
};

/**
* Contain an array of serializable objects.
*/
template<typename T>
class ArrayJsonSerializable : public IJsonSerializable
{
private:
	std::vector<T> mValue;

public:
	/**
	* Empty constructor.
	*/
	ArrayJsonSerializable() {}

	/**
	* Constructor with serialized data.
	* @param _serializedData serialized data.
	*/
	ArrayJsonSerializable(const std::string& _serializedData) { deserializeJson(_serializedData); }

	/**
	* Constructor.
	*/
	ArrayJsonSerializable(const std::vector<T>& _value) : mValue(_value) {}

	/**
	* Export to node.
	* @param _node node to edit.
	*/
	void serializeToJsonNode(Json::Value& _node) const
	{
		unsigned int size = mValue.size();
		_node.resize(size);
		for (unsigned int i = 0; i < size; i++)
			mValue[i].serializeToJsonNode(_node[i]);
	}

	/**
	* Load from node.
	* @param _node node.
	* @return true if successfully finished.
	*/
	bool deserializeFromJsonNode(const Json::Value& _node) {
		if (_node.type() == Json::arrayValue)
		{
			mValue.assign(_node.size(), T());

			Json::Value::const_iterator it = _node.begin();
			Json::Value::const_iterator itEnd = _node.end();
			unsigned int i = 0;
			for(; it != itEnd; ++it, i++)
			{
				const Json::Value& itemNode = *it;
				if (!mValue[i].deserializeFromJsonNode(itemNode))
					return false;
			}
		}
		else
			return false;
		return true;
	}

	const std::vector<T>& getValue() const { return mValue; }
};

#endif // _I_JSON_SERIALIZABLE_H_
