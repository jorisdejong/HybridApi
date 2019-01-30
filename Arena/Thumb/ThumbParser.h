/*
  ==============================================================================

    ThumbParser.h
    Created: 10 Oct 2018 11:27:40am
    Author:  Joris

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Thumb.h"
#include "../Clip/ClipParser.h"

#pragma once

/** scrape's the res comp for clips 
and then find the matching thumbnail */
class ThumbParser :
	public ClipParser
{
public:
	ThumbParser();
	~ThumbParser();

	/** scrape the comp for thumbnails */
	Array<Thumb> getThumbs();
	
private:
	Array<Thumb> thumbs;

	void buildCollection();
	String getBase64ForFile( String thumbData );
	
	
};
