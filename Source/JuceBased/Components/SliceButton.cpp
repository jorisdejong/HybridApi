/*
  ==============================================================================

    SliceButton.cpp
    Created: 4 Feb 2017 1:31:25am
    Author:  Joris

  ==============================================================================
*/

#include "SliceButton.h"

SliceButton::SliceButton( hybrid::Slice & slice ) : 
	slice ( slice ), 
	PathButton( slice.sliceId.first, slice.maskPoints.size() > 0 ? slice.maskPoints : slice.inputRectPoints )
{
	//check if the slice has maskpoints,
	//if so, it's a regular slice with a mask or a polyslice
	//and the mask points should be used for the path
	//otherwise it's a regular slice and the inputrect points should be used for the path

	////enable loads the state from the ass xml file
	////it can still be edited via the slicelist
	setVisible( slice.enabled );
}

SliceButton::~SliceButton()
{
}

int64 SliceButton::getUniqueId()
{
	return slice.sliceId.second;
}

hybrid::Slice & SliceButton::getSlice()
{
	return slice;
}

void SliceButton::resized()
{
	PathButton::resized();
	////update the visibilty
	setVisible( slice.enabled && !slice.screenIsCollapsed );
}
