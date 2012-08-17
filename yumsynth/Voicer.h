/*
 * Voicer.h
 *
 * Handles polyphony, sums individual voices.
 *
 * @author Andrew Ford
 */

#ifndef __VOICER_H
#define __VOICER_H

#include <vector>

class Voice;

class Voicer
{
public:
	Voicer( int num );
	~Voicer();

	/*
	 * Return current output sample.
	 */
	float evaluate();

	void noteOn( int note );
	void noteOff( int note );

	void setSamplerate( int sr );

private:
	std::vector< Voice* > voices;
	int numVoices;

	// midi note -> frequency
	float* frequencyTable;
};

#endif
