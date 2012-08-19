/*
 * Operator.cpp
 *
 * Implements an FM operator.
 *
 * @author Andrew Ford
 */

#include "Operator.h"

Operator::Operator()
{
	cache = -9999.9f;

	frequency = 0.0f;
	sineInput = 0.0f;
	sineIncrement = 0.0f;

	currentEnvelopeState = attack;
	currentEnvelopeCounter = 0;

	playing = false;

	params = new float[ numOperatorParams ];
}

Operator::~Operator()
{
	delete[] params;
}

void Operator::noteOn( float freq )
{
	frequency = freq;
	sineInput = 0.0f;
	// period of the sine wave is (1/freq) * samplerate samples, so for each
	// sample we should increment inverse * 2PI (since it's in radians)
	//sineIncrement = frequency * params[ frequencyMult ] * 2.0f * (float)PI /
		//(float)samplerate;

	currentEnvelopeState = attack;
	currentEnvelopeCounter = 0;

	playing = true;
}

void Operator::noteOff()
{
	currentEnvelopeState = release;
	currentEnvelopeCounter = 0;
	playing = false;
}

float Operator::evaluate()
{
	if ( !playing )
	{
		return 0.0f;
	}

	// cache is for further ops in chain - if >1 later operator evaluates us,
	// we shouldn't calculate twice
	if ( cache > -9999.0f )
	{
		return cache;
	}

	// evaluate value of inputs to use as input to modulate our frequency
	// TODO maybe check for loops? here or in voice?
	float in = 0.0f;
	for( unsigned int i = 0; i < inputs.size(); i++ )
	{
		in += inputs[ i ]->evaluate();
	}
	// mix inputs properly - no need to div by 1 either
	in = inputs.size() > 1 ? in / (float)inputs.size() : in;

	float tempFreq = ( frequency * params[ frequencyMult ] ) + ( in * 100.0f * inputs.size() );
	sineIncrement = tempFreq * 2.0f * (float)PI / (float)samplerate;
	// increment value used to calculate frequency
	sineInput += sineIncrement;
	// keep in 0-2PI range
	if ( sineInput >= 2.0f * PI )
	{
		sineInput = fmod( sineInput, 2.0f * (float)PI );
	}

	float out = sin( sineInput ) * getEnvelopeValue();
	cache = out;

	return out;
}

void Operator::postEvaluate()
{
	// increment envelope

	// if release is over, set playing to false

	// reset cache
	cache = -9999.9f;
}

void Operator::setSamplerate( int sr )
{
	samplerate = sr;
	// have to update increment here, in case note is sounding when samplerate
	// changed
	//sineIncrement = frequency * params[ frequencyMult ] * 2.0f * (float)PI /
	//	(float)samplerate;
}

float Operator::getParam( int param )
{
	if ( param < 0 || param >= numOperatorParams )
	{
		return -9999.9f;
	}
	return params[ param ];
}

void Operator::setParam( int param, float value )
{
	if ( param < 0 || param >= numOperatorParams )
	{
		return;
	}
	params[ param ] = value;

	// if we're changing the frequency multiplier we need to restart our sine
	// generation since that depends on it (inaccurate otherwise)
	if ( param == frequencyMult )
	{
		sineInput = 0.0f;
	}
}

void Operator::addInputOperator( Operator* in )
{
	inputs.push_back( in );
}

void Operator::resetInputOperators()
{
	inputs.clear();
	// restart sine wave in case we're playing a note (see above)
	sineInput = 0.0f;
}

bool Operator::isPlaying()
{
	return playing;
}

float Operator::getEnvelopeValue()
{
	return 1.0f;
}
