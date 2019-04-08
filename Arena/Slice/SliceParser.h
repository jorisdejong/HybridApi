/*
  ==============================================================================

    SliceParser.h
    Created: 5 Mar 2019 9:57:29pm
    Author:  Joris

  ==============================================================================
*/

#pragma once

#include "../ResXmlParser.h"
#include "Slice.h"

class SliceParser : public ResXmlParser
{
public:
	SliceParser();
	~SliceParser();

	Array<Slice> getSlices();
	Array<Screen> getScreens();
	
private:
	Array<Slice> slices;
	Array<Screen> screens;
	void parse();

	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType );
};
