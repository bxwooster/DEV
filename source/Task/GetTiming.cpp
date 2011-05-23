#include "GetTiming.hpp"
#include <Windows.h>

namespace DEV {

void GetTiming::run()
{
	LARGE_INTEGER large;
	QueryPerformanceCounter( &large );
	timing.delta = float(double(large.QuadPart - timing.counter) / timing.frequency);
	timing.counter = large.QuadPart;
}

} // namespace DEV