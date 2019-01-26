/*
  ==============================================================================

    ThumbReader.h
    Created: 10 Oct 2018 11:27:40am
    Author:  Joris

  ==============================================================================
*/

#include "JuceHeader.h"

#pragma once

//* find a clip's thumbnail */

class ThumbReader
{
public:
	ThumbReader();
	~ThumbReader();

	struct Thumbnail
	{
		int64 uniqueId = 0;
		String thumbFileData = String(); //to reconstruct the thumbfile name: filename for files, uid for generators
		String thumbData = String(); //the actual pixels as base64
	};

	//*loops throught the array passed in and sets the corresponding Image*/
	static void buildCollection( Array<Thumbnail>& thumbs );

	Image getImageFromBase64( String base64 ); //get an image from base64

private:
	String getBase64ForFile( String thumbData );
	MemoryBlock toStream( const Thumbnail& thumb );
	Thumbnail toThumb( const MemoryBlock& block );
	
};
