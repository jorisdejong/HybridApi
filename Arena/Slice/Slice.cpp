#include "Slice.h"

Slice::Slice() : Slice( "New Slice", 0, false )
{
}

Slice::~Slice()
{
}

Slice::Slice( String name, int64 uid, bool enable ) :
	name( name ),
	uniqueId( uid ),
	enabled( enable ),
	inputRectOrientation( 0.0f )
{
	inputRectPoints.clear();
	maskPoints.clear();
}

Slice::Slice( const Slice & slice ) :
	name( slice.name ),
	uniqueId( slice.uniqueId ),
	enabled( slice.enabled ),
	inputRectPoints( slice.inputRectPoints ),
	inputRectOrientation( slice.inputRectOrientation ),
	maskPoints( slice.maskPoints )
{

}

Array<Point<float>> Slice::getPoints()
{
	if ( !maskPoints.isEmpty() )
		return maskPoints;
	else
		return inputRectPoints;
}

XmlElement * Slice::toXml()
{
	//for every slice, create an xmlelement and its id
	XmlElement* sliceXml = new XmlElement( "slice" );
	sliceXml->setAttribute( "name", name );
	sliceXml->setAttribute( "uniqueId", String( uniqueId ) );
	sliceXml->setAttribute( "enable", enabled );

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
