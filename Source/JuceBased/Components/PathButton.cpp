/*
  ==============================================================================

	SliceButton.cpp
	Created: 28 Dec 2014 11:15:33am
	Author:  Joris de Jong

  ==============================================================================
*/

#include "PathButton.h"


PathButton::PathButton( String name, Array<Point<float>> points ) : ShapeButton( name, Colours::transparentBlack, Colours::transparentBlack, Colours::transparentBlack )
{
	setName( name );
	
	pathPoints = points;
	setClickingTogglesState( true );
	setToggleState( false, sendNotification );
}

PathButton::~PathButton()
{
	lastDraggedButton = nullptr;
}

void PathButton::makePath( Array<Point<float>> points )
{
	if ( !getParentComponent() )
		return;

	path.clear();

	Point<int> scale = Point<int>{ getParentWidth(), getParentHeight() };
	for ( int i = 0; i < points.size(); i++ )
	{
		Point<float> p = points[i];
		//points are stored normalized in 0...1 range
		//here points are transformed to fit within the window they live in
		p *= scale;
		if ( i == 0 )
			path.startNewSubPath( p );
		else
			path.lineTo( p );
	}
	path.closeSubPath();
}



bool PathButton::hitTest( int x, int y )
{
	//the path points are absolute, while the hitTest x and y points are relative to the bounds
	//that's why I'm adding getPosition to get the correct result
	if ( path.contains( float( x + getPosition().x ), float( y + getPosition().y ) ) )
		return true;
	return false;
}

void PathButton::paintButton( juce::Graphics &g, bool isMouseOverButton, bool isButtonDown )
{
	if ( isVisible() )
	{
		Rectangle<float> r = getLocalBounds().toFloat().reduced( 0.5f );

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
		g.drawFittedText( getName(), r.toType<int>(), Justification::centred, 1 );

		g.strokePath( path, PathStrokeType( 1.0 ), trans );
	}
}

void PathButton::resized()
{
	makePath( pathPoints );

	Rectangle<int> bounds = path.getBounds().toType<int>();
	setBounds( bounds );
}


void PathButton::mouseDown( const MouseEvent& event )
{
	Button::mouseDown( event );

	lastDraggedButton = nullptr;
}

void PathButton::mouseUp( const MouseEvent& event )
{
	//if this button was part of the last drag action,
	//don't do anything, the state will already have been toggled
	if ( lastDraggedButton == this )
	{
		lastDraggedButton = nullptr;
		return;
	}

	//otherwise do the regular mouse up action
	Button::mouseUp( event );
	lastDraggedButton = nullptr;
}

void PathButton::mouseDrag( const MouseEvent& event )
{
	Button::mouseDrag( event );

	Component* parent = getParentComponent();
	PathButton* button = dynamic_cast<PathButton*>(parent->getComponentAt( event.getPosition() + getPosition() ));

	//if we're actually over a slicebutton and it wasn't the last button we toggled
	if ( button && button != lastDraggedButton )
	{
		bool state = button->getToggleState();
		button->setToggleState( !state, sendNotification );
		lastDraggedButton = button;
	}
}





