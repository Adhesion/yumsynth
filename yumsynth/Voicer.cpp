/*
 * Voicer.cpp
 *
 * Handles polyphony, sums individual voices.
 *
 * @author Andrew Ford
 */

#include "Voicer.h"
#include "Voice.h"

#include <stdio.h>

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

	for( int i = 0; i < numVoices; i++ )
	{
		voices.push_back( new Voice( frequencyTable, 4 ) );
	}

	volume = 1.0f;
}

Voicer::~Voicer()
{
	for( int i = 0; i < numVoices; i++ )
	{
		delete voices[ i ];
	}
	delete[] frequencyTable;
}

float Voicer::evaluate()
{
	float temp = 0.0f;
	for( int i = 0; i < numVoices; i++ )
	{
		temp += voices[ i ]->evaluate();
	}
	// mix voices based on whether they're active or not
	temp = temp / (float)numVoices;
	return temp * volume;
}

void Voicer::noteOn( int note )
{
	// check if note is already playing, if it is ignore
	for( int i = 0; i < numVoices; i++ )
	{
		if ( voices[ i ]->getNote() == note )
		{
			return;
		}
	}

	// play note in a free voice
	for( int i = 0; i < numVoices; i++ )
	{
		if ( !voices[ i ]->isPlaying() )
		{
			voices[ i ]->noteOn( note );
			return;
		}
	}
}

void Voicer::noteOff( int note )
{
	// find voice playing this note & turn it off
	for( int i = 0; i < numVoices; i++ )
	{
		if ( voices[ i ]->getNote() == note )
		{
			voices[ i ]->noteOff();
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
		voices[ i ]->setSamplerate( sr );
	}
}

void Voicer::setOperatorArrangement( int type )
{
	for( int i = 0; i < numVoices; i++ )
	{
		voices[ i ]->setOperatorArrangement( type );
	}
}

int Voicer::getOperatorArrangement()
{
	return voices[ 0 ]->getOperatorArrangement();
}

int Voicer::getNumOperatorArrangements()
{
	// assume they all have the same amount since it's in their constructor
	return voices[ 0 ]->getNumOperatorArrangements();
}

std::string Voicer::getOperatorArrangementDescription( int type )
{
	return voices[ 0 ]->getOperatorArrangementDescription( type );
}

void Voicer::setOperatorParam( int op, int param, float value )
{
	// maybe allow for per-voice differences in the future
	for( int i = 0; i < numVoices; i++ )
	{
		voices[ i ]->setOperatorParam( op, param, value );
	}
}

float Voicer::getOperatorParam( int op, int param )
{
	// again, assume they all have the same values for now
	return voices[ 0 ]->getOperatorParam( op, param );
}

void Voicer::setVolume( float in )
{
	if ( volume < 0.0f || volume > 10.0f )
	{
		return;
	}
	volume = in;
}

float Voicer::getVolume()
{
	return volume;
}
