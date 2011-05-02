#include "TimingData.hpp"

#include <Windows.h>

namespace Devora {

void InitTiming(TimingData& timing)
{
	LARGE_INTEGER large;
	QueryPerformanceFrequency( &large );
	timing.frequency = double(large.QuadPart);
	QueryPerformanceCounter( &large );
	timing.counter = large.QuadPart;
}

} // namespace Devora