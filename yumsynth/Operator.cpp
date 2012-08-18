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
	sineIncrement = frequency * params[ frequencyMult ] * 2.0f * (float)PI /
		(float)samplerate;

	currentEnvelopeState = attack;
	currentEnvelopeCounter = 0;

	playing = true;
}

void Operator::noteOff()
{
	currentEnvelopeState = release;
	currentEnvelopeCounter = 0;
}

float Operator::evaluate()
{
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

	float out = sin( sineInput + in ) * getEnvelopeValue();
	cache = out;

	return out;
}

void Operator::postEvaluate()
{
	// increment value used to calculate frequency
	sineInput += sineIncrement;
	// keep in 0-2PI range
	if ( sineInput >= 2.0f * PI )
	{
		sineInput = fmod( sineInput, 2.0f * (float)PI );
	}

	// increment envelope

	// if release if over, set playing to false

	// reset cache
	cache = -9999.9f;
}

void Operator::setSamplerate( int sr )
{
	samplerate = sr;
	// have to update increment here, in case note is sounding when samplerate
	// changed
	sineIncrement = frequency * params[ frequencyMult ] * 2.0f * (float)PI /
		(float)samplerate;
}

void Operator::addInputOperator( Operator* in )
{
	inputs.push_back( in );
}

void Operator::resetInputOperators()
{
	inputs.clear();
}

bool Operator::isPlaying()
{
	return playing;
}

float Operator::getEnvelopeValue()
{
	return 1.0f;
}
