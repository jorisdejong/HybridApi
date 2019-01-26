#include "ThumbReader.h"
/*
  ==============================================================================

	ThumbReader.cpp
	Created: 10 Oct 2018 11:27:40am
	Author:  Joris

  ==============================================================================
*/

#include "ThumbReader.h"


ThumbReader::ThumbReader()
{
}

ThumbReader::~ThumbReader()
{
}

void ThumbReader::buildCollection( Array<Thumbnail>& thumbs )
{
	for ( auto thumb : thumbs )
		thumb.thumbData = getBase64ForFile( thumb.thumgFileData );
}

Image ThumbReader::getImageFromBase64( String base64 )
{
	MemoryOutputStream base64Stream;
	Base64::convertFromBase64( base64Stream, base64 );
	PNGImageFormat format;
	MemoryInputStream imgStream( base64Stream.getMemoryBlock(), true );
	return format.decodeImage( imgStream );
}

String ThumbReader::getBase64ForFile( String thumbFileData )
{
	Image img = Image();
	if ( thumbFileData.isNotEmpty() )
	{
		String thumbname;
		File file = File( thumbFileData );
		if ( file.existsAsFile() )
		{
			thumbname = file.getFileNameWithoutExtension();
			Time time = file.getLastModificationTime();
			thumbname += String( time.getYear() )
				+ String( time.getMonth() )
				+ String( time.getDayOfMonth() )
				+ String( time.getHours() )
				+ String( time.getMinutes() )
				+ String( time.getSeconds() )
				+ String( time.getMilliseconds() )
				+ String( file.getSize() )
				+ file.getFileExtension().substring( 1 )
				+ "_video.png";
		}

		File thumbFile = File::getSpecialLocation( File::SpecialLocationType::userApplicationDataDirectory ).getParentDirectory().getChildFile( "Local/Resolume Arena 6/previews/" + thumbname );
		if ( thumbFile.existsAsFile() )
			img = ImageFileFormat::loadFrom( thumbFile );
	}
	
	PNGImageFormat format;
	MemoryOutputStream imgStream;
	format.writeImageToStream( img, imgStream );
	return Base64::toBase64( imgStream.getData(), imgStream.getDataSize() );
}

MemoryBlock ThumbReader::toStream( const Thumbnail& thumb )
{
	ValueTree thumbTree( Identifier( "thumb" ) );
	thumbTree.setProperty( Identifier( "uniqueId" ), thumb.uniqueId, nullptr );
	thumbTree.setProperty( Identifier( "thumbData" ), thumb.thumbData, nullptr );

	MemoryOutputStream clipStream;
	thumbTree.writeToStream( clipStream );
	return clipStream.getMemoryBlock();
}

Thumbnail ThumbReader::toThumb( const MemoryBlock& block )
{
	ValueTree thumbTree = ValueTree::readFromData( block.getData(), block.getSize() );
	Thumbnail thumb;
	thumb.uniqueId = thumbTree.getProperty( "uniqueId" );
	thumb.thumbData = thumbTree.getProperty( "thumbData" );
	
	return thumb;
}
