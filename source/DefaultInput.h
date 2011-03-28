#ifndef __DefaultInput_h__
#define __DefaultInput_h__

#include <Windows.h>

#include <list>

class DefaultInput
{
public:
	struct Key
	{
		int key;
		bool up;
	};

	struct Mouse
	{
		long x;
		long y;
	};

	Mouse mouse;
	std::list<Key> keys;

	DefaultInput();
	void flush();
	void message(MSG msg);
};

#endif