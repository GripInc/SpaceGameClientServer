#include "manager/InputHistoryManager.h"

#include "utils/StringUtils.h"
#include "manager/LoggerManager.h"

#include <assert.h>

namespace
{
	const std::string LOG_CLASS_TAG = "InputHistoryManager";
}

void InputHistoryManager::addInput(const RakNet::RakNetGUID& _id, const InputState& _newClientInput)
{
	ClientsInputMap::const_iterator foundTick = mLastInputReceivedFromClient.find(_id);
	if (foundTick == mLastInputReceivedFromClient.end() || (*foundTick).second.mTick < _newClientInput.mTick)
	{
		mLastInputReceivedFromClient[_id] = _newClientInput;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "Add input at tick " + StringUtils::toStr(_newClientInput.mTick), false);
	}
	else
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "Discard input at tick " + StringUtils::toStr(_newClientInput.mTick), false);
}
