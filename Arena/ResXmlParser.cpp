/*
  ==============================================================================

  ResXmlParser.cpp
  Created: 9 Sep 2015 8:45:41pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "ResXmlParser.h"
#include "../File/FileHelper.h"
#include "Clip/Clip.h"

ResXmlParser::ResXmlParser()
{
	setCompXml();
	setAssXml();
}

ResXmlParser::~ResXmlParser()
{

}

void ResXmlParser::setCompXml()
{
	if ( XmlElement* configXml = getConfigXml() )
	{
		String fileName;
		if ( XmlElement* appXml = configXml->getChildByAttribute( "name", "Application" ) )
			if ( XmlElement* settingsXml = appXml->getChildByAttribute( "name", "Settings" ) )
				if ( XmlElement* currentCompXml = settingsXml->getChildByAttribute( "name", "CurrentCompositionFile" ) )
					fileName = currentCompXml->getStringAttribute( "value" );

		if ( File( fileName ).existsAsFile() )
			compXml = XmlDocument::parse( File( fileName ) );
		else
			DBG( "Couldn't find current comp file" );

		delete configXml;
	}
}

String ResXmlParser::getElementName( XmlElement * element )
{
	forEachXmlChildElementWithTagName( *element, paramsXml, "Params" )
	{
		forEachXmlChildElementWithTagName( *paramsXml, paramXml, "Param" )
		{
			if ( paramXml->getStringAttribute( "name" ) == "Name" )
				return paramXml->getStringAttribute( "value" );
		}
	}
	return String();
}

XmlElement * ResXmlParser::getOscXml()
{
	File xmlFile = File( getPrefsFolder().getFullPathName() + "/osc.xml" );
	return XmlDocument::parse( xmlFile );
}

void ResXmlParser::setAssXml()
{
	File presetFileConfig = File( getPrefsFolder().getFullPathName() + "/AdvancedOutput.xml" );
	if ( presetFileConfig.existsAsFile() )
	{
		if ( XmlElement* screenSetupXml = XmlDocument::parse( presetFileConfig ) )
		{
			String presetFile = screenSetupXml->getStringAttribute( "presetFile" );
			if ( presetFile.isEmpty() ) // no seperate preset file saved, so the data is stored in this xmlelement itself
			{
				assXml = screenSetupXml; //assXml is scopedpointer, so we become owner
				assFile = presetFileConfig;
			}
			else
			{
				assFile = File( getAppFolder().getFullPathName() + "/Presets/Advanced Output/" + presetFile + ".xml" );
				if ( ScopedPointer<XmlElement> presetFileXml = XmlDocument::parse( assFile ) )
				{
					assXml = new XmlElement( *presetFileXml->getChildByName( "ScreenSetup" ) );
					delete screenSetupXml;
				}
			}
		}
	}
}

File ResXmlParser::getAssFile()
{
	return assFile;
}

Point<int> ResXmlParser::getCompSize()
{
	if ( assXml )
    {
		if ( XmlElement* curCompTexSize = assXml->getChildByName( "CurrentCompositionTextureSize" ) ) // method for > 6.1.2
			return Point<int>( curCompTexSize->getIntAttribute( "width", 1920 ), curCompTexSize->getIntAttribute( "height", 1080 ) );
		else if ( XmlElement* sizing = assXml->getChildByName( "sizing" ) ) //method for < 6.0.11
			forEachXmlChildElementWithTagName( *sizing, inputs, "inputs" )
				forEachXmlChildElement( *inputs, inputSize )
					if ( inputSize->getStringAttribute( "name" ) == "0:1" )
						return Point<int>( inputSize->getIntAttribute( "width", 1920 ), inputSize->getIntAttribute( "height", 1080 ) );
    }

	if ( compXml ) //patch for 6.0.11, 6.1.0 and 6.1.1
		if ( XmlElement* compositionInfoXml = compXml->getChildByName( "CompositionInfo" ) )
			return Point<int>( compositionInfoXml->getIntAttribute( "width", 1920 ), compositionInfoXml->getIntAttribute( "height", 1080 ) );

	return Point<int>( 1920, 1080 );
}

String  ResXmlParser::getCompName()
{
	if ( XmlElement* compositionInfoXml = compXml->getChildByName( "CompositionInfo" ) )
		return compositionInfoXml->getStringAttribute( "name" );
	return String();
}

Array<hybrid::Slice> ResXmlParser::getSlices()
{
	/*
	//get the resolution from the "sizing" element
	//we later need the resolution to store the slices in the 0...1 range
	Point<int> resolution = Point<int>{ 0,0 };
	XmlElement* sizing = assXml->getChildByName( "sizing" );
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
	if ( resolution == Point<int>{ 0, 0 } )
	{
		resolution = getCompSize();
	}
	*/

	Array<hybrid::Slice> slices;

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
								hybrid::Slice newSlice;
								String uidString = layerChild->getStringAttribute( "uniqueId", "0" );
								int64 uid = uidString.getLargeIntValue();
								String name = "Slice";
								newSlice.enabled = true;

								XmlElement* params = layerChild->getChildByName( "Params" );
								if ( params != nullptr )
								{
									forEachXmlChildElement( *params, paramChild )
									{
										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Name" )
											name = paramChild->getStringAttribute( "value", "Slice" );

										if ( paramChild->hasTagName( "Param" ) && paramChild->getStringAttribute( "name" ) == "Enabled" )
											newSlice.enabled = paramChild->getStringAttribute( "value", "1" ).getIntValue() != 0;
									}
								}

								//store the name and the uniqueId
								newSlice.sliceId = std::make_pair( name, uid );

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
										XmlElement* maskRect = shapeObject->getChildByName( "Rect" );
										if ( maskRect != nullptr )
										{
											newSlice.maskRectOrientation = maskRect->getStringAttribute( "orientation", "0" ).getFloatValue();
											forEachXmlChildElement( *maskRect, vertex )
											{
												if ( vertex->hasTagName( "v" ) )
													addPointToSlice( vertex, newSlice.maskRectPoints );
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

	return slices;
}

/**
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
		xmlRoot = ( XmlDocument::parse( f ) );

		//see if we're dealing with a res 4 or res 5 ass file
		//then try to parse it
		bool succesfulParse = false;

		ScopedPointer<XmlElement> mainPreset = getMainPresetElement( f );
		if ( mainPreset )
			succesfulParse = parseRes5Xml( *mainPreset, slices, screens, resolution );

		return succesfulParse;
	}
}
*/
/*
XmlElement * ResXmlParser::getSmpteData( File f )
{
	if ( !f.exists() )
		return nullptr;
	else
	{
		DBG( "Trying to parse: " + f.getFullPathName() );

		ScopedPointer<XmlElement> avc = XmlDocument::parse( f );
		if ( avc )
		{
			if ( avc->hasTagName( "Composition" ) )
			{
				XmlElement* returnElement = new XmlElement( "SmpteClips" );
				returnElement->setAttribute( "name", avc->getStringAttribute( "name" ) );
				forEachXmlChildElement( *avc, deck )
				{
					if ( deck->hasTagName( "Deck" ) )
					{
						String deckName;
						forEachXmlChildElement( *deck, deckChild )
						{
							if ( deckChild->hasTagName( "Params" ) )
							{
								forEachXmlChildElement( *deckChild, param )
								{
									if ( param->getStringAttribute( "name" ) == "Name" )
										deckName = param->getStringAttribute( "value" );
								}
							}
							else if ( deckChild->hasTagName( "Clip" ) )
							{
								int column = deckChild->getIntAttribute( "columnIndex" );
								int layer = deckChild->getIntAttribute( "layerIndex" );
								String clipName;
								int transportType = 0;
								int offset = 0;
								forEachXmlChildElement( *deckChild, clipChild )
								{
									if ( clipChild->hasTagName( "Params" ) )
									{
										forEachXmlChildElement( *clipChild, param )
										{
											if ( param->getStringAttribute( "name" ) == "Name" )
												clipName = param->getStringAttribute( "value" );
											else if ( param->getStringAttribute( "name" ) == "TransportType" )
												transportType = param->getIntAttribute( "value" );
										}
									}
									else if ( clipChild->hasTagName( "Transport" ) )
									{
										if ( XmlElement* params = clipChild->getChildByName( "Params" ) )
											if ( XmlElement* position = params->getChildByAttribute( "name", "Position" ) )
												if ( XmlElement* bDouble = position->getChildByAttribute( "name", "BehaviourDouble" ) )
													if ( XmlElement* pSourceSmpte = bDouble->getChildByAttribute( "name", "PhaseSourceSMPTE" ) )
														if ( XmlElement* moreParams = pSourceSmpte->getChildByAttribute( "name", "Params" ) )
															if ( XmlElement* offsetNode = moreParams->getChildByAttribute( "name", "Offset" ) )
																offset = offsetNode->getIntAttribute( "value" );
									}
								}
								if ( transportType == 2 || transportType == 3 )
								{
									XmlElement* clip = new XmlElement( "Clip" );
									clip->setAttribute( "name", clipName );
									clip->setAttribute( "layer", layer );
									clip->setAttribute( "column", column );
									clip->setAttribute( "line", transportType - 2 );
									clip->setAttribute( "offset", offset );
									clip->setAttribute( "deck", deckName );
									returnElement->addChildElement( clip );
								}
							}
						}
					}
				}
				return returnElement;
			}
			else
				return nullptr;
		}
		else
			return nullptr;
	}
}*/
/*
<Composition name="">
	<Deck name="Deck" uniqueId="1504783438820" closed="0" numLayers="3" numColumns="8" deckIndex="0">
		<Params name="Params">
			<Param name="Name" default="empty" value="Desktop"/>
		<Clip name="Clip" uniqueId="1504783438824" layerIndex="0" columnIndex="1">
			<Params name="Params">
				<Param storage="3" name="Name" default="Resolume Testcard" value="Resolume Testcard"/>
				<ParamChoice storage="3" name="TransportType" default="0" value="2" storeChoices="0"/>
			<Transport name="Transport">
				<Params name="Params">
					<ParamRange name="Position" default="0" value="0">
						<BehaviourDouble name="BehaviourDouble">
							<PhaseSourceSMPTE name="PhaseSourceSMPTE">
								<Params name="Params">
									<Param name="Offset" default="0" value="4985000"/> stored in frames, so we'll need to do some parsing based on the fps set in prefs
								</Params>
*/
/*
Array< hybrid::NamedUniqueId > ResXmlParser::getScreenNames()
{
	Array< hybrid::NamedUniqueId > names;

	if ( assXml != nullptr )
	{
		XmlElement* screens = assXml->getChildByName( "screens" );
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
	}
	return names;
}
*/
/*
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
			if ( child->hasTagName( "Screen" ) || child->hasTagName( "DmxScreen" ) )
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
							if ( layerChild->hasTagName( "Slice" ) || layerChild->hasTagName( "Polygon" ) || layerChild->hasTagName( "DmxSlice" ) )
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
*/



void ResXmlParser::addPointToSlice( juce::XmlElement *element, Array<Point<float>>& pointType )
{
	Point<float> newPoint;
	newPoint.x = element->getStringAttribute( "x", "0.0" ).getFloatValue();
	newPoint.y = element->getStringAttribute( "y", "0.0" ).getFloatValue();
	pointType.add( newPoint );
}

File ResXmlParser::getAppFolder()
{
	return File( File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 6" );
}

File ResXmlParser::getPrefsFolder()
{
	return File( getAppFolder().getFullPathName() + "/Preferences" );
}

XmlElement* ResXmlParser::getConfigXml()
{
	//get the current comp file
	File prefsFile = File( getPrefsFolder().getFullPathName() + "/config.xml" );
	return XmlDocument::parse( prefsFile );
}



/*
bool ResXmlParser::parseRes4ConfigXml( juce::XmlElement &xmlTreeToParse, OwnedArray<Slice>& slices, Point<int> &resolution )
{
	//clear the slice array so we don't get doubles
	slices.clear();
	forEachXmlChildElement( xmlTreeToParse, avenueChild )
	{
		if ( avenueChild->hasTagName( "settings" ) )
		{
			forEachXmlChildElement( *avenueChild, settingsChild )
			{
				if ( settingsChild->hasTagName( "video" ) )
				{
					forEachXmlChildElement( *settingsChild, videoChild )
					{
						if ( videoChild->hasTagName( "settings" ) )
						{
							forEachXmlChildElement( *videoChild, videoSettingsChild )
							{
								if ( videoSettingsChild->hasTagName( "screenSetup" ) )
								{
									forEachXmlChildElement( *videoSettingsChild, screenSetupChild )
									{
										if ( screenSetupChild->hasTagName( "screens" ) )
										{
											forEachXmlChildElement( *screenSetupChild, screensChild )
											{
												if ( screensChild->hasTagName( "advancedScreens" ) )
												{
													forEachXmlChildElement( *screensChild, advancedScreensChild )
													{
														if ( advancedScreensChild->hasTagName( "screen" ) )
														{
															forEachXmlChildElement( *advancedScreensChild, screenNode )
															{
																if ( screenNode->hasTagName( "slices" ) )
																{
																	forEachXmlChildElement( *screenNode, slicesNode )
																	{
																		if ( slicesNode->hasTagName( "Slice" ) )
																		{
																			String name;
																			bool enabled = slicesNode->getBoolAttribute( "isEnabled" );
																			int type = 0;
																			float l = 0.0f;
																			float t = 0.0f;
																			float r = 0.0f;
																			float b = 0.0f;

																			forEachXmlChildElement( *slicesNode, sliceNode )
																			{
																				if ( sliceNode->hasTagName( "name" ) )
																					name = sliceNode->getStringAttribute( "value" );
																				if ( sliceNode->hasTagName( "type" ) )
																					type = sliceNode->getIntAttribute( "value" );
																				if ( sliceNode->hasTagName( "rect" ) )
																				{
																					l = float( sliceNode->getDoubleAttribute( "l" ) );
																					t = float( sliceNode->getDoubleAttribute( "t" ) );
																					r = float( sliceNode->getDoubleAttribute( "r" ) );
																					b = float( sliceNode->getDoubleAttribute( "b" ) );
																				}
																			}
																			//type 0 means only slices are loaded, no masks or crops
																			if ( type == 0 )
																			{
																				Slice* slice = new Slice( name, enabled );
																				//create 4 points out of the ltrb data
																				for ( int i = 0; i < 4; i++ )
																				{
																					Point<float> newPoint;
																					switch ( i )
																					{
																					case 0:
																						newPoint = Point<float>( l, t );
																						break;
																					case 1:
																						newPoint = Point<float>( r, t );
																						break;
																					case 2:
																						newPoint = Point<float>( r, b );
																						break;
																					case 3:
																						newPoint = Point<float>( l, b );
																						break;
																					}
																					slice->inputRectPoints.add( newPoint );
																				}
																				slices.insert( 0, slice );
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
				if ( settingsChild->hasTagName( "composition" ) )
				{
					//read out the resolution from the comp file
					File compFile = settingsChild->getStringAttribute( "startupFileName" );
					if ( compFile.exists() )
					{
						ScopedPointer<XmlElement> composition;
						composition = (XmlDocument::parse( compFile ));
						forEachXmlChildElement( *composition, compChild )
						{
							if ( compChild->hasTagName( "generalInfo" ) )
							{
								resolution.x = compChild->getIntAttribute( "width", 1920 );
								resolution.y = compChild->getIntAttribute( "height", 1080 );
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

bool ResXmlParser::parseRes4Xml( XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution )
{
	//clear the slice array so we don't get doubles
	slices.clear();
	forEachXmlChildElement( xmlTreeToParse, presetNode )
	{
		if ( presetNode->hasTagName( "screen" ) )
		{
			forEachXmlChildElement( *presetNode, screenNode )
			{
				if ( screenNode->hasTagName( "slices" ) )
				{

					forEachXmlChildElement( *screenNode, slicesNode )
					{
						if ( slicesNode->hasTagName( "Slice" ) )
						{
							String name;
							bool enabled = slicesNode->getBoolAttribute( "isEnabled" );
							int type = 0;
							float l = 0.0f;
							float t = 0.0f;
							float r = 0.0f;
							float b = 0.0f;

							forEachXmlChildElement( *slicesNode, sliceNode )
							{
								if ( sliceNode->hasTagName( "name" ) )
									name = sliceNode->getStringAttribute( "value" );
								if ( sliceNode->hasTagName( "type" ) )
									type = sliceNode->getIntAttribute( "value" );
								if ( sliceNode->hasTagName( "rect" ) )
								{
									l = float( sliceNode->getDoubleAttribute( "l" ) );
									t = float( sliceNode->getDoubleAttribute( "t" ) );
									r = float( sliceNode->getDoubleAttribute( "r" ) );
									b = float( sliceNode->getDoubleAttribute( "b" ) );
								}
							}
							//type 0 means only slices are loaded, no masks or crops
							if ( type == 0 )
							{
								Slice* slice = new Slice( name, enabled );
								//create 4 points out of the ltrb data
								for ( int i = 0; i < 4; i++ )
								{
									Point<float> newPoint;
									switch ( i )
									{
									case 0:
										newPoint = Point<float>( l, t );
										break;
									case 1:
										newPoint = Point<float>( r, t );
										break;
									case 2:
										newPoint = Point<float>( r, b );
										break;
									case 3:
										newPoint = Point<float>( l, b );
										break;
									}
									slice->inputRectPoints.add( newPoint );
								}
								slices.insert( 0, slice );
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
		xmlRoot = (XmlDocument::parse( advancedLocation ));
		forEachXmlChildElement( *xmlRoot, avenueChild )
		{
			if ( avenueChild->hasTagName( "settings" ) )
			{
				forEachXmlChildElement( *avenueChild, settingsChild )
				{
					if ( settingsChild->hasTagName( "composition" ) )
					{
						//read out the resolution from the comp file
						File compFile = settingsChild->getStringAttribute( "startupFileName" );
						if ( compFile.exists() )
						{
							ScopedPointer<XmlElement> composition;
							composition = (XmlDocument::parse( compFile ));
							forEachXmlChildElement( *composition, compChild )
							{
								if ( compChild->hasTagName( "generalInfo" ) )
								{
									resolution.x = compChild->getIntAttribute( "width", 1920 );
									resolution.y = compChild->getIntAttribute( "height", 1080 );
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
		DBG( "Not able to parse any slice data" );
		return false;
	}
	else
	{
		DBG( "Slice data parsed succesfully" );
		return true;
	}
}

*/

