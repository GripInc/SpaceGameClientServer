#ifndef _I_CONNECTION_READY_LISTENER_H_
#define _I_CONNECTION_READY_LISTENER_H_

class IConnectionReadyListener
{
public:
	virtual void notifyIsConnected(bool _value) = 0;
};

#endif //_I_CONNECTION_READY_LISTENER_H_