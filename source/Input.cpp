#include "Ok.h"
#include "Input.h"

typedef unsigned int uint;

using std::fstream;

DefaultInput::DefaultInput()
{
	flush();

	RAWINPUTDEVICE info[2];
	info[0].dwFlags = 0;
	info[0].hwndTarget = NULL;
	info[0].usUsagePage = 1;
	info[0].usUsage = 6; //Keyboard
    
	info[1].dwFlags = 0;
	info[1].hwndTarget = NULL;
	info[1].usUsagePage = 1;
	info[1].usUsage = 2; //Mouse
    
	OK_P( RegisterRawInputDevices( info, 2, sizeof(RAWINPUTDEVICE) ) );
}

void DefaultInput::flush()
{
	mouse.x = mouse.y = 0;
	keys.clear();
}

void DefaultInput::message(MSG msg)
{
	if (WM_INPUT == msg.message)
	{
		uint size;
		GetRawInputData((HRAWINPUT)msg.lParam, 
			RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                    
		byte* bytes = new byte[size];
		GetRawInputData((HRAWINPUT)msg.lParam, 
			RID_INPUT, bytes, &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)bytes;

		if ( RIM_TYPEKEYBOARD == raw->header.dwType )
		{
			RAWKEYBOARD* data = &raw->data.keyboard;
			bool up = data->Flags & RI_KEY_BREAK;
			Key key = { data->VKey, up };
			keys.push_back(key);
		}
		else if ( RIM_TYPEMOUSE == raw->header.dwType )
		{
			RAWMOUSE* data = &raw->data.mouse;
			mouse.x += data->lLastX;
			mouse.y += data->lLastY;
		}

		delete[] bytes;
	}
}

InputPlayer::InputPlayer(std::string path)
{
	file.exceptions( fstream::failbit | fstream::badbit | fstream::eofbit );
	file.open(path.c_str(), fstream::in | fstream::binary);
}

InputRecorder::InputRecorder(std::string path)
{
	file.exceptions( fstream::failbit | fstream::badbit | fstream::eofbit );
	file.open(path.c_str(), fstream::out | fstream::trunc | fstream::binary);
}

void InputRecorder::write(Input& input)
{
	file.write( (char*)&input.mouse, sizeof(input.mouse));

	int n = input.keys.size();
	file.write( (char*)&n, sizeof(n) );

	for(int i = 0; i < n; i++)
	{
		file.write( (char*)&input.keys[i], sizeof(input.keys[i]));
	}
}

void InputPlayer::read(Input& input)
{
	file.read( (char*)&input.mouse, sizeof(input.mouse));

	int n;
	file.read( (char*)&n, sizeof(n) );
	input.keys.resize(n);

	for(int i = 0; i < n; i++)
	{
		file.read( (char*)&input.keys[i], sizeof(input.keys[i]));
	}
}