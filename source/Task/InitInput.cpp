#include "InitInput.hpp"
#include "OK.hpp"
#include <Windows.h>

namespace DEV {

void InitInput::run()
{
	RAWINPUTDEVICE info[2];
	info[0].dwFlags = 0;
	info[0].hwndTarget = NULL;
	info[0].usUsagePage = 1;
	info[0].usUsage = 6; //Keyboard
    
	info[1].dwFlags = 0;
	info[1].hwndTarget = NULL;
	info[1].usUsagePage = 1;
	info[1].usUsage = 2; //Mouse
    
	OK( RegisterRawInputDevices( info, 2, sizeof(RAWINPUTDEVICE) ) );
}

} // namespace DEV