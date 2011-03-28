#define NOMINMAX

#include <iostream>
#include <Windows.h>

#include "DefaultInput.h"
#include "Recorder.h"
#include "PixelShaderTracy.h"

void run()
{
	const bool write = true;

	PixelShaderTracy::Settings settings = {480, 480, 0.25};
	PixelShaderTracy tracy(settings);

	DefaultInput input;
	Recorder recorder("replay");

	while(true)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); // this calls window procs!

			if (write) input.message(msg);
		}

		write ? recorder.write(input) : recorder.read(input);
		tracy.step(input);
		if (write) input.flush();
	}
}

int main()
{
	try
	{
		run();
	}
	catch(std::exception exception)
	{
		using namespace std;
		//cout << "Exception:" << endl << exception.what() << endl;
		//getchar();
		MessageBox( GetConsoleWindow(), exception.what(), "Exception!", 0);
		return -1;
	}
	catch(PixelShaderTracy::Quit) 
	{
		return 0;
	}
}