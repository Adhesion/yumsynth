/*
 * Voicer.h
 *
 * Handles polyphony, sums individual operator voices.
 *
 * @author Andrew Ford
 */

#ifndef __VOICER_H
#define __VOICER_H

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

private:
	int numVoices;
};

#endif
