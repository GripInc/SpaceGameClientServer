#ifndef _INPUT_HISTORY_MANAGER_H_
#define _INPUT_HISTORY_MANAGER_H_

#include <map>
#include <vector>

#include "RakNetTypes.h"
#include "SpaceGameTypes.h"
#include "model/InputState.h"

typedef std::map<RakNet::RakNetGUID, InputState> ClientsInputMap;

class InputHistoryManager
{
public:
	//Add one tick input
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _newClientInput);

	const ClientsInputMap& getLastInputReceivedByClient() const { return mLastInputReceivedByClient; }

protected:
	std::map<RakNet::RakNetGUID, SectorTick> mLastTickInputReceivedByClient;

	ClientsInputMap mLastInputReceivedByClient;
};

#endif //_INPUT_HISTORY_MANAGER_H_
