#include "Recorder.h"

Recorder::Recorder(std::string path)
{
	file.exceptions( fstream::failbit | fstream::badbit | fstream::eofbit );
	file.open(path.c_str(), fstream::in | fstream::out | fstream::binary);
}

void Recorder::write(DefaultInput& input)
{
	file.write( (char*)&input.mouse, sizeof(input.mouse));

	int n = input.keys.size();
	file.write( (char*)&n, sizeof(n) );

	for(int i = 0; i < n; i++)
	{
		file.write( (char*)&input.keys[i], sizeof(input.keys[i]));
	}
}

void Recorder::read(DefaultInput& input)
{
	file.read( (char*)&input.mouse, sizeof(input.mouse));

	int n;
	file.read( (char*)&n, sizeof(n) );
	input.keys.resize(n);

	for(int i = 0; i < n; i++)
	{
		file.read( (char*)&input.keys[i], sizeof(input.keys[i]));
	}
}