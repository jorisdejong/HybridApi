/*
  ==============================================================================

  ChaserXmlParser.cpp
  Created: 20 Feb 2016 11:51:34pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "ChaserXmlParser.h"
#include "../File/FileHelper.h"
#include "../../Fileless/FileLess.h"

ChaserXmlParser::ChaserXmlParser()
{

}

ChaserXmlParser::~ChaserXmlParser()
{

}

Array<hybrid::NamedUniqueId> ChaserXmlParser::getChaserSequenceNames( juce::File chaserFile, bool /*onlyActiveChasers*/ )
{
	Array<hybrid::NamedUniqueId> chasers;

	if ( !FileHelper::isFileValid( chaserFile, false ) )
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
				while ( count != sequence->getIntAttribute( "nr" ) ) //fill with empty sequences
				{
					chasers.add( std::make_pair( "Sequence " + String(count), count ) );
					count++;
				}
				chasers.add( std::make_pair( sequence->getStringAttribute("name"), count ) );
				count++;
			}
		}
	}

	return chasers;
}

Time ChaserXmlParser::getLastUpdateTimeForActiveAssFile()
{
	File assFile;
	parseAssFile( FileLess::getLastUsedFileName(FileLess::Chaser), assFile );
	return assFile.getLastModificationTime();
}

bool ChaserXmlParser::parseAssFile( File chaserFile, juce::File& assFile )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* slicesXml = chaserData->getChildByName( "slices" );
		if ( slicesXml )
		{
			assFile = File( slicesXml->getStringAttribute( "assFile", String() ) );
			return true;
		}
	}
	return false;
}

bool ChaserXmlParser::parseResolution( File chaserFile, Point<int>& resolution )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		XmlElement* slicesXml = chaserData->getChildByName( "slices" );
		if ( slicesXml )
		{
			resolution.x = slicesXml->getIntAttribute( "width", 1920 );
			resolution.y = slicesXml->getIntAttribute( "height", 1080 );

			return true;
		}
	}

	return false;
}

bool ChaserXmlParser::parseScreens( File chaserFile, Array<hybrid::Screen>& screens )
{
	screens.clear();
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData )
	{
		if ( XmlElement* screensXml = chaserData->getChildByName( "screens" ) )
		{
			forEachXmlChildElement( *screensXml, screenXml )
			{
				hybrid::Screen newScreen;
				newScreen.name = screenXml->getStringAttribute( "name" );
				newScreen.uid = screenXml->getStringAttribute( "uniqueId" ).getLargeIntValue();
				newScreen.folded = screenXml->getBoolAttribute( "folded", false );
				screens.add( newScreen );
			}
		}
		return true;
	}
	return false;
}

bool ChaserXmlParser::parseSlices( File chaserFile, OwnedArray<hybrid::Slice>& slices )
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
				hybrid::Slice* newSlice = new hybrid::Slice();
				newSlice->sliceId.first = slice->getStringAttribute( "name", "Slice " + String( sliceCount + 1 ) );
				newSlice->sliceId.second = slice->getStringAttribute( "uniqueId", String( sliceCount ) ).getLargeIntValue();
				newSlice->enabled = slice->getBoolAttribute( "enable", true );
				newSlice->screenId = slice->getStringAttribute( "screenId", String( 0 ) ).getLargeIntValue();

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

		return true;
	}

	return false;
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



bool ChaserXmlParser::canThisAppVersionLoadThisChaser( File chaserFile, juce::String version )
{
	ScopedPointer<XmlElement> chaserData = getRoot( chaserFile );
	if ( chaserData != nullptr )
	{
		XmlElement* versionXml = chaserData->getChildByName( "version" );
		if ( versionXml )
		{
			String savedVersion = versionXml->getStringAttribute( "nr" );
			return isVersionNewer( savedVersion, version );
		}
		else
		{
			//if we can't extract the version tag, it means we're always
			//dealing wih a different version
			FileHelper::throwVersionError();
			return false;
		}
	}

	FileHelper::throwEmptyError();
	return false;
}

bool ChaserXmlParser::isVersionNewer( juce::String savedVersion, juce::String thisVersion )
{
	//if the savedversion string is empty, it's always out of date
	if ( savedVersion.isEmpty() )
	{
		FileHelper::throwVersionError();
		return false;
	}

	//versions always have three components so break them up to individual parts
	Array<int> savedVersionInts = subDivideString( savedVersion );
	Array<int> thisVersionInts = subDivideString( thisVersion );

	//check each level
	//if the major version doesn't match, we're no good
	if ( savedVersionInts[ 0 ] != thisVersionInts[ 0 ] )
		FileHelper::throwVersionError();
	else
		//if the rest matches or is newer, we're good to go
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