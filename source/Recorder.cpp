#include "Recorder.h"

Recorder::Recorder()
{
	file = fstream("C:/replay", fstream::in | fstream::out | fstream::binary);
	file.exceptions( 0 );
}

void Recorder::write(DefaultInput& input)
{

}

void Recorder::read(DefaultInput& input)
{

}