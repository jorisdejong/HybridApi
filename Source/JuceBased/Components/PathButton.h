/*
  ==============================================================================

    PathButton.h
    Created: 28 Dec 2014 11:15:33am
    Author:  Joris de Jong

  ==============================================================================
*/

#ifndef PATHBUTTON_H_INCLUDED
#define PATHBUTTON_H_INCLUDED

#include "JuceHeader.h"



//==============================================================================
/*
PathButton create a button based on a set of points
It can then resize its path based on the resolution of the view it's living in
*/



class PathButton 
{
public:
	PathButton ( Array<Point<float>> points );
    ~PathButton();

	void updatePath( Point<int> resolution );
	Path path;

	Array<Point<float>> pathPoints;

private:
	
	
	

	
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PathButton)
};


#endif  // PATHBUTTON_H_INCLUDED
