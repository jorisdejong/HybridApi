/*
  ==============================================================================

	Clip.cpp
	Created: 30 Jan 2019 9:48:43pm
	Author:  Joris

  ==============================================================================
*/

#include "Clip.h"

Identifier Clip::ClipIdentifier = Identifier( "clip" );

Clip::Clip() :
	uniqueId( -1 ),
	column( -1 ),
	layer( -1 ),
	deck( -1 ),
	name( String() ),
	defaultName( String() ),
	thumbFileData( String() )
{
}

Clip::~Clip()
{
}

ValueTree Clip::toValueTree()
{
	ValueTree clipTree( ClipIdentifier );
	clipTree.setProperty( Identifier( "uniqueId" ), uniqueId, nullptr );
	clipTree.setProperty( Identifier( "column" ), column, nullptr );
	clipTree.setProperty( Identifier( "layer" ), layer, nullptr );
	clipTree.setProperty( Identifier( "deck" ), deck, nullptr );
	clipTree.setProperty( Identifier( "name" ), name, nullptr );

	return clipTree;
}

Clip Clip::fromValueTree( const ValueTree& clipTree )
{
	Clip clip;
	clip.uniqueId = clipTree.getProperty( "uniqueId" );
	clip.column = clipTree.getProperty( "column" );
	clip.layer = clipTree.getProperty( "layer" );
	clip.deck = clipTree.getProperty( "deck" );
	clip.name = clipTree.getProperty( "name" );

	return clip;
}
