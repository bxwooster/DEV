#define NOMINMAX

#include <iostream>
#include <Windows.h>

#include "DefaultInput.h"
#include "Recorder.h"
#include "PixelShaderTracy.h"

const bool read = true;

void run()
{
	PixelShaderTracy::Settings settings = {480, 480, 0.25};
	PixelShaderTracy tracy(settings);

	DefaultInput input;
	Recorder recorder("C:\\replay");

	if(!read)
		while(true)
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); // this calls window procs!

				input.message(msg);
			}

			recorder.write(input);
			tracy.step(input);
			input.flush();
		}
	else
		while(true)
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg); // this calls window procs!
			}
		
			recorder.read(input);
			tracy.step(input);
			input.flush();
		}
}

int main()
{
	try
	{
		run();
	}
	//catch(std::exception exception)
	//{
	//	using namespace std;
	//	cout << "Exception:" << endl << exception.what() << endl;
	//	getchar();
	//	return -1;
	//}
	catch(PixelShaderTracy::Quit) 
	{
		return 0;
	}
}