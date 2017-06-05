/*
  ==============================================================================

  ResXmlParser.cpp
  Created: 9 Sep 2015 8:45:41pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "ResXmlParser.h"
#include "../File/FileHelper.h"

ResXmlParser::ResXmlParser()
{

}

ResXmlParser::~ResXmlParser()
{

}



bool ResXmlParser::parseAssFile( File f, OwnedArray<hybrid::Slice>& slices, Array<hybrid::Screen>& screens, Point<int>& resolution )
{
	if ( !f.exists() )
	{
		return false;
	}
	else
	{
		DBG( "Trying to parse: " + f.getFullPathName() );

		ScopedPointer<XmlElement> xmlRoot;
		xmlRoot = (XmlDocument::parse( f ));

		//see if we're dealing with a res 4 or res 5 ass file
		//then try to parse it
		bool succesfulParse = false;

		ScopedPointer<XmlElement> mainPreset = getMainPresetElement( f );
		if ( mainPreset )
			succesfulParse = parseRes5Xml( *mainPreset, slices, screens, resolution );
		
		return succesfulParse;
	}
}


String ResXmlParser::getAdvancedPresetNameFromAdvancedXml( File res5XmlFile )
{
	String fileNameToReturn = String().empty;

	if ( !FileHelper::isFileValid( res5XmlFile, false ) )
		return fileNameToReturn;

	ScopedPointer<XmlElement> mainXmlElement = XmlDocument::parse( res5XmlFile );

	if ( mainXmlElement->hasAttribute( "presetFile" ) )
		fileNameToReturn = mainXmlElement->getStringAttribute( "presetFile", String().empty );

	return fileNameToReturn;
}


juce::XmlElement* ResXmlParser::getMainPresetElement( File assFile )
{
	if ( !FileHelper::isFileValid( assFile, false ) )
		return nullptr;

	DBG( "Trying to parse: " + assFile.getFullPathName() );
	int64 startTime = Time::getHighResolutionTicks();
	int64 totalTime = Time::getHighResolutionTicks();


	XmlElement* mainXmlElement = XmlDocument::parse( assFile );
	//deletion of the returned xmlelement will happen in the function that uses the data

	DBG( "Main parsing took " + String( Time::highResolutionTicksToSeconds( Time::getHighResolutionTicks() - startTime ) ) + " seconds" );
	startTime = Time::getHighResolutionTicks();

	//a little sleight of hand so we can work both with the internal res pref file as well as a saved preset file
	XmlElement* returnElement = nullptr;

	//if the assfile starts with the tag screensetup, we got the advanced.xml file itself, so just return that
	if ( mainXmlElement->hasTagName( "ScreenSetup" ) )
	{
		returnElement = mainXmlElement;
		DBG( "advanced.xml parsing took " + String( Time::highResolutionTicksToSeconds( Time::getHighResolutionTicks() - startTime ) ) + " seconds" );
		startTime = Time::getHighResolutionTicks();
	}

	//else we got a named and seperately saved preset file, so we first need to extract the screensetup element
	else if ( mainXmlElement->getChildByName( "ScreenSetup" ) != nullptr )
	{
		//create a new element, so we don't delete the child with the parent
		returnElement = new XmlElement( *mainXmlElement->getChildByName( "ScreenSetup" ) );
		//delete the parent
		delete mainXmlElement;
		DBG( "preset parsing took " + String( Time::highResolutionTicksToSeconds( Time::getHighResolutionTicks() - startTime ) ) + " seconds" );
		startTime = Time::getHighResolutionTicks();
	}

	DBG( "Total parsing took " + String( Time::highResolutionTicksToSeconds( Time::getHighResolutionTicks() - totalTime ) ) + " seconds" );
	return returnElement;
}


Array< hybrid::NamedUniqueId > ResXmlParser::getScreenNames( File assFile )
{
	Array< hybrid::NamedUniqueId > names;

	//if the file isn't valid, just return an empty array
	//the component will catch this
	if ( !FileHelper::isFileValid( assFile, false ) )
		return names;

	XmlElement* mainPreset = getMainPresetElement( assFile );
	if ( mainPreset != nullptr )
	{
		XmlElement* screens = mainPreset->getChildByName( "screens" );
		if ( screens != nullptr )
		{
			forEachXmlChildElement( *screens, child )
			{
				//keep track of how many scaling screens we have

				if ( child->hasTagName( "Screen" ) )
				{
					String screenName = child->getStringAttribute( "name" );

					//only add screens that are called scaling
					if ( screenName.containsIgnoreCase( "scaling" ) )
					{
						//remove the word scaling from the name
						screenName = screenName.fromFirstOccurrenceOf( "scaling", false, true );

						//remove beginning white spaces
						while ( screenName.startsWith( " " ) )
							screenName = screenName.trimStart();

						//if that doesn't leave any actual name, just give it a default name
						if ( screenName == String() )
							screenName = "Unnamed Scaling";

						String screenUniqueIdString = child->getStringAttribute( "uniqueId", "0" );
						int64 uId = screenUniqueIdString.getLargeIntValue();
						names.add( std::make_pair( screenName, uId ) );
					}
				}
			}
		}

		delete mainPreset;
	}
	return names;
}








bool ResXmlParser::parseRes5Xml( juce::XmlElement& screenSetup, OwnedArray<hybrid::Slice>& slices, Array<hybrid::Screen>& screens, Point<int>& resolution )
{
	//get the resolution from the "sizing" element
	//we later need the resolution to store the slices in the 0...1 range
	XmlElement* sizing = screenSetup.getChildByName( "sizing" );
	if ( sizing != nullptr )
	{
		forEachXmlChildElement( *sizing, inputs )
		{
			if ( inputs->hasTagName( "inputs" ) )
			{
				forEachXmlChildElement( *inputs, InputSize )
				{
					if ( InputSize->getStringAttribute( "name" ) == "0:1" )
					{
						resolution.x = InputSize->getIntAttribute( "width", 1920 );
						resolution.y = InputSize->getIntAttribute( "height", 1080 );
					}
				}
			}
		}
	}

	slices.clear();

	//i can't clear the screens array completely, because i need to restore the folded value
	//make a list of screennames that are folded
	Array<int64> foldedScreens;
	for ( auto screen : screens )
		if ( screen.folded )
			foldedScreens.add( screen.uid );
	screens.clear();

	XmlElement* screensXml = screenSetup.getChildByName( "screens" );
	if ( screensXml != nullptr )
	{
		forEachXmlChildElement( *screensXml, child )
		{
			if ( child->hasTagName( "Screen" ) )
			{
				hybrid::Screen newScreen;
				newScreen.name = child->getStringAttribute( "name" );
				newScreen.uid = child->getStringAttribute( "uniqueId", "0" ).getLargeIntValue();
				//check if the uid is part of the folded list
				for ( auto id : foldedScreens )
					if ( id == newScreen.uid )
						newScreen.folded = true;
				screens.add( newScreen );


				//ignore files with the name scaling to help KBK setups
				if ( !child->getStringAttribute( "name" ).containsIgnoreCase( "scaling" ) )
				{
					XmlElement* layers = child->getChildByName( "layers" );
					if ( layers != nullptr )
					{
						forEachXmlChildElement( *layers, layerChild )
						{
							if ( layerChild->hasTagName( "Slice" ) || layerChild->hasTagName( "Polygon" ) )
							{
								hybrid::Slice* newSlice = new hybrid::Slice();
								String uidString = layerChild->getStringAttribute( "uniqueId", "0" );
								int64 uid = uidString.getLargeIntValue();
								String name = "Slice";
								newSlice->enabled = true;

								XmlElement* params = layerChild->getChildByName( "Params" );
								if ( params != nullptr )
								{
									forEachXmlChildElement( *params, paramChild )
									{
										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Name" )
											name = paramChild->getStringAttribute( "value", "Slice" );

										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Enabled" )
											newSlice->enabled = paramChild->getStringAttribute( "value", "1" ).getIntValue() != 0;
									}
								}

								//store the name and the uniqueId
								newSlice->sliceId = std::make_pair( name, uid );

								XmlElement* inputRect = layerChild->getChildByName( "InputRect" );
								if ( inputRect != nullptr )
								{
									newSlice->inputRectOrientation = inputRect->getStringAttribute( "orientation", "0" ).getFloatValue();
									forEachXmlChildElement( *inputRect, vertex )
									{
										if ( vertex->hasTagName( "v" ) )
											addPointToSlice( vertex, newSlice->inputRectPoints, resolution );
									}
								}
								XmlElement* sliceMask = layerChild->getChildByName( "SliceMask" );
								if ( sliceMask != nullptr )
								{
									XmlElement* shapeObject = sliceMask->getChildByName( "ShapeObject" );
									if ( shapeObject != nullptr )
									{
										XmlElement* maskRect = shapeObject->getChildByName( "Rect" );
										if ( maskRect != nullptr )
										{
											newSlice->maskRectOrientation = maskRect->getStringAttribute( "orientation", "0" ).getFloatValue();
											forEachXmlChildElement( *maskRect, vertex )
											{
												if ( vertex->hasTagName( "v" ) )
													addPointToSlice( vertex, newSlice->maskRectPoints, resolution );
											}
										}
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
															addPointToSlice( vertex, newSlice->maskPoints, resolution );
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
												addPointToSlice( vertex, newSlice->maskPoints, resolution );
										}
									}
								}
								newSlice->screenId = newScreen.uid;

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
	{
		DBG( "Not able to parse any slice data" );
		return false;
	}
	else
	{
		DBG( "Slice data parsed succesfully" );
		return true;
	}
}




void ResXmlParser::addPointToSlice( juce::XmlElement *element, Array<Point<float>>& pointType, Point<int> resolution )
{
	Point<float> newPoint;
	newPoint.x = element->getStringAttribute( "x", "0.0" ).getFloatValue() / float( resolution.x );
	newPoint.y = element->getStringAttribute( "y", "0.0" ).getFloatValue() / float( resolution.y );
	pointType.add( newPoint );
}
