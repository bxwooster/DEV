#ifndef __Recorder_h__
#define __Recorder_h__

#include <fstream>

#include "DefaultInput.h"

using std::fstream;

class Recorder
{
	fstream file;

public:
	Recorder();
	void write(DefaultInput& input);
	void read(DefaultInput& input);
};

#endif