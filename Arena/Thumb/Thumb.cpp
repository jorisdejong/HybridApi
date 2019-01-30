/*
  ==============================================================================

    Thumb.cpp
    Created: 30 Jan 2019 9:48:51pm
    Author:  Joris

  ==============================================================================
*/

#include "Thumb.h"

Identifier Thumb::ThumbIdentifier = Identifier( "thumb" );

Thumb::Thumb() :
	uniqueId ( 0 ),
	thumbData ( String() )
{
}

Thumb::~Thumb()
{
}

Image Thumb::getImage()
{
	MemoryOutputStream base64Stream;
	Base64::convertFromBase64( base64Stream, thumbData );
	PNGImageFormat format;
	MemoryInputStream imgStream( base64Stream.getMemoryBlock(), true );
	return format.decodeImage( imgStream );
}

ValueTree Thumb::toValueTree()
{
	ValueTree thumbTree( ThumbIdentifier );
	thumbTree.setProperty( Identifier( "uniqueId" ), uniqueId, nullptr );
	thumbTree.setProperty( Identifier( "thumbData" ), thumbData, nullptr );
	return thumbTree;
}

Thumb Thumb::fromValueTree( const ValueTree & tree )
{
	Thumb thumb;
	thumb.uniqueId = tree.getProperty( "uniqueId" );
	thumb.thumbData = tree.getProperty( "thumbData" );

	return thumb;
}
