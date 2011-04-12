#define NOMINMAX

#include <iostream>
#include <memory>
#include <Windows.h>

#include "Input.h"
#include "Freddy.h"

using std::auto_ptr;

void run()
{
	const bool write = true;

	Freddy::Settings settings = {960, 960, 0.25};
	auto_ptr<Freddy> freddy( new Freddy(settings) );

	InputParser parser;
	InputRecorder recorder("replay");
	InputPlayer player("replay");

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

		write ? recorder.write(input) : player.read(input);

		freddy->step(input);
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
	catch(Freddy::Quit) 
	{
		return 0;
	}
}