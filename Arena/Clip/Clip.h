/*
  ==============================================================================

    Clip.h
    Created: 30 Jan 2019 9:48:43pm
    Author:  Joris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

/**basic info about the clip
and serialisation functions */
class Clip 
{
public:
	Clip();
	~Clip();

	//these are all public
	//because the main purpose of a Clip object
	//is to provide access to these variables

	int64 uniqueId;
	int column;
	int layer;
	int deck;
	String name;
	String defaultName;

	/** used to reconstruct the thumbfile name:
	this will be the filename for files
	and a uid for generators */
	String thumbFileData;

	ValueTree toValueTree();
	static Clip fromValueTree( const ValueTree& tree );

	/**used to identify a ValueTree containing clip info*/
	static Identifier ClipIdentifier;

private:
};
