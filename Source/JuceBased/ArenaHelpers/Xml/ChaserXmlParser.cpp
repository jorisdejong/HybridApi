/*
  ==============================================================================

  ChaserXmlParser.cpp
  Created: 20 Feb 2016 11:51:34pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "ChaserXmlParser.h"
#include "../File/FileHelper.h"

ChaserXmlParser::ChaserXmlParser()
{

}

ChaserXmlParser::~ChaserXmlParser()
{

}

std::map<int, std::pair<String, int64>> ChaserXmlParser::getChaserSequenceNames( juce::File chaserFile, bool onlyActiveChasers )
{
	std::map<int, std::pair<String, int64>> chasers;

	if ( !FileHelper::isFileValid( chaserFile ) )
		return chasers;

	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* sequences = chaserData->getChildByName( "sequences" );
		if ( sequences )
		{
			int count = 0;
			forEachXmlChildElement( *sequences, sequence )
			{
				//check if it has any filled steps, otherwise it's pointless to add it
				forEachXmlChildElement( *sequence, step )
				{
					//so if the step has any children, add it and then break out of the loop
					if ( step->getNumChildElements() > 0 || onlyActiveChasers == false )
					{
						chasers[ sequence->getIntAttribute( "nr" ) ] = std::make_pair( sequence->getStringAttribute( "name" ), count );
						count++;
						break;
					}
				}
			}
		}
	}

	return chasers;
}

Time ChaserXmlParser::getLastUpdateTimeForActiveAssFile()
{
	File assFile;
	parseAssFile( FileHelper::getLastUsedChaserFile(), assFile );
	return assFile.getLastModificationTime();
}

void ChaserXmlParser::parseAssFile( juce::File chaserFile, juce::File& assFile )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* slicesXml = chaserData->getChildByName( "slices" );
		if ( slicesXml )
		{
			assFile = File( slicesXml->getStringAttribute( "assFile", String().empty ) );
		}
	}
}

void ChaserXmlParser::parseResolution( juce::File chaserFile, Point<int>& resolution )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* slicesXml = chaserData->getChildByName( "slices" );
		if ( slicesXml )
		{
			resolution.x = slicesXml->getIntAttribute( "width", 1920 );
			resolution.y = slicesXml->getIntAttribute( "height", 1080 );
		}
	}
}

XmlElement ChaserXmlParser::parseSequences( juce::File chaserFile )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* sequences = chaserData->getChildByName( "sequences" );
		if ( sequences )
		{
			return *sequences;
		}
	}

	return XmlElement( "empty" );
}




void ChaserXmlParser::parseSlices( juce::File chaserFile, OwnedArray<Slice>& slices )
{
	slices.clear();

	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* slicesXml = chaserData->getChildByName( "slices" );
		if ( slicesXml )
		{
			//keep track of how many slices we've processed
			//we use this to give numbers when we
			//can't retrieve them from xml
			int sliceCount = 0;

			forEachXmlChildElement( *slicesXml, slice )
			{
				Slice* newSlice = new Slice();
				newSlice->sliceId.second = slice->getStringAttribute( "name", "Slice " + String( sliceCount + 1 ) );
				newSlice->sliceId.first = slice->getStringAttribute( "uniqueId", String( sliceCount ) ).getLargeIntValue();
				newSlice->enabled = slice->getBoolAttribute( "enable", true );
				newSlice->screenId.second = slice->getStringAttribute( "screenName", "Screen 1" );
				newSlice->screenId.first = slice->getStringAttribute( "screenId", String( 0 ) ).getLargeIntValue();

				forEachXmlChildElement( *slice, pointData )
				{
					if ( pointData->hasTagName( "inputRect" ) )
					{
						newSlice->inputRectOrientation = float( pointData->getDoubleAttribute( "orientation" ) );
						forEachXmlChildElement( *pointData, pointXml )
						{
							float x = float( pointXml->getDoubleAttribute( "x" ) );
							float y = float( pointXml->getDoubleAttribute( "y" ) );
							Point<float> point( x, y );
							newSlice->inputRectPoints.add( point );
						}
					}
					else if ( pointData->hasTagName( "mask" ) )
					{
						forEachXmlChildElement( *pointData, pointXml )
						{
							float x = float( pointXml->getDoubleAttribute( "x" ) );
							float y = float( pointXml->getDoubleAttribute( "y" ) );
							Point<float> point( x, y );
							newSlice->maskPoints.add( point );
						}
					}
				}

				slices.add( newSlice );
			}
		}
	}
}

bool ChaserXmlParser::canThisAppVersionLoadThisChaser( juce::String version, juce::File chaserFile )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData != nullptr )
	{
		if ( chaserData->getChildByName( "version" ) != nullptr )
		{
			String savedVersion = chaserData->getChildByName( "version" )->getAllSubText();
			return isVersionNewer( savedVersion, version );
		}
	}
	return false;
}

bool ChaserXmlParser::isVersionNewer( juce::String savedVersion, juce::String thisVersion )
{
	//if the savedversion string is empty, it's always out of date
	if ( savedVersion.isEmpty() )
		return false;

	//versions always have three components so break them up to individual parts
	Array<int> savedVersionInts = subDivideString( savedVersion );
	Array<int> thisVersionInts = subDivideString( thisVersion );

	//check each level
	//if they all match or are newer, we're good to go
	if ( savedVersionInts[ 0 ] >= thisVersionInts[ 0 ] )
		if ( savedVersionInts[ 1 ] >= thisVersionInts[ 1 ] )
			if ( savedVersionInts[ 2 ] >= thisVersionInts[ 2 ] )
				return true;

	return false;
}

XmlElement* ChaserXmlParser::getRoot( File chaserFile )
{
	if ( chaserFile.exists() )
	{
		//read in the xml data
		XmlDocument dataDoc( chaserFile );
		return dataDoc.getDocumentElement();
	}

	return nullptr;
}

Array<int> ChaserXmlParser::subDivideString( juce::String s )
{
	Array<int> returnIntArray;
	StringArray vArray;
	vArray.addTokens( s, ".", "\"" );
	for ( int i = 0; i < vArray.size(); i++ )
		returnIntArray.add( vArray[ i ].getIntValue() );
	return returnIntArray;
}