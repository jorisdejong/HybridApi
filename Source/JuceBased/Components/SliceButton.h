/*
  ==============================================================================

    SliceButton.h
    Created: 4 Feb 2017 1:31:25am
    Author:  Joris

  ==============================================================================
*/

#ifndef SLICEBUTTON_H_INCLUDED
#define SLICEBUTTON_H_INCLUDED

#include "PathButton.h"
#include "../ArenaHelpers/Slice/Slice.h"
#include "../../Chaser/Source/gui/MultiDragButton.h"

class SliceButton : 
	public PathButton,
	public MultiDragButton
{
public:
	SliceButton( hybrid::Slice& slice );
	~SliceButton();

	//overridden functions for MultiDragButton
	bool hitTest( int x, int y ) override;
	void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown ) override;
	void resized() override;

	int64 getUniqueId();
	hybrid::Slice& getSlice();

	void setColors( Colour primary, Colour background, Colour outline );

private:
	hybrid::Slice& slice;

	Colour primaryColour = Colours::aquamarine;
	Colour backgroundColour = Colours::crimson;
	Colour outlineColour = Colours::antiquewhite;
};




#endif  // SLICEBUTTON_H_INCLUDED
