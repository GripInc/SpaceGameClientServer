/*
* StringUtils.cpp
*
* Created by P.G. on 18/04/11.
* Copyright 2011 Kobojo. All rights reserved.
*
*/

#include "utils/StringUtils.h"

//#include "md5/md5.h"

void StringUtils::replaceOnce(std::string& _value, 
	const std::string& _search, 
	const std::string& _replace)
{
	const int pos = _value.find(_search);
	if (pos == -1) return;
	_value.replace(pos, _search.size(), _replace);
}

void StringUtils::replaceAll(std::string& _value, 
	const std::string& _search, 
	const std::string& _replace)
{
	std::string::size_type next;

	for(next = _value.find(_search);		// Try and find the first match
		next != std::string::npos;			// next is npos if nothing was found
		next = _value.find(_search, next)	// search for the next match starting after
											// the last match that was found.
		)
	{
		// Inside the loop. So we found a match.
		_value.replace(next, _search.length(), _replace); // Do the replacement.
		next += _replace.length();		// Move to just after the replace
										// This is the point were we start
										// the next search from. 
	}
}
/*
void StringUtils::transform(std::string& _value, const char* _regexStr, TranformFunc _func)
{
	struct slre regex;

	if (!slre_compile(&regex, _regexStr))
	{
		LOG_E(NULL, "Error compiling RE: %s\n", regex.err_str);
	}
	std::string tmp = _value;
	_value.clear();
	transformRecursively(tmp.c_str(), tmp.length(), regex, _func, _value);
}*/

std::string StringUtils::toLowerCase(const std::string& _str)
{
	std::string result = _str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}


std::string StringUtils::toUpperCase(const std::string& _str)
{
	std::string result = _str;
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}
/*
void StringUtils::transformRecursively(const char* _str, size_t _len, struct slre& _regex, TranformFunc _func, std::string& _out)
{
	struct cap match[10 + 1];

	if (strlen(_str) == 0) { return; }
	
	if (slre_match(&_regex, _str, _len, match))
	{
		if (_str != match[1].ptr)
		{
			// Add line before tag
			_out += std::string(_str, match[0].ptr - _str);
		}

		// Apply function
		_out += _func(std::string(match[1].ptr, match[1].len));

		// Parse line after tag
		const char* newBegin = match[1].ptr + match[1].len;

		if (newBegin < (_str + _len)) {
			transformRecursively(newBegin, ((_str + _len) - newBegin), _regex, _func, _out);
		}
	}
	else
	{
		_out += std::string(_str, _len);
	}


}*/

/**
* @return The md5 hash of a string
*/
/*
std::string StringUtils::MD5Hash(const std::string& _str)
{
	
	std::stringstream ss;
	ss << _str;

	MD5 digest;
	digest.update(ss);  
	digest.finalize();

	return std::string(digest.hex_digest());
}*/

std::vector<std::string> & StringUtils::split(const std::string &_stringToSplit, char _delim, std::vector<std::string> &_resultVect) {
    std::stringstream ss(_stringToSplit);
    std::string item;
    while (std::getline(ss, item, _delim)) {
        _resultVect.push_back(item);
    }
    return _resultVect;
}

std::vector<std::string> StringUtils::split(const std::string &_stringToSplit, char _delim) {
	std::vector<std::string> elems;
	split(_stringToSplit, _delim, elems);
	return elems;
}