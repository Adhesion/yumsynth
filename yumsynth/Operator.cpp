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
	releaseLevel = 0;

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
		// maximum amount of modulation is based on the modulator's frequency
		in += inputs[ i ]->evaluate() * frequency *
			inputs[ i ]->getParam( frequencyMult );
	}
	// mix inputs properly - no need to div by 1 either
	in = inputs.size() > 1 ? in / (float)inputs.size() : in;

	// period of the sine wave is (1/freq) * samplerate samples, so for each
	// sample we should increment inverse * 2PI (since it's in radians)
	float tempFreq = ( frequency * params[ frequencyMult ] ) + in;
	sineIncrement = tempFreq * 2.0f * (float)PI / (float)samplerate;
	// increment value used to calculate frequency (phase)
	sineInput += sineIncrement;
	// keep in 0-2PI range
	if ( sineInput >= 2.0f * PI )
	{
		sineInput = fmod( sineInput, 2.0f * (float)PI );
	}

	float out = sin( sineInput ) * getEnvelopeValue();
	currentEnvelopeCounter++;
	cache = out;

	return out;
}

void Operator::postEvaluate()
{
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
	float out = 1.0f;

	// A,D,R are times in seconds, S is level (progress also in seconds)
	float progress = (float)currentEnvelopeCounter / (float)samplerate;

	// check/increment state if necessary
	if ( currentEnvelopeState != sustain &&
		progress > params[ currentEnvelopeState ] )
	{
		// if release is done, we're not playing anymore
		if ( currentEnvelopeState == release )
		{
			playing = false;
			return 0.0f;
		}
		// for attack/decay, go to subsequent state
		else
		{
			currentEnvelopeCounter = 0;
			progress = (float)currentEnvelopeCounter / (float)samplerate;
			currentEnvelopeState++;
		}
	}

	switch ( currentEnvelopeState )
	{
	case attack:
		// go from 0->1 from 0-max attack time
		out = progress / params[ attack ];
		break;
	case decay:
		// go from 1->sustain
		out = 1.0f -
			( ( progress / params[ decay ] ) * ( 1.0f - params[ sustain ] ) );
		break;
	case sustain:
		out = params[ sustain ];
		break;
	case release:
		// go from last level->0
		out = releaseLevel -
			( ( progress / params[ release ] ) * releaseLevel );
		break;
	}

	if ( currentEnvelopeState != release )
	{
		releaseLevel = out;
	}

	return out;
}
