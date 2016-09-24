#ifndef _JSON_HELPERS_H_
#define _JSON_HELPERS_H_

#include <string>

#include "utils/json/json.h"
#include "IJsonSerializable.h"

/**
* Some helpers method to use with Json
*/
class JsonHelpers
{
public:
	/**
	* Try to get the node value as an 32 bits integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readInt32(const Json::Value& _rootNode, const std::string& _subNodeName, int& _value, int _defaultValue = 0 )
	{
		const Json::Value& node = _rootNode[_subNodeName];

		return readInt32(node,_value,_defaultValue);
	}



	/**
	* Try to get the node value as an 32 bits integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _node Json node to read
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readInt32(const Json::Value& _node, int& _value, int _defaultValue = 0)
	{
		if(!_node.isNull()
			&& _node.isConvertibleTo(Json::intValue)
			&& _node.asInt64() <= Json::Value::maxInt
			&& _node.asInt64() >= Json::Value::minInt)
		{
			_value = _node.asInt();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	static bool readInt32(const Json::Value& _node, int& _value)
	{
		return readInt32(_node,_value,0);
	}

	/**
	* Try to get the node value as an 32 bits unsigned integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _node Json node to read
	* @param _value Reference to an unsigned int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readUInt32(const Json::Value& _node, unsigned int& _value, unsigned int _defaultValue = 0)
	{
		if (!_node.isNull()
			&& _node.isConvertibleTo(Json::uintValue)
			&& _node.asInt64() <= Json::Value::maxUInt)
		{
			_value = _node.asUInt();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	/**
	* Try to get the node value as an 32 bits unsigned integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readUInt32(const Json::Value& _rootNode, const std::string& _subNodeName, unsigned int& _value, unsigned int _defaultValue = 0)
	{
		const Json::Value& node = _rootNode[_subNodeName];
		return readUInt32(node, _value, _defaultValue);
	}

	/**
	* Try to get the node value as a short.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readShort(const Json::Value& _rootNode, const std::string& _subNodeName, short& _value, short _defaultValue = 0)
	{
		const Json::Value& node = _rootNode[_subNodeName];

		if(!node.isNull()
			&& node.isConvertibleTo(Json::intValue)
			&& node.asInt64() <= short((unsigned short)(-1)/2)
			&& node.asInt64() >= short(~((unsigned short)(-1)/2)))
		{
			_value = (short)node.asInt();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	/**
	* Try to get the node value as an 64 bits integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _node Json node to read
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readInt64(const Json::Value& _node, long long int& _value, long long int _defaultValue = 0)
	{
		if(_node.isInt() || (_node.isUInt() && _node.asUInt64() <= Json::UInt64(Json::Value::maxInt64)))
		{
			_value = _node.asInt64();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	/**
	* Try to get the node value as an 64 bits integer.
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readInt64(const Json::Value& _rootNode, const std::string& _subNodeName, long long int& _value, long long int _defaultValue = 0)
	{
		const Json::Value& node = _rootNode[_subNodeName];
		return readInt64(node,_value,_defaultValue);
	}


	/**
	* Try to get the node value as string
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readString(const Json::Value& _rootNode, const std::string& _subNodeName, std::string& _value, const std::string& _defaultValue = "")
	{
		const Json::Value& node = _rootNode[_subNodeName];

		if(node.isString())
		{
			_value = node.asString();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	/**
	* Try to get the node value as boolean
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readBool(const Json::Value& _rootNode, const std::string& _subNodeName, bool& _value, bool _defaultValue = false)
	{
		const Json::Value& node = _rootNode[_subNodeName];

		if(node.isBool())
		{
			_value = node.asBool();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	/**
	* Try to get the node value as float
	* If success Set the _value ref to the result and returns true
	* Else Set the _value ref to defaultValue and returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _value Reference to an int value that will be set
	* @param _defaultValue Value that will be set in error case (0 by default)
	* @return true in case of success, else false
	*/
	static inline bool readFloat(const Json::Value& _rootNode, const std::string& _subNodeName, float& _value, float _defaultValue = false)
	{
		const Json::Value& node = _rootNode[_subNodeName];

		if(node.isNumeric())
		{
			_value = node.asFloat();
			return true;
		}
		else
		{
			_value = _defaultValue;
			return false;
		}
	}

	
	/**
	* Read a json array of into a generic list of T. User must provide a pointer to function to deserialize the type
	* If success fills the list an returns true
	* Else returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _toArray Array/list that will be filled
	* @param _func Pointer to function with signature : bool FUNC(const Json::Value& , T&)
	* @return true in case of success, else false
	*/
	template<template <class,class> class ArrayT, class T, class T2>
	static inline bool readGenericArray(const Json::Value& _rootNode, const std::string& _subNodeName, ArrayT<T, std::allocator<T> >& _toArray, bool (*_func)(const Json::Value& , T2&) )
	{
		const Json::Value& node = _rootNode[_subNodeName];

		if(node.isArray())
		{
			size_t arraySize = node.size();
			for(unsigned int i = 0; i < arraySize; ++i)
			{
				T obj;
				_func(node[i],obj);
				_toArray.push_back(obj);
			}

			return true;
		}else
		{
			return false;
		}
	}




	static bool readIJsonDeserializable(const Json::Value& _node, IJsonSerializable& _value)
	{
		return _value.deserializeFromJsonNode(_node);
	}

	/**
	* Read a json array of into a generic list of T. T must be a IJsonDeserializable
	* If success fills the list an returns true
	* Else returns false
	* @param _rootNode Json node that should contains the subnode
	* @param _subnodeName Name of the subnode
	* @param _toArray Array/list that will be filled
	* @return true in case of success, else false
	*/
	template<template <class,class> class ArrayT, class T>
	static inline bool readGenericArrayOfObjects(const Json::Value& _rootNode, const std::string& _subNodeName, ArrayT<T, std::allocator<T> >& _toArray)
	{
		return readGenericArray(_rootNode,_subNodeName,_toArray,&readIJsonDeserializable);
	}


};

#endif //_JSON_HELPERS_H_