/*
 * yumsynth.h
 *
 * Main class for yumsynth (VST setup, events, etc.)
 *
 * @author Andrew Ford
 */

#ifndef __YUMSYNTH_H
#define __YUMSYNTH_H

#include "public.sdk/source/vst2.x/audioeffectx.h"

#include <vector>

class Voicer;

enum yParams
{
	arrangement = 0,
	numYParams,
	numYPrograms = 128
};

typedef struct
{
	float parameters[ numYParams ];
	char name[ kVstMaxProgNameLen ];
} yumsynthProgram;

class yumsynth : public AudioEffectX
{
public:
	yumsynth( audioMasterCallback master );
	~yumsynth();

	void processReplacing( float** inputs, float** outputs,
		VstInt32 sampleFrames );
	VstInt32 processEvents( VstEvents* events );
	
	void setParameter( VstInt32 index, float value );
	float getParameter( VstInt32 index );
	void getParameterLabel( VstInt32 index, char* text );
	void getParameterDisplay( VstInt32 index, char* text );
	void getParameterName( VstInt32 index, char* text );

	void setProgramName( char* name );
	void getProgramName( char* name );

	bool getEffectName( char* eName );
	bool getProductString( char* pString );
	bool getVendorString( char* vString );
	VstInt32 getVendorVersion();

private:
	// in order to deal with deltaframes (ie that midi events will happen in
	// the "future") we keep a list of pending midi events. this will dispatch
	// one to the voicer (called by processReplacing() if it's in the current
	// block of samples)
	void dispatchMIDI( VstMidiEvent midiEvent );
	std::vector<VstMidiEvent> midiEvents;

	char name[ kVstMaxEffectNameLen ];
	char vendor[ kVstMaxVendorStrLen ];
	VstInt32 version;

	float parameters[ numYParams ];
	int numChannels;

	yumsynthProgram* programs;

	Voicer* voicer;
};

#endif