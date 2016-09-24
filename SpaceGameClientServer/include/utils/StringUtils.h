/*
* StringUtils.h
*
* Created by P.G. on 18/04/11.
* Copyright 2011 Kobojo. All rights reserved.
*
*/

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
//#include <sigc++/slot.h>
//#include <regex/slre.h>
#include <vector>
//#include "core/Logger.h"

class StringUtils
{
public:
	//typedef sigc::slot<std::string, const std::string&> TranformFunc;
public:
	static void replaceOnce(std::string& _value, 
		const std::string& _search, 
		const std::string& _replace);

	static void replaceAll(std::string& _value, 
		const std::string& _search, 
		const std::string& _replace);

	//static void transform(std::string& _value, const char* _regexStr, TranformFunc _func);

	template<typename T>
	static std::string toStr(const T& _value) {
		std::ostringstream oss;
		oss << _value;
		return oss.str();
	}

	/**
	* Try to convert a string into the T type
	* @param _result the result of the conversion
	* @param _str the string to convert
	* @param _toBase the output transformation to a base function
	* @return true if conversion was successfull, false otherwise.
	* @see std::ios_base
	*/
	template<typename T>
	static bool fromString(T& _result, const std::string& _str, std::ios_base& (*_toBase)(std::ios_base&) = std::dec)
	{
		std::istringstream iss(_str);
		return !(iss >> _toBase >> _result).fail();
	}


	static bool endsWith(const std::string& _str, const std::string& _ending) {
		if (_str.length() >= _ending.length()) {
			return (0 == _str.compare (_str.length() - _ending.length(), _ending.length(), _ending));
		} else {
			return false;
		}
	}

	static bool beginsWith(const std::string& _str, const std::string& _begins) {
		if (_str.length() >= _begins.length()) {
			return (0 == _str.compare (0, _begins.length(), _begins));
		} else {
			return false;
		}
	}

	static std::string toLowerCase(const std::string& _str);
	static std::string toUpperCase(const std::string& _str);

	/**
	* Concatenate a number to a string , and fill this number with 0 
	* example : concatFilledNumber("test", 42 , 3) returns  "test042"
	* @param _str :  string to concatenate
	* @param _number : number to concatenate
	* @param _numberSize : nulmber of 0 to fill 
	*/
	static std::string concatFilledNumber(const std::string& _str, int number, int numberSize = 2)
	{
		std::stringstream numberStr;
		numberStr.width(numberSize);
		numberStr.fill('0');
		numberStr << number;
		return _str + numberStr.str();
	}

	/**
	* Check type of a string.
	* @param _str string to check type.
	* @return true if string matches type, false otherwise.
	*/
	template<typename T>
	static bool isOfType(const std::string& _str)
	{
		std::istringstream iss(_str);
		T tmp;
		return (iss >> tmp) && iss.eof();
	}

	/**
	* @return The md5 hash of a string
	*/
	static std::string MD5Hash(const std::string& _str);

	/**
	* Split the string into a new vector
	*/
	static std::vector<std::string> split(const std::string & _stringToSplit, char _delim);
	/**
	* Split the string into a  pre-construted vector
	*/
	static std::vector<std::string> &split(const std::string &_stringToSplit, char _delim, std::vector<std::string> &_resultVect);

protected:
	//static void transformRecursively(const char* _str, size_t _len, struct slre& _regex, TranformFunc _func, std::string& _out);
};

#endif