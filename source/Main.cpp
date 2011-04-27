#define NOMINMAX

#include <iostream>
#include <memory>
#include <Windows.h>

#include "Window.h"
#include "Input.h"
#include "App.h"
#include "Timer.h"

using std::auto_ptr;
using namespace Devora;

void run()
{
	const bool write = true;

	App::Settings settings = {960, 960, 0.25, 5.0};
	auto_ptr<App> Devora( new App(settings) );

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
	catch(App::Quit) 
	{
		return 0;
	}
}