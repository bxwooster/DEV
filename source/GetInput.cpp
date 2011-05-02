#include "InputData.hpp"

#include <Windows.h>
#include <vector>

namespace Devora {

void GetInput(InputData& input)
{
	input.keys_up.clear();
	input.keys_down.clear();
	input.mouse.x = 0;
	input.mouse.y = 0;

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); // this calls window procs!

		if (WM_INPUT == msg.message)
		{
			UINT size;
			GetRawInputData((HRAWINPUT)msg.lParam, 
				RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                    
			std::vector<byte> bytes(size);
			GetRawInputData((HRAWINPUT)msg.lParam, 
				RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)bytes.data();

			if ( RIM_TYPEKEYBOARD == raw->header.dwType )
			{
				RAWKEYBOARD* data = &raw->data.keyboard;
				bool up = data->Flags & RI_KEY_BREAK;
				(up ? input.keys_up : input.keys_down).insert(data->VKey);
			}
			else if ( RIM_TYPEMOUSE == raw->header.dwType )
			{
				RAWMOUSE* data = &raw->data.mouse;
				input.mouse.x += data->lLastX;
				input.mouse.y += data->lLastY;
			}
		}
	}

	for(auto i = input.keys_down.begin(); i != input.keys_down.end(); i++)
	{
		input.keys_held.insert( (*i) );
	}

	for(auto i = input.keys_up.begin(); i != input.keys_up.end(); i++)
	{
		input.keys_held.erase( (*i) );
	}
}

} // namespace Devora