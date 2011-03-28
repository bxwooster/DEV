#ifndef __Input_h__
#define __Input_h__

#include <Windows.h>

#include <vector>
#include <string>
#include <fstream>

struct Input
{
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
};

class DefaultInput : public Input
{
public:
	DefaultInput();
	void flush();
	void message(MSG msg);
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