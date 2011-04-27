#define NOMINMAX

#include <iostream>
#include <memory>
#include <Windows.h>

#include "Window.h"
#include "Input.h"
#include "Devora.h"
#include "Timer.h"

using std::auto_ptr;

void run()
{
	const bool write = true;

	Devora::Settings settings = {960, 960, 0.25, 5.0};
	auto_ptr<Devora> Devora( new Devora(settings) );

	InputParser parser;
	//InputRecorder recorder("replay");
	//InputPlayer player("replay");

	Timer timer;

	while(true)
	{
		Input input;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); // this calls window procs!

			if (write) parser.parse(msg, input);
		}

		//write ? recorder.write(input) : player.read(input);

		Devora->step(input, timer.elapsed() );
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
		MessageBox( GetConsoleWindow(), exception.what(), "Exception!", 0);
		return -1;
	}
	catch(Devora::Quit) 
	{
		return 0;
	}
}