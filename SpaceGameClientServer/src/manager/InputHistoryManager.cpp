#include "manager/InputHistoryManager.h"

#include "utils/StringUtils.h"
#include "manager/LoggerManager.h"

#include <assert.h>

namespace
{
	const std::string LOG_CLASS_TAG = "InputHistoryManager";
}

void InputHistoryManager::addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _newClientInput)
{
	std::map<RakNet::RakNetGUID, SectorTick>::const_iterator foundTick = mLastTickInputReceivedByClient.find(_id);
	if ((foundTick != mLastTickInputReceivedByClient.end() && (*foundTick).second < _tick) || foundTick == mLastTickInputReceivedByClient.end())
	{
		mLastInputReceivedByClient[_id] = _newClientInput;
		mLastTickInputReceivedByClient[_id] = _tick;
	}
}
