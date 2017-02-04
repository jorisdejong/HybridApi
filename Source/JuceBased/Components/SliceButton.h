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

class SliceButton : public PathButton
{
public:
	SliceButton( hybrid::Slice& slice );
	~SliceButton();

	int64 getUniqueId();

	void resized() override;

private:
	hybrid::Slice& slice;
};




#endif  // SLICEBUTTON_H_INCLUDED
