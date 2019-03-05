/*
  ==============================================================================

	SliceParser.cpp
	Created: 5 Mar 2019 9:57:29pm
	Author:  Joris

  ==============================================================================
*/

#include "SliceParser.h"

SliceParser::SliceParser()
{
	parseSlices();
}

SliceParser::~SliceParser()
{
}

Array<Slice> SliceParser::getSlices()
{
	return slices;
}

void SliceParser::parseSlices()
{
	/**
	* oh noes... we haven't been able to parse the ass xml file
	* the most likely cause is that there is no resolume folder in documents
	* or that the current ass file doesn't actually exist
	* either way, we're not going to find any slices like this
	*/
	jassert( assXml );
	if ( !assXml )
		return;

	slices.clear();

	XmlElement* screensXml = assXml->getChildByName( "screens" );
	if ( screensXml != nullptr )
	{
		forEachXmlChildElement( *screensXml, child )
		{
			if ( child->hasTagName( "Screen" ) || child->hasTagName( "DmxScreen" ) )
			{
				//ignore files with the name scaling to help KBK setups
				if ( !child->getStringAttribute( "name" ).containsIgnoreCase( "scaling" ) )
				{
					XmlElement* layers = child->getChildByName( "layers" );
					if ( layers != nullptr )
					{
						forEachXmlChildElement( *layers, layerChild )
						{
							if ( layerChild->hasTagName( "Slice" ) || layerChild->hasTagName( "Polygon" ) || layerChild->hasTagName( "DmxSlice" ) )
							{
								Slice newSlice;
								String uidString = layerChild->getStringAttribute( "uniqueId", "0" );
								newSlice.uniqueId = uidString.getLargeIntValue();
								
								XmlElement* params = layerChild->getChildByName( "Params" );
								if ( params != nullptr )
								{
									forEachXmlChildElement( *params, paramChild )
									{
										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Name" )
											newSlice.name = paramChild->getStringAttribute( "value", "Slice" );

										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Enabled" )
											newSlice.enabled = paramChild->getStringAttribute( "value", "1" ).getIntValue() != 0;
									}
								}

								XmlElement* inputRect = layerChild->getChildByName( "InputRect" );
								if ( inputRect != nullptr )
								{
									newSlice.inputRectOrientation = inputRect->getStringAttribute( "orientation", "0" ).getFloatValue();
									forEachXmlChildElement( *inputRect, vertex )
									{
										if ( vertex->hasTagName( "v" ) )
											addPointToSlice( vertex, newSlice.inputRectPoints );
									}
								}
								XmlElement* sliceMask = layerChild->getChildByName( "SliceMask" );
								if ( sliceMask != nullptr )
								{
									XmlElement* shapeObject = sliceMask->getChildByName( "ShapeObject" );
									if ( shapeObject != nullptr )
									{
										XmlElement* shape = shapeObject->getChildByName( "Shape" );
										if ( shape != nullptr )
										{
											XmlElement* contour = shape->getChildByName( "Contour" );
											if ( contour != nullptr )
											{
												XmlElement* points = contour->getChildByName( "points" );
												if ( points != nullptr )
												{
													forEachXmlChildElement( *points, vertex )
													{
														if ( vertex->hasTagName( "v" ) )
															addPointToSlice( vertex, newSlice.maskPoints );
													}
												}
											}
										}
									}
								}

								XmlElement* contour = layerChild->getChildByName( "InputContour" );
								if ( contour != nullptr )
								{
									XmlElement* points = contour->getChildByName( "points" );
									if ( points != nullptr )
									{
										forEachXmlChildElement( *points, vertex )
										{
											if ( vertex->hasTagName( "v" ) )
												addPointToSlice( vertex, newSlice.maskPoints );
										}
									}
								}
								//insert at the beginning to keep everything in order
								slices.insert( 0, newSlice );
							}
						}
					}
				}
			}
		}
	}

	if ( slices.size() == 0 )
		DBG( "Not able to parse any slice data" );
	else
		DBG( "Slice data parsed succesfully" );
}


void SliceParser::addPointToSlice( juce::XmlElement *element, Array<Point<float>>& pointType )
{
	Point<float> newPoint;
	newPoint.x = element->getStringAttribute( "x", "0.0" ).getFloatValue();
	newPoint.y = element->getStringAttribute( "y", "0.0" ).getFloatValue();
	pointType.add( newPoint );
}
