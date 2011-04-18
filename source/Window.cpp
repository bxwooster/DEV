#include "Freddy.h"
#include "Window.h"

#include "ok.h"

LRESULT CALLBACK WindowProc(HWND handle, UINT msg, WPARAM w, LPARAM l)
{
    switch (msg) 
    { 
		case WM_SETFOCUS:
			ShowCursor(false);
			RECT rect;
			GetWindowRect( handle, &rect );
			ClipCursor( &rect );
			return 0;
		case WM_KILLFOCUS:
			ShowCursor(true);
			ClipCursor( NULL );
			return 0;
		case WM_CLOSE:
			throw Freddy::Quit();
    } 
    return DefWindowProc(handle, msg, w, l);
}

Window::Window(int width, int height)
{
	WNDCLASSEX window_class;
	ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
	window_class.lpszClassName = "Freddy::window";                        
	window_class.cbSize = sizeof( WNDCLASSEX );      
	window_class.lpfnWndProc = WindowProc;  
	RegisterClassEx( &window_class );

	OK_P( handle = CreateWindowEx( 0, window_class.lpszClassName, "Freddy",
		WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, width, height, NULL, NULL, NULL, 0 ) );
}