/*
 * Operator.h
 *
 * Implements an FM operator.
 *
 * @author Andrew Ford
 */

#ifndef __OPERATOR_H
#define __OPERATOR_H

#define PI 3.1415926535

#include <vector>

enum operatorParams
{
	attack = 0,
	decay,
	sustain,
	release,
	frequencyMult,
	FMindex,
	numOperatorParams
};

class Operator
{
public:
	Operator();
	~Operator();

	void noteOn( float freq );
	void noteOff();

	/*
	 * Call evaluate() on inputs (linked operators) and use the result for
	 * frequency modulation.
	 * Caches value in case multiple other operators reference this one as
	 * input.
	 */
	float evaluate();
	/*
	 * Increment envelope counter & reset cache.
	 * Should ALWAYS be called after evaluate.
	 */
	void postEvaluate();

	void setSamplerate( int sr );

	float getParam( int param );
	void setParam( int param, float value );

	void addInputOperator( Operator* in );
	void resetInputOperators();

	bool isPlaying();

private:
	float frequency;
	float sineInput;
	float sineIncrement;

	float* params;

	int currentEnvelopeState;
	int currentEnvelopeCounter;
	float getEnvelopeValue();
	/*
	 * Last envelope level - needed when calculating release if released before
	 * sustain.
	 */
	float releaseLevel;

	bool playing;

	int samplerate;

	float cache;

	std::vector< Operator* > inputs;
};

#endif
