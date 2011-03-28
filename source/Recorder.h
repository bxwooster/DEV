#ifndef __Recorder_h__
#define __Recorder_h__

#include <string>
#include <fstream>

#include "DefaultInput.h"

using std::fstream;

class Recorder
{
	fstream file;

public:
	Recorder(std::string path);
	void write(DefaultInput& input);
	void read(DefaultInput& input);
};

#endif