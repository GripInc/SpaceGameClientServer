#ifndef _CLIENTS_INPUT_MAP_H_
#define _CLIENTS_INPUT_MAP_H_

#include <string>
#include <map>

#include "RakNetTypes.h"
#include "BitStream.h"

#include "utils/StringUtils.h"

#include "model/InputState.h"

class ClientsInputMap : public std::map<RakNet::RakNetGUID, InputState>
{
public:
	void serialize(RakNet::BitStream& _stream)
	{
		_stream.Write(size());
		for (std::map<RakNet::RakNetGUID, InputState>::const_reference pair : *this)
		{
			_stream.Write(pair.first);
			_stream.Write(pair.second);
		}
	}

	void deserialize(RakNet::BitStream& _stream)
	{
		this->clear();

		size_type size;
		_stream.Read(size);

		RakNet::RakNetGUID UID;
		InputState inputState;
		for (size_type i = 0; i < size; ++i)
		{
			_stream.Read(UID);
			_stream.Read(inputState);
			(*this)[UID] = inputState;
		}
	}

	std::string getDebugString() const
	{
		std::string result;
		for (std::map<RakNet::RakNetGUID, InputState>::const_reference pair : *this)
		{
			result += "RakNetGUID: " + StringUtils::toStr(pair.first.ToString());
			result += "\nInput: " + pair.second.getDebugString();
		}

		return result;
	}
};

#endif //_CLIENTS_INPUT_MAP_H_
