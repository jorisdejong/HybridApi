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
		int64 uniqueId;
		String thumbFileData; //to reconstruct the thumbfile name: filename for files, uid for generators
		String thumbData; //the actual pixels as base64
	};

	//*loops throught the array passed in and sets the corresponding Image*/
	static void buildCollection( Array<Thumbnail>& thumbs );
	
	//struct Thumb //convenience struct to hold these two together
	//{
	//	ResXmlParser::Clip clip;
	//	Image image;
	//};
	//Array<Thumb> getThumbs() const;
	//void addThumb( Thumb thumb );
	//void clearThumbs();

private:
	Image getThumbForFile( String thumbData );
	//Array<Thumb> thumbs;
	
};
