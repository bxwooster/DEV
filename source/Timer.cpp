#include "Timer.h"

Timer::Timer(bool reset_)
{
	LARGE_INTEGER large;
	reset = reset_;
	QueryPerformanceFrequency( &large );
	frequency = double(large.QuadPart);
	QueryPerformanceCounter( &large );
	last = large.QuadPart;
}

double Timer::elapsed()
{
	LARGE_INTEGER large;
	QueryPerformanceCounter( &large );
	double result = double(large.QuadPart - last) / frequency;
	if (reset) last = large.QuadPart;
	return result;
}