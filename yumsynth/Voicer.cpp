/*
 * Voicer.cpp
 *
 * Handles polyphony, sums individual voices.
 *
 * @author Andrew Ford
 */

#include "Voicer.h"
#include "Voice.h"

float frequencyTable[ 128 ];

Voicer::Voicer( int num )
	: numVoices( num )
{
	frequencyTable = new float[ 128 ];
	// fill midi note -> frequency table
	double root = 1.05946309435929; // 12th root of 2
	// start at A, go to C (lowest midi note)
	double note = 6.875;
	for( int i = 0; i < 3; i++ )
	{
		note *= root;
	}
	for( int i = 0; i < 128; i++ )
	{
		frequencyTable[i] = (float)note;
		note *= root;
	}

	voices[ numVoices ];
	for( int i = 0; i < numVoices; i++ )
	{
		voices[ i ] = Voice( frequencyTable );
	}
}

Voicer::~Voicer()
{
	delete[] voices;
	delete[] frequencyTable;
}

float Voicer::evaluate()
{
	float temp = 0.0f;
	for( int i = 0; i < numVoices; i++ )
	{
		temp += voices[ i ].evaluate();
	}
	return temp;
}

void Voicer::noteOn( int note )
{
	// check if note is already playing, if it is ignore
	for( int i = 0; i < numVoices; i++ )
	{
		if ( voices[ i ].getNote() == note )
		{
			return;
		}
	}

	// play note in a free voice
	for( int i = 0; i < numVoices; i++ )
	{
		if ( !voices[ i ].isPlaying() )
		{
			voices[ i ].noteOn( note );
			return;
		}
	}
}

void Voicer::noteOff( int note )
{
	// find voice playing this note & turn it off
	for( int i = 0; i < numVoices; i++ )
	{
		if ( voices[ i ].getNote() == note )
		{
			voices[ i ].noteOff();
			// based on noteOn() we already know only one voice will be
			// playing a particular note, so we can exit here
			return;
		}
	}
}

void Voicer::setSamplerate( int sr )
{
	for( int i = 0; i < numVoices; i++ )
	{
		voices[ i ].setSamplerate( sr );
	}
}
