#ifndef _TIME_PROVIDER_H_
#define _TIME_PROVIDER_H_

#include <chrono>

class TimeProvider
{
public:
	///Singleton
	static TimeProvider& getInstance();



protected:
	///Singleton
	static TimeProvider* mInstance;
	TimeProvider() {}
};

#endif //_TIME_PROVIDER_H_