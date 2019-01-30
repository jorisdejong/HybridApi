/*
  ==============================================================================

	Thumb.h
	Created: 30 Jan 2019 9:48:51pm
	Author:  Joris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

/**basic info about the thumbnail
and serialisation functions */
class Thumb 
{
public:
	Thumb();
	~Thumb();

	int64 uniqueId;

	/** the actual pixels as base64 */
	String thumbData;

	static Identifier ThumbIdentifier;
	
	ValueTree toValueTree();
	static Thumb fromValueTree( const ValueTree& tree );

	Image getImage(); //get an image from base64

private:
};
