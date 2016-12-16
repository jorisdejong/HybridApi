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



bool ResXmlParser::parseAssFile( File f, OwnedArray<Slice>& slices, Array<Screen>& screens, Point<int>& resolution )
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
		/*
		if (xmlRoot != nullptr && xmlRoot->hasTagName ("preset"))
		succesfulParse = parseRes4Xml( *xmlRoot, slices, resolution );
		else if ( xmlRoot != nullptr && xmlRoot->hasTagName("XmlState"))
		succesfulParse = parseRes5Xml( *xmlRoot, slices, resolution );
		else if ( xmlRoot != nullptr && xmlRoot->hasTagName("ScreenSetup") )
		succesfulParse = parseRes5PrefXml( *xmlRoot, slices, resolution);
		else if ( xmlRoot != nullptr && xmlRoot->hasTagName("avenue"))
		succesfulParse = parseRes4ConfigXml( *xmlRoot, slices, resolution);

		*/
		return succesfulParse;
	}
}


String ResXmlParser::getAdvancedPresetNameFromAdvancedXml( File res5XmlFile )
{
	String fileNameToReturn = String().empty;

	if ( !FileHelper::isFileValid( res5XmlFile ) )
		return fileNameToReturn;

	ScopedPointer<XmlElement> mainXmlElement = XmlDocument::parse( res5XmlFile );

	if ( mainXmlElement->hasAttribute( "presetFile" ) )
		fileNameToReturn = mainXmlElement->getStringAttribute( "presetFile", String().empty );

	return fileNameToReturn;
}


