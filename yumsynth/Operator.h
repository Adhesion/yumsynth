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

enum envelope
{
	attack = 0,
	decay,
	sustain,
	release,
	envelopeStates
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

	void addInputOperator( Operator* in );
	void resetInputOperators();

private:
	float frequency;
	float sineInput;
	float sineIncrement;

	float* envelopeVals;
	int currentEnvelopeState;
	int currentEnvelopeCounter;
	float getEnvelopeValue();

	int samplerate;

	float cache;

	std::vector< Operator* > inputs;
};

#endif
