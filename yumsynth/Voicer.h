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

private:
	int numVoices;
};

#endif
