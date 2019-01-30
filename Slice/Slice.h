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

namespace hybrid
{
	//keep track of the unique id of the screen
	//jtodo this should be the other way around
	//or be a struct
	//or even be a map instead of a pair
	typedef std::pair<String, int64> NamedUniqueId;

	struct Screen
	{
		int64 uid;
		String name;
		bool folded = false;
	};

	/*A slice consists of

	inputRect: the smallest rectangle that covers this slice in its entirety
	mask points: the input polygon or input mask points that make up the shape of this slice
	orientation: the rotation of the input rect

	*/

	enum FillMode
	{
		Fit = 0,
		Fill = 1,
		Stretch = 2,
		Clone = 3
	};

	class Slice
	{
	public:

		Slice( NamedUniqueId id, bool enable ) : sliceId( id ), enabled( enable )
		{
			inputRectPoints.clear();
			maskPoints.clear();
			maskRectPoints.clear();
			inputRectOrientation = 0.0;
			maskRectOrientation = 0.0;

			fillMode = FillMode::Fit;
		}

		Slice( const Slice& slice ) : sliceId( slice.sliceId ), enabled( slice.enabled ), inputRectPoints( slice.inputRectPoints ), maskPoints( slice.maskPoints ), maskRectPoints( slice.maskRectPoints ), inputRectOrientation( slice.inputRectOrientation ), maskRectOrientation( slice.maskRectOrientation ) {	}

		Slice() : Slice( std::make_pair( String( "New Slice" ), 0 ), false ) {}

		~Slice() {}

		
		/**when a screen is collapsed, the slice should not draw in preview
		i cannot use the enabled bool for this, because when the screen is uncollapsed
		this value should be maintained*/
		bool screenIsCollapsed = false;

		/**uniqued id and name of the screen this slice is a part of
		this is needed to sort the slice into screens in chaser app*/
		int64 screenId = 0;
        
        /**unique id and name of this slice*/
		NamedUniqueId sliceId;
        bool enabled;
        
		Array<Point<float>> getPoints();

		Array<Point<float>> inputRectPoints;
		float inputRectOrientation;
		Array<Point<float>> maskPoints;
		
        //currently the maskRectPoints are not being used for anything
        //i use the inputRect and optionally the maskpoints of the slice to decide which parts of the slice to draw
        Array<Point<float>> maskRectPoints;
        float maskRectOrientation;

		//this is used in TemplateTool to set the fillmode of a slice individually
		FillMode fillMode;

		XmlElement* toXml();

	private:

	};
}



#endif  // SLICE_H_INCLUDED
