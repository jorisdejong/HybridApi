
/*
  ==============================================================================

	ThumbParser.cpp
	Created: 10 Oct 2018 11:27:40am
	Author:  Joris

  ==============================================================================
*/

#include "ThumbParser.h"

ThumbParser::ThumbParser()
{
	buildCollection();
}

ThumbParser::~ThumbParser()
{
}

Array<Thumb> ThumbParser::getThumbs()
{
	return thumbs;
}

void ThumbParser::buildCollection()
{
	thumbs.clear();
	for ( auto clip : getClips() )
	{
		Thumb thumb;
		thumb.uniqueId = clip.uniqueId;
		thumb.thumbData = getBase64ForFile( clip.thumbFileData );
		thumbs.add( thumb );
	}
}

String ThumbParser::getBase64ForFile( String thumbFileData )
{
	String base64 = String();
	if ( thumbFileData.isNotEmpty() )
	{
		String thumbname;
		File file = File( thumbFileData );
		if ( file != File() ) //todo, here we could catch that the file needs to be relocated
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

		// Mac: /Users/[username]/Library/Application Support/Resolume Avenue 6/
		// Windows: C:\Users\[ username ]\AppData\Local\Resolume Avenue 6\
		
		File thumbDir;
		if ( ( SystemStats::getOperatingSystemType() & SystemStats::Windows ) != 0 )
			thumbDir = File::getSpecialLocation( File::SpecialLocationType::userApplicationDataDirectory ).getParentDirectory().getChildFile( "Local/Resolume Arena 6/previews/" );
		else if (  ( SystemStats::getOperatingSystemType() & SystemStats::MacOSX ) != 0  )
			thumbDir = File::getSpecialLocation( File::SpecialLocationType::userApplicationDataDirectory ).getChildFile( "Application Support/Resolume Arena 6/previews/" );
		
		/** we're running on a system that Resolume can't run on 
		  * so we have no point being here
		  * I don't know how we got here in the first place, 
		  * because ClipParser should't have found any clips to find thumbs for */
		jassert( thumbDir != File() );
		
		File thumbFile = thumbDir.getChildFile( thumbname );
		if ( thumbFile.existsAsFile() )
		{
			Image img = ImageFileFormat::loadFrom( thumbFile );
			PNGImageFormat format;
			MemoryOutputStream imgStream;
			format.writeImageToStream( img, imgStream );
			base64 = Base64::toBase64( imgStream.getData(), imgStream.getDataSize() );
		}
	}
	//todo 
	//when we get here, it means Resolume has a clip that we want a thumbnail for
	//if base64 is empty at this point, we should return some basic thumbnail to use as a placeholder
	return base64;
}


