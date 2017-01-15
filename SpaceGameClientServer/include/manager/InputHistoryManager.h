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
	void addInput(const RakNet::RakNetGUID& _id, const InputState& _newClientInput);

	const ClientsInputMap& getLastInputReceivedFromClient() const { return mLastInputReceivedFromClient; }

protected:
	ClientsInputMap mLastInputReceivedFromClient;
};

#endif //_INPUT_HISTORY_MANAGER_H_
