#ifndef __Window_h__
#define __Window_h__

#include "Windows.h"

namespace Devora {

class Window
{
public:
	Window(int, int);

	HWND handle;
};

} // namespace Devora
#endif