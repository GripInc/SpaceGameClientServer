#ifndef _I_NETWORK_SERVICE_H_
#define _I_NETWORK_SERVICE_H_

namespace RakNet
{
	struct Packet;
}

class INetworkService
{
public:
	virtual void handlePacket(RakNet::Packet* _packet) = 0;
};

#endif //_I_NETWORK_SERVICE_H_