#ifndef __Timer_h__
#define __Timer_h__

#include "Windows.h"

namespace Devora {

class Timer
{
	__int64 last;
	double frequency;
	bool reset;

public:
	Timer(bool reset_ = true);
	double elapsed();
};

} // namespace Devora
#endif