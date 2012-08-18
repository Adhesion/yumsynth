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

	/*
	 * Getters/setters for how each voice's operators are arranged - for now
	 * all have same settings.
	 */
	void setOperatorArrangement( int type );
	int getOperatorArrangement();
	int getNumOperatorArrangements();
	std::string getOperatorArrangementDescription( int type );

	void setOperatorParam( int op, int param, float value );
	float getOperatorParam( int op, int param );

	void setVolume( float in );
	float getVolume();

private:
	float volume;

	std::vector< Voice* > voices;
	int numVoices;

	// midi note -> frequency
	float* frequencyTable;
};

#endif
