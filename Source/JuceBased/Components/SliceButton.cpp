/*
  ==============================================================================

    SliceButton.cpp
    Created: 4 Feb 2017 1:31:25am
    Author:  Joris

  ==============================================================================
*/

#include "SliceButton.h"

SliceButton::SliceButton( hybrid::Slice & slice ) :
	PathButton( slice.maskPoints.size() > 0 ? slice.maskPoints : slice.inputRectPoints ),
    slice ( slice )
{
	setName( slice.sliceId.first );
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


bool SliceButton::hitTest( int x, int y )
{
	//the path points are absolute, while the hitTest x and y points are relative to the bounds
	//that's why I'm adding getPosition to get the correct result
	if ( path.contains( float( x + getPosition().x ), float( y + getPosition().y ) ) )
		return true;
	return false;
}

void SliceButton::paintButton( juce::Graphics &g, bool isMouseOverButton, bool isButtonDown )
{
	if ( isVisible() )
	{
		Rectangle<float> r = path.getBounds().toFloat().reduced( 0.5f );

		if ( isButtonDown )
		{
			const float sizeReductionWhenPressed = 0.01f;

			r = r.reduced( sizeReductionWhenPressed * r.getWidth(),
				sizeReductionWhenPressed * r.getHeight() );
		}
		const AffineTransform trans( path.getTransformToScaleToFit( r, false ) );

		Colour col = getToggleState() ? primaryColour : backgroundColour;
		if ( isMouseOverButton )
			col = col.brighter( 0.2f );
		g.setColour( col );

		g.fillPath( path, trans );

		//draw the name because DJAktion is a little bitch
		g.setColour( outlineColour );
		g.drawFittedText( getName(), r.toNearestInt(), Justification::centred, 1 );

		g.strokePath( path, PathStrokeType( 1.0 ), trans );
	}
}


void SliceButton::resized()
{
	updatePath( getParentComponent()->getLocalBounds().getBottomRight() );

	//the path coordinates are relative to 0.0, so set the bounds of the component to cover the whole parent
	//the path will then be drawn correctly
	setBounds( getParentComponent()->getLocalBounds() );
	////update the visibilty
	setVisible( slice.enabled && !slice.screenIsCollapsed );
}

void SliceButton::setColors( Colour primary, Colour background, Colour outline )
{
	primaryColour = primary;
	backgroundColour = background;
	outlineColour = outline;
}
