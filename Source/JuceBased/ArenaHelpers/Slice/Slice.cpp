#include "Slice.h"

Array<Point<float>> hybrid::Slice::getPoints()
{
	if ( !maskPoints.isEmpty() )
		return maskPoints;
	else
		return inputRectPoints;
}
