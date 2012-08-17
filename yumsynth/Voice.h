/*
 * Voice.h
 *
 * Represents a single voice in the synth - holds the operators and the
 * operator arrangement. Each voice has X of its own operators - a bit
 * wasteful if they all have the same settings but opens the future
 * possibility of having separate operator settings per voice (similar to the
 * actual YM2612)
 *
 * @author Andrew Ford
 */

#ifndef __VOICE_H
#define __VOICE_H

#include <vector>

class Operator;

class Voice
{
public:
	Voice( float* freq, int numOps );
	~Voice();

	float evaluate();
	void noteOn( int n );
	void noteOff();

	void setSamplerate( int samplerate );

	int getNote();

	void setOperatorArrangement( int type );

	// note that this means "is this voice sounding", ie, note could be off but
	// voice is in release stage so it's still making sound (and thus can't be
	// used for a new note)
	bool isPlaying();

private:
	int note;
	int samplerate;

	// lists for operators - slots are operators that are last in the chain.
	int numOperators;
	std::vector< Operator* > operators;
	std::vector< Operator* > slots;

	// gotten from voicer at init
	float* frequencyTable;
};

#endif
