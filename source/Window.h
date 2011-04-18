#ifndef __Window_h__
#define __Window_h__

#include "Windows.h"

class Window
{
public:
	Window(int, int);

	HWND handle;
};

#endif