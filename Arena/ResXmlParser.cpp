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

	/**
	* oh noes... we haven't been able to parse the composition or ass xml files
	* the most likely cause is that there is no resolume folder in documents
	* or that the current composition or ass file doesn't actually exist
	* either way, something is seriously wrong if we come here
	*/
	//jassert( compXml && assXml );
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

void ResXmlParser::getVersionInfo( String& appName, int& majorVersion, int& minorVersion, int& microVersion, int& revision )
{
	//<versionInfo name="Resolume Arena" majorVersion="6" minorVersion="1" microVersion="1" revision="61321"/>
	if ( XmlElement* versionXml = compXml->getChildByName( "versionInfo" ) )
	{
		appName = versionXml->getStringAttribute( "name" );
		majorVersion = versionXml->getIntAttribute( "majorVersion" );
		minorVersion = versionXml->getIntAttribute( "minorVersion" );
		microVersion = versionXml->getIntAttribute( "microVersion" );
		revision = versionXml->getIntAttribute( "revision" );
	}
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