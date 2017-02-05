#ifndef _INPUT_HISTORY_MANAGER_H_
#define _INPUT_HISTORY_MANAGER_H_

#include "RakNetTypes.h"
#include "SpaceGameTypes.h"
#include "model/InputState.h"
#include "model/ClientsInputMap.h"

#include <list>

class InputHistoryManager
{
public:

	const std::map<RakNet::RakNetGUID, SectorTick>& getLastInputReceivedFromClient() const { return mLastInputReceivedFromClient; }

	//Return true if first input ever
	bool addInputs(const RakNet::RakNetGUID& _id, const std::list<InputState>& _clientInputs);

	void getInputForTick(const RakNet::RakNetGUID& _id, SectorTick _sectorTick, InputState& _inputState) const;

	void incrementNextInputToUse();

	void setFirstInputToUse(RakNet::RakNetGUID _id, SectorTick _sectorTick);

	const std::map<RakNet::RakNetGUID, SectorTick>& getNextInputToUse() const { return mNextInputToUse; }

protected:
	std::map<RakNet::RakNetGUID, SectorTick> mLastInputReceivedFromClient;

	std::map<RakNet::RakNetGUID, std::list<InputState> > mClientsInputs;

	std::map<RakNet::RakNetGUID, SectorTick> mNextInputToUse; //Should be ++ at end of sector update
};

#endif //_INPUT_HISTORY_MANAGER_H_
