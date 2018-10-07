/*
  ==============================================================================

	SliceButton.cpp
	Created: 28 Dec 2014 11:15:33am
	Author:  Joris de Jong

  ==============================================================================
*/

#include "PathButton.h"


PathButton::PathButton( Array<Point<float>> points )
{
	pathPoints = points;
}

PathButton::~PathButton()
{
	
}


void PathButton::updatePath( Point<int> resolution )
{
	path.clear();

	for ( int i = 0; i < pathPoints.size(); i++ )
	{
		Point<float> p = pathPoints[i];
		//points are stored normalized in 0...1 range
		//here points are transformed to fit within the window they live in
		p *= resolution;
		if ( i == 0 )
			path.startNewSubPath( p );
		else
			path.lineTo( p );
	}
	path.closeSubPath();
}













