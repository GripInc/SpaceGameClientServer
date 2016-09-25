#include "network/NetworkService.h"

const char NetworkService::LEVEL_1_CHANNEL = 1;

void NetworkService::processNetworkBuffer()
{
	mNetworkLayer->getNetworkData();
}
