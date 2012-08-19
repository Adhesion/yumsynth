/*
 * yumsynth.cpp
 *
 * Main class for yumsynth (VST setup, events, etc.)
 *
 * @author Andrew Ford
 */

#include "yumsynth.h"
#include "Voicer.h"
#include "Operator.h"

#include <sstream>

AudioEffect* createEffectInstance( audioMasterCallback master )
{
	return new yumsynth( master );
}

yumsynth::yumsynth( audioMasterCallback master )
	: AudioEffectX( master, numYPrograms, numYParams )
{
	strcpy( name, "yumsynth" );
	strcpy( vendor, "Adhesion" );

	version = 100;

	// setup defaults
	// note these all have to be 0.0-1.0 since they directly get used for
	// getParameter()
	parameters[ arrangement ] = 0.0f;
	for( int i = operatorParamBase; i < volume; i += numOperatorParams )
	{
		parameters[ i + attack ] = 0.1f;
		parameters[ i + decay ] = 0.1f;
		parameters[ i + sustain ] = 0.7f;
		parameters[ i + release ] = 0.2f;
		parameters[ i + frequencyMult ] = 0.0f;
	}
	parameters[ volume ] = 0.5f;

	programs = new yumsynthProgram[ numYPrograms ];
	for( int i = 0; i < numYPrograms; i++ )
	{
		for( int j = 0; j < numYParams; j++ )
		{
			programs[ i ].parameters[ j ] = parameters[ j ];
		}
		strcpy( programs[ i ].name, "Init" );
	}

	numChannels = 2;

	setNumInputs( 0 );
	setNumOutputs( numChannels );
	canProcessReplacing( true );

	isSynth();

	setUniqueID( 'yums' );
	
	// 6 voices for now
	voicer = new Voicer( 6 );

	// update voice(r) etc values with init params
	// TODO better way to do this?
	for( int i = 0; i < numYParams; i++ )
	{
		setParameter( i, parameters[ i ] );
	}
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
			if ( it->deltaFrames <= i )
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

void yumsynth::resume()
{
	AudioEffectX::resume();
}

void yumsynth::setParameter( VstInt32 index, float value )
{
	parameters[ index ] = value;
	if ( index == arrangement )
	{
		// need to scale to 0-numOperatorArrangements
		// ie, 0.0 - 7.9999 for max 8
		voicer->setOperatorArrangement(
			(int)floor( value *
			( (float)voicer->getNumOperatorArrangements() - 0.001f ) ) );
	}
	// handle all operator params here
	else if ( index < volume )
	{
		int in = index - operatorParamBase;
		int op = in / numOperatorParams;
		int param = in % numOperatorParams;

		// do some scaling here - maybe should be somewhere else?
		if ( param == release )
		{
			value *= 2.0f;
		}
		else if ( param == frequencyMult )
		{
			value = floor( value * 99.0f + 1.0f );
		}

		voicer->setOperatorParam( op, param, value );
	}
	else if ( index == volume )
	{
		voicer->setVolume( value * 2.0f );
	}
}

float yumsynth::getParameter( VstInt32 index )
{
	return parameters[ index ];
}

void yumsynth::getParameterName( VstInt32 index, char* text )
{
	if ( index == arrangement )
	{
		strcpy( text, "Operator Arrangement" );
	}
	else if ( index < volume )
	{
		std::string paramTemp;
		std::stringstream stream;

		int in = index - operatorParamBase;
		int op = in / numOperatorParams;
		int param = in % numOperatorParams;

		switch( param )
		{
		case attack:
			paramTemp = "Attack";
			break;
		case decay:
			paramTemp = "Decay";
			break;
		case sustain:
			paramTemp = "Sustain";
			break;
		case release:
			paramTemp = "Release";
			break;
		case frequencyMult:
			paramTemp = "Frequency";
			break;
		}

		stream << "Operator " << op << " " << paramTemp;
		strcpy( text, stream.str().c_str() );
	}
	else if ( index == volume )
	{
		strcpy( text, "Volume" );
	}
	else
	{
		strcpy( text, "" );
	}
}

void yumsynth::getParameterDisplay( VstInt32 index, char* text )
{
	if ( index == arrangement )
	{
		std::string desc = voicer->getOperatorArrangementDescription(
			voicer->getOperatorArrangement() );
		strcpy( text, desc.c_str() );
	}
	else if ( index < volume )
	{
		int in = index - operatorParamBase;
		int op = in / numOperatorParams;
		int param = in % numOperatorParams;

		float2string( voicer->getOperatorParam( op, param ), text,
			kVstMaxParamStrLen );
	}
	else if ( index == volume )
	{
		dB2string( voicer->getVolume(), text, kVstMaxParamStrLen );
	}
	else
	{
		strcpy( text, "" );
	}
}

void yumsynth::getParameterLabel( VstInt32 index, char* text )
{
	if ( index == volume )
	{
		strcpy( text, "dB" );
	}
	else
	{
		strcpy( text, "" );
	}
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

VstInt32 yumsynth::getNumMidiInputChannels()
{
	return 1;
}

VstInt32 yumsynth::getNumMidiOutputChannels()
{
	return 0;
}

VstInt32 yumsynth::canDo( char* string )
{
	if ( !strcmp( string, "receiveVstEvents" ) )
		return 1;
	if ( !strcmp( string, "receiveVstMidiEvent" ) )
		return 1;
	if ( !strcmp( string, "midiProgramNames" ) )
		return -1;

	return 0;
}

void yumsynth::dispatchMIDI( VstMidiEvent midiEvent )
{
	// highest 4 bits = what sort of midi event (note on, off, etc.)
	int type = midiEvent.midiData[ 0 ] & 0xf0;
	// note on
	if ( type == 0x90 )
	{
		int note = midiEvent.midiData[ 1 ] & 0x7f;
		int vel = midiEvent.midiData[ 2 ] & 0x7f;
		if ( vel > 0 )
		{
			voicer->noteOn( note );
		}
		else
		{
			// interpret vel=0 as note off for now
			voicer->noteOff( note );
		}
	}
	// note off
	else if ( type == 0x80 )
	{
		int note = midiEvent.midiData[ 1 ] & 0x7f;
		voicer->noteOff( note );
	}
	// maybe handle pitchbend (0xe0), also sustain?
}