juce::XmlElement* ResXmlParser::getMainPresetElement( File assFile )
{
	if ( !FileHelper::isFileValid( assFile ) )
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


Array< NamedUniqueId > ResXmlParser::getScreenNames( File assFile )
{
	Array< NamedUniqueId > names;

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



/*
bool ResXmlParser::parseRes4ConfigXml(juce::XmlElement &xmlTreeToParse, OwnedArray<Slice>& slices, Point<int> &resolution)
{
forEachXmlChildElement( xmlTreeToParse, avenueChild )
{
if ( avenueChild->hasTagName("settings") )
{
forEachXmlChildElement( *avenueChild, settingsChild )
{
if ( settingsChild->hasTagName("video"))
{
forEachXmlChildElement( *settingsChild, videoChild )
{
if ( videoChild->hasTagName("settings"))
{
forEachXmlChildElement( *videoChild, videoSettingsChild )
{
if ( videoSettingsChild->hasTagName("screenSetup"))
{
forEachXmlChildElement( *videoSettingsChild, screenSetupChild )
{
if ( screenSetupChild->hasTagName("screens"))
{
forEachXmlChildElement(*screenSetupChild, screensChild )
{
if ( screensChild->hasTagName("advancedScreens") )
{
forEachXmlChildElement( *screensChild, advancedScreensChild )
{
if ( advancedScreensChild->hasTagName("screen") )
{
forEachXmlChildElement( *advancedScreensChild, screenNode )
{
if ( screenNode->hasTagName("slices") )
{
//clear the slice array so we don't get doubles
slices.clear();

forEachXmlChildElement( *screenNode, slicesNode)
{
if ( slicesNode->hasTagName("Slice") )
{
String name;
bool enabled = slicesNode->getBoolAttribute("isEnabled");
int type = 0;
float l = 0.0f;
float t = 0.0f;
float r = 0.0f;
float b = 0.0f;

forEachXmlChildElement( *slicesNode, sliceNode)
{

if ( sliceNode->hasTagName("name") )
{
name = sliceNode->getStringAttribute("value");
}

if ( sliceNode->hasTagName("type") )
{
type = sliceNode->getIntAttribute("value");
}

if ( sliceNode->hasTagName("rect") )
{
l = float(sliceNode->getDoubleAttribute("l"));
t = float(sliceNode->getDoubleAttribute("t"));
r = float(sliceNode->getDoubleAttribute("r"));
b = float(sliceNode->getDoubleAttribute("b"));
}

}

//type 0 means only slices are loaded, no masks or crops
if ( type == 0 )
{
Slice* slice = new Slice( name, enabled );
//create 4 points out of the ltrb data
for (int i = 0; i < 4; i++ )
{
Point<float> newPoint;
switch (i)
{
case 0:
newPoint = Point<float>(l,t);
break;
case 1:
newPoint = Point<float>(r,t);
break;
case 2:
newPoint = Point<float>(r,b);
break;
case 3:
newPoint = Point<float>(l,b);
break;
}
slice->inputRectPoints.add(newPoint);
}
slices.insert(0, slice);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
if ( settingsChild->hasTagName("composition"))
{
//read out the resolution from the comp file
File compFile = settingsChild->getStringAttribute("startupFileName");
if (compFile.exists())
{
ScopedPointer<XmlElement> composition;
composition = (XmlDocument::parse ( compFile ));
forEachXmlChildElement( *composition, compChild )
{
if ( compChild->hasTagName("generalInfo"))
{
resolution.x = compChild->getIntAttribute("width", 1920);
resolution.y = compChild->getIntAttribute("height", 1080);
}
}
}
}
}
}
}

if ( slices.size() == 0 )
{
DBG("Not able to parse any slice data");
return false;
}
else
{
DBG("Slice data parsed succesfully");
return true;
}
}

bool ResXmlParser::parseRes4Xml(XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution)
{
forEachXmlChildElement( xmlTreeToParse, presetNode )
{
if ( presetNode->hasTagName("screen") )
{
forEachXmlChildElement( *presetNode, screenNode )
{
if ( screenNode->hasTagName("slices") )
{
//clear the slice array so we don't get doubles
slices.clear();

forEachXmlChildElement( *screenNode, slicesNode)
{
if ( slicesNode->hasTagName("Slice") )
{
String name;
bool enabled = slicesNode->getBoolAttribute("isEnabled");
int type = 0;
float l = 0.0f;
float t = 0.0f;
float r = 0.0f;
float b = 0.0f;

forEachXmlChildElement( *slicesNode, sliceNode)
{

if ( sliceNode->hasTagName("name") )
{
name = sliceNode->getStringAttribute("value");
}

if ( sliceNode->hasTagName("type") )
{
type = sliceNode->getIntAttribute("value");
}

if ( sliceNode->hasTagName("rect") )
{
l = float(sliceNode->getDoubleAttribute("l"));
t = float(sliceNode->getDoubleAttribute("t"));
r = float(sliceNode->getDoubleAttribute("r"));
b = float(sliceNode->getDoubleAttribute("b"));
}

}

//type 0 means only slices are loaded, no masks or crops
if ( type == 0 )
{
Slice* slice = new Slice( name, enabled );


//create 4 points out of the ltrb data
for (int i = 0; i < 4; i++ )
{
Point<float> newPoint;
switch (i)
{
case 0:
newPoint = Point<float>(l,t);
break;
case 1:
newPoint = Point<float>(r,t);
break;
case 2:
newPoint = Point<float>(r,b);
break;
case 3:
newPoint = Point<float>(l,b);
break;
}
slice->inputRectPoints.add(newPoint);
}
slices.insert(0, slice);
}
}
}
}
}
}
}

//try to get the resolution from the config.xml file and the associated comp file
File advancedLocation = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 4/preferences/config.xml";
if ( advancedLocation.exists() )
{
ScopedPointer<XmlElement> xmlRoot;
xmlRoot = (XmlDocument::parse ( advancedLocation ));
forEachXmlChildElement( *xmlRoot, avenueChild )
{
if ( avenueChild->hasTagName("settings") )
{
forEachXmlChildElement( *avenueChild, settingsChild )
{
if ( settingsChild->hasTagName("composition"))
{
//read out the resolution from the comp file
File compFile = settingsChild->getStringAttribute("startupFileName");
if (compFile.exists())
{
ScopedPointer<XmlElement> composition;
composition = (XmlDocument::parse ( compFile ));
forEachXmlChildElement( *composition, compChild )
{
if ( compChild->hasTagName("generalInfo"))
{
resolution.x = compChild->getIntAttribute("width", 1920);
resolution.y = compChild->getIntAttribute("height", 1080);
}
}
}
}
}
}
}

}

if ( slices.size() == 0 )
{
DBG("Not able to parse any slice data");
return false;
}
else
{
DBG("Slice data parsed succesfully");
return true;
}
}
*/




bool ResXmlParser::parseRes5Xml( juce::XmlElement& screenSetup, OwnedArray<Slice>& slices, Array<Screen>& screens, Point<int>& resolution )
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
	screens.clear();

	XmlElement* screensXml = screenSetup.getChildByName( "screens" );
	if ( screensXml != nullptr )
	{
		forEachXmlChildElement( *screensXml, child )
		{
			if ( child->hasTagName( "Screen" ) )
			{
				Screen newScreen;
				newScreen.name = child->getStringAttribute( "name" );
				newScreen.uid = child->getStringAttribute( "uniqueId", "0" ).getLargeIntValue();
				newScreen.folded = child->getBoolAttribute( "folded", false );
				screens.add( newScreen );


				//ignore files with the name scaling to help KBK setups
				if ( !child->getStringAttribute( "name" ).containsIgnoreCase( "scaling" ) )
				{
					XmlElement* layers = child->getChildByName( "layers" );
					if ( layers != nullptr )
					{
						forEachXmlChildElement( *layers, child )
						{
							if ( child->hasTagName( "Slice" ) || child->hasTagName( "Polygon" ) )
							{
								Slice* newSlice = new Slice();
								String uidString = child->getStringAttribute( "uniqueId", "0" );
								int64 uid = uidString.getLargeIntValue();
								String name = "Slice";
								newSlice->enabled = true;

								XmlElement* params = child->getChildByName( "Params" );
								if ( params != nullptr )
								{
									forEachXmlChildElement( *params, child )
									{
										if ( child->hasTagName( "Param" ) && child->getStringAttribute( "name" ) == "Name" )
											name = child->getStringAttribute( "value", "Slice" );

										if ( child->hasTagName( "Param" ) && child->getStringAttribute( "name" ) == "Enabled" )
											newSlice->enabled = child->getStringAttribute( "value", "1" ).getIntValue() != 0;
									}
								}

								//store the name and the uniqueId
								newSlice->sliceId = std::make_pair( name, uid );

								XmlElement* inputRect = child->getChildByName( "InputRect" );
								if ( inputRect != nullptr )
								{
									newSlice->inputRectOrientation = inputRect->getStringAttribute( "orientation", "0" ).getFloatValue();
									forEachXmlChildElement( *inputRect, child )
									{
										if ( child->hasTagName( "v" ) )
											addPointToSlice( child, newSlice->inputRectPoints, resolution );
									}
								}
								XmlElement* sliceMask = child->getChildByName( "SliceMask" );
								if ( sliceMask != nullptr )
								{
									XmlElement* shapeObject = sliceMask->getChildByName( "ShapeObject" );
									if ( shapeObject != nullptr )
									{
										XmlElement* maskRect = shapeObject->getChildByName( "Rect" );
										if ( maskRect != nullptr )
										{
											newSlice->maskRectOrientation = maskRect->getStringAttribute( "orientation", "0" ).getFloatValue();
											forEachXmlChildElement( *maskRect, child )
											{
												if ( child->hasTagName( "v" ) )
													addPointToSlice( child, newSlice->maskRectPoints, resolution );
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
													forEachXmlChildElement( *points, child )
													{
														if ( child->hasTagName( "v" ) )
															addPointToSlice( child, newSlice->maskPoints, resolution );
													}
												}
											}
										}
									}
								}

								XmlElement* contour = child->getChildByName( "InputContour" );
								if ( contour != nullptr )
								{
									XmlElement* points = contour->getChildByName( "points" );
									if ( points != nullptr )
									{
										forEachXmlChildElement( *points, child )
										{
											if ( child->hasTagName( "v" ) )
												addPointToSlice( child, newSlice->maskPoints, resolution );
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


/* no longer needed because getMainScreenSetupElement takes care of this
bool ResXmlParser::parseRes5Xml(XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution )
{
XmlElement* screenSetup = xmlTreeToParse.getChildByName("ScreenSetup");

if ( screenSetup != nullptr )
return parseRes5PrefXml(*screenSetup, slices, resolution);
else
return false;

}
*/


void ResXmlParser::addPointToSlice( juce::XmlElement *element, Array<Point<float>>& pointType, Point<int> resolution )
{
	Point<float> newPoint;
	newPoint.x = element->getStringAttribute( "x", "0.0" ).getFloatValue() / float( resolution.x );
	newPoint.y = element->getStringAttribute( "y", "0.0" ).getFloatValue() / float( resolution.y );
	pointType.add( newPoint );
}
