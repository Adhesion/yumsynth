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

	setNumInputs( numChannels );
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
