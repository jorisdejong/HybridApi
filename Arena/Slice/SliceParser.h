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

private:
	Array<Slice> slices;
	void parseSlices();

	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType );
};
