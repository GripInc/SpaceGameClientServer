#ifndef _INPUT_HISTORY_MANAGER_H_
#define _INPUT_HISTORY_MANAGER_H_

#include "RakNetTypes.h"
#include "SpaceGameTypes.h"
#include "model/InputState.h"
#include "model/ClientsInputMap.h"

class InputHistoryManager
{
public:
	//Add one tick input
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _newClientInput);

	const ClientsInputMap& getLastInputReceivedByClient() const { return mLastInputReceivedByClient; }
	const std::map<RakNet::RakNetGUID, SectorTick>& getLastTickInputReceivedByClient() const { return mLastTickInputReceivedByClient; }

protected:
	std::map<RakNet::RakNetGUID, SectorTick> mLastTickInputReceivedByClient;

	ClientsInputMap mLastInputReceivedByClient;
};

#endif //_INPUT_HISTORY_MANAGER_H_
