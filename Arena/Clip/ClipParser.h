/*
  ==============================================================================

    ClipParser.h
    Created: 30 Jan 2019 10:16:20pm
    Author:  Joris

  ==============================================================================
*/

#pragma once

#include "../ResXmlParser.h"
#include "Clip.h"

class ClipParser : 
	private ResXmlParser
{
public:
	ClipParser();
	~ClipParser();

	/**	scrape the comp for all the clips */
	Array<Clip> getClips();

private:
	Array<Clip> clips;
	void parseClips();
	
};