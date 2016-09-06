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
	String getUniqueName( String nameToCheck, StringArray existingNames, String defaultIfEmpty = "New Name" )
	{
		//if the name is empty, give it a default name
		if ( nameToCheck.isEmpty() )
			nameToCheck = defaultIfEmpty;

		//check if the name exists already
		while ( existingNames.contains( nameToCheck ) )
		{
			//check if its ends in a number
			//if necessary, up that number by 1
			//and replace it in the string
			int endValue = nameToCheck.getTrailingIntValue();
			if ( endValue != 0 || nameToCheck.endsWith( "0" ) )
			{
				int newEndValue = endValue + 1;
				nameToCheck = nameToCheck.replaceSection( nameToCheck.lastIndexOf( String( endValue ) ),
					String( newEndValue ).length(), String( newEndValue ) );
			}
			else
				//just append " 2" and be done with it
				nameToCheck += " 2";
		}
		//everything is fine, just return the name
		return nameToCheck;
	}
}

#endif  // UTILITIES_H_INCLUDED


