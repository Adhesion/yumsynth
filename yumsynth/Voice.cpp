/*
 * Voice.cpp
 *
 * Represents a single voice with 4 operators. See Voice.h for details.
 *
 * @author Andrew Ford
 */

#include "Voice.h"
#include "Operator.h"

#include <math.h>

Voice::Voice( float* freq, int numOps )
	: frequencyTable( freq ), numOperators( numOps )
{
	note = -1;
	// assume dummy samplerate at first, setSamplerate() will get called by
	// host
	samplerate = 48000;

	for( int i = 0; i < numOperators; i++ )
	{
		Operator* o = new Operator();
		o->setSamplerate( samplerate );
		operators.push_back( o );
	}
}

Voice::~Voice()
{
	for( int i = 0; i < numOperators; i++ )
	{
		delete operators[ i ];
	}
}

float Voice::evaluate()
{
	float out = 0.0f;

	for( unsigned int i = 0; i < slots.size(); i++ )
	{
		out += slots[ i ]->evaluate();
	}

	return out;
}

void Voice::noteOn( int n )
{
	// if invalid note val, return
	if ( n < 0 || n > 127 )
	{
		return;
	}
	note = n;
	
	float frequency = frequencyTable[ note ];
	for( unsigned int i = 0; i < operators.size(); i++ )
	{
		operators[ i ]->noteOn( frequency );
	}
}

void Voice::noteOff()
{
	note = -1;
	for( unsigned int i = 0; i < operators.size(); i++ )
	{
		operators[ i ]->noteOff();
	}
}

void Voice::setSamplerate( int sr )
{
	samplerate = sr;
	for( unsigned int i = 0; i < operators.size(); i++ )
	{
		operators[ i ]->setSamplerate( sr );
	}
}

int Voice::getNote()
{
	return note;
}

bool Voice::isPlaying()
{
	// TODO aggregate values from operators - if one is playing, voice is playing
	return false;
}
