/*
  ==============================================================================

    SliceButton.h
    Created: 28 Dec 2014 11:15:33am
    Author:  Joris de Jong

  ==============================================================================
*/

#ifndef SLICEBUTTON_H_INCLUDED
#define SLICEBUTTON_H_INCLUDED

#include "JuceHeader.h"



//==============================================================================
/*
PathButton create a button based on a set of points
It can then resize its path based on the resolution of the view it's living in
*/

class PathButton    : public ShapeButton
{
public:
	PathButton ( String name, Array<Point<float>> points, Point<int> scale );
    ~PathButton();
    
	//overridden functions for ShapeButton
	bool hitTest(int x, int y) override;
	void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override;
	void resized() override;

	void mouseDown( const MouseEvent& ) override;
	void mouseUp( const MouseEvent& ) override;
	void mouseDrag( const MouseEvent& ) override;

private:
	Array<Point<float>> pathPoints;
	Path path;
	Path makePath( Array<Point<float>> points, Point<int> scale );

	PathButton* lastDraggedButton;

	Colour primaryColour = Colours::aquamarine;
	Colour backgroundColour = Colours::crimson;
	Colour outlineColour = Colours::antiquewhite;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PathButton)
};


#endif  // SLICEBUTTON_H_INCLUDED
