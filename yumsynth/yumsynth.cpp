/*
 * yumsynth.cpp
 *
 * Main class for yumsynth (VST setup, events, etc.)
 *
 * @author Andrew Ford
 */

#include "yumsynth.h"
#include "Voicer.h"

AudioEffect* createEffectInstance( audioMasterCallback master )
{
	return new yumsynth( master );
}

yumsynth::yumsynth( audioMasterCallback master )
	: AudioEffectX( master, numYParams, numYPrograms )
{
	strcpy( name, "yumsynth" );
	strcpy( vendor, "Adhesion" );

	version = 100;

	parameters[ arrangement ] = 0.0f;

	programs = new yumsynthProgram[ numYPrograms ];
	for( int i = 0; i < numYPrograms; i++ )
	{
		programs[ i ].parameters[ arrangement ] = parameters[ arrangement ];
		strcpy( programs[ i ].name, "Init" );
	}

	numChannels = 2;

	setNumInputs( 0 );
	setNumOutputs( numChannels );
	canProcessReplacing( true );

	setUniqueID( 'yums' );

	// 6 voices for now
	voicer = new Voicer( 6 );
}

yumsynth::~yumsynth()
{
	delete voicer;
}

void yumsynth::processReplacing( float** inputs, float** outputs,
		VstInt32 sampleFrames )
{
	for( VstInt32 i = 0; i < sampleFrames; i++ )
	{
		// dispatch MIDI events if deltaFrames == current position
		// TODO optimize this?
		std::vector< VstMidiEvent>::iterator it = midiEvents.begin();
		while( it != midiEvents.end() )
		{
			if ( it->deltaFrames == i )
			{
				dispatchMIDI( *it );
				it = midiEvents.erase( it );
			}
			else
			{
				it->deltaFrames--;
				++it;
			}
		}

		float out = voicer->evaluate();
		for( int j = 0; j < numChannels; j++ )
		{
			// TODO pan here?
			outputs[j][i] = out;
		}
	}
}

VstInt32 yumsynth::processEvents( VstEvents* events )
{
	for( int i = 0; i < events->numEvents; i++ )
	{
		// we only care about MIDI for the moment
		if ( events->events[ i ]->type == kVstMidiType )
		{
			VstMidiEvent* ev = (VstMidiEvent*)events->events[ i ];
			midiEvents.push_back( *ev );
		}
	}

	// maybe pass through MIDI here? not sure

	return 1;
}

void yumsynth::setProgramName( char* name )
{
	strcpy( programs[ curProgram ].name, name );
}

void yumsynth::getProgramName( char* name )
{
	strcpy( name, programs[ curProgram ].name );
}

bool yumsynth::getEffectName( char* eName )
{
	strcpy( eName, name );
	return true;
}

bool yumsynth::getProductString( char* pString )
{
	strcpy( pString, name );
	return true;
}

bool yumsynth::getVendorString( char* vString )
{
	strcpy( vString, vendor );
	return true;
}

VstInt32 yumsynth::getVendorVersion()
{
	return version;
}

void yumsynth::dispatchMIDI( VstMidiEvent midiEvent )
{

}