#ifndef __Input_h__
#define __Input_h__

#include <Windows.h>

#include <vector>
#include <string>
#include <fstream>

class Input
{
public:
	struct Key
	{
		int key;
		bool up;
	};

	struct Mouse
	{
		long x;
		long y;
	};

	Mouse mouse;
	std::vector<Key> keys;

	Input();
};

class InputParser
{
public:
	InputParser();
	void parse(MSG msg, Input& input);
};

class InputPlayer
{
	std::fstream file;

public:
	InputPlayer(std::string path);
	void read(Input& input);
};

class InputRecorder
{
	std::fstream file;

public:
	InputRecorder(std::string path);
	void write(Input& input);
};

#endif