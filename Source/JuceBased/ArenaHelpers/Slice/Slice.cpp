#include "Slice.h"

Array<Point<float>> hybrid::Slice::getPoints()
{
	if ( !maskPoints.isEmpty() )
		return maskPoints;
	else
		return inputRectPoints;
}

XmlElement * hybrid::Slice::toXml()
{
	//for every slice, create an xmlelement and its id, and which screen it belongs to
	XmlElement* sliceXml = new XmlElement( "slice" );
	sliceXml->setAttribute( "name", sliceId.first );
	sliceXml->setAttribute( "uniqueId", String( sliceId.second ) );
	sliceXml->setAttribute( "enable", enabled );
	sliceXml->setAttribute( "screenId", String( screenId ) );
	

	//then create an xmlelement to store its rects and points
	XmlElement* inputRectXml = new XmlElement( "inputRect" );
	inputRectXml->setAttribute( "orientation", inputRectOrientation );
	sliceXml->addChildElement( inputRectXml );

	//loop through this rect's points
	for ( Point<float> point : inputRectPoints )
	{
		//for every step, create an xmlelement and store the step nr
		XmlElement* pointXml = new XmlElement( "point" );
		pointXml->setAttribute( "x", point.getX() );
		pointXml->setAttribute( "y", point.getY() );
		inputRectXml->addChildElement( pointXml );
	}

	//then create an xmlelement to store its rects and points
	XmlElement* maskXml = new XmlElement( "mask" );
	sliceXml->addChildElement( maskXml );

	//loop through this mask's points
	for ( Point<float> point : maskPoints )
	{
		//for every step, create an xmlelement and store the step nr
		XmlElement* pointXml = new XmlElement( "point" );
		pointXml->setAttribute( "x", point.getX() );
		pointXml->setAttribute( "y", point.getY() );
		maskXml->addChildElement( pointXml );
	}

	return sliceXml;
}
