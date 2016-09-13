/*
  ==============================================================================

    Utilities.h
    Created: 6 Sep 2016 11:47:39pm
    Author:  Joris

  ==============================================================================
*/

#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "JuceHeader.h"

namespace utils
{
	/*  this will return a unique name based on the input name
	if the name already exists, it will return that name with a "2" appended
	if the name already ends in a number, it will return that name with that number
	raised to the first available free number
	*/
	String getUniqueName( String nameToCheck, StringArray existingNames, String defaultIfEmpty = "New Name" );
	
}

#endif  // UTILITIES_H_INCLUDED


