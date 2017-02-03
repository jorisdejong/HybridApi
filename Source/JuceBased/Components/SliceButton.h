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
#include "../ArenaHelpers/Slice/Slice.h"



//==============================================================================
/*
*/

class SliceButton    : public ShapeButton
{
public:
	SliceButton ( hybrid::Slice& s, Point<int> scale );
    ~SliceButton();
	
    String name;
	int64 getUniqueId();
    
    void update();
	Path getPath();
	void createPath( Point<int> scale);
	
	//overridden functions for ShapeButton
	bool hitTest(int x, int y) override;
	void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override;
	void resized() override;

	void mouseDown( const MouseEvent& ) override;
	void mouseUp( const MouseEvent& ) override;
	void mouseDrag( const MouseEvent& ) override;

private:
	Path path;
	hybrid::Slice& slice;
	Path makePath( Array<Point<float>>& points, Point<int> scale );

	SliceButton* lastDraggedButton;

	Colour primaryColour = Colours::aquamarine;
	Colour backgroundColour = Colours::crimson;
	Colour outlineColour = Colours::antiquewhite;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliceButton)
};


#endif  // SLICEBUTTON_H_INCLUDED
