/*
  ==============================================================================

  Slice.h
  Created: 10 Jan 2015 7:46:04pm
  Author:  Joris de Jong

  ==============================================================================
  */

#ifndef SLICE_H_INCLUDED
#define SLICE_H_INCLUDED

#include "JuceHeader.h"


  /** A slice consists of:
	* name and uniqueid
	*	inputRect: the smallest rectangle that covers this slice in its entirety
	*	mask points: the input polygon or input mask points that make up the shape of this slice
	*	orientation: the rotation of the input rect	*/

class Slice
{
public:
	Slice();
	~Slice();

	Slice( String name, int64 uid, bool enable );
	Slice( const Slice& slice );

	String name;
	int64 uniqueId;
	bool enabled;

	/** will return the mask points
	  * or the input rect points if the mask is empty */
	Array<Point<float>> getPoints();

	Array<Point<float>> inputRectPoints;
	float inputRectOrientation;
	Array<Point<float>> maskPoints;
	
	XmlElement* toXml();

private:
	
};

struct Screen
{
	int64 uid;
	String name;
	Array<int64> slices;
};



#endif  // SLICE_H_INCLUDED
