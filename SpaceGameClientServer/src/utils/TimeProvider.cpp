#include "utils/TimeProvider.h"

TimeProvider* TimeProvider::mInstance;

TimeProvider& TimeProvider::getInstance()
{
	if (!mInstance)
		mInstance = new TimeProvider();

	return *mInstance;
}