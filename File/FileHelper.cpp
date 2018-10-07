/*
  ==============================================================================

  FileHelper.cpp
  Created: 20 Feb 2016 5:58:44pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "FileHelper.h"

FileHelper::FileHelper()
{

}

FileHelper::~FileHelper()
{

}

File FileHelper::getAssFileAutomagically( bool showDialog )
{
	File returnFile;
	//check for the Arena 6 preset, this is stored in a file called advanced.xml in the res 5 preference directory
	File advancedFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 6/Preferences/AdvancedOutput.xml";
	if ( advancedFile.exists() )
	{
		returnFile = getAssFileByVersion( advancedFile, 6, showDialog );
		if ( returnFile != File() ) //if we didn't press cancel
			return returnFile;
	}

	//we pressed cancel, or we couldn't find the res 6 folder
	advancedFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/preferences/screensetup/advanced.xml";
	if ( advancedFile.exists() )
	{
		returnFile = getAssFileByVersion( advancedFile, 5, showDialog );
		if ( returnFile != File() ) //if we didn't press cancel
			return returnFile;
	}
	//if everything has failed, we return an empty file
	return File();
}


File FileHelper::getArenaCompFileByVersion( int version )
{
	File configFile;
	switch ( version )
	{
	case( 5 ):
		configFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/preferences/config.xml";
		if ( configFile.existsAsFile() )
		{
			if ( ScopedPointer<XmlElement> configXml = XmlDocument::parse( configFile ) )
			{
				if ( XmlElement* settings = configXml->getChildByName( "settings" ) )
				{
					if ( XmlElement* composition = settings->getChildByName( "composition" ) )
					{
						return File( composition->getStringAttribute( "startupFileName" ) );
					}
				}
			}
		}
		return File();
		break;
	case ( 6 ):
		configFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 6/Preferences/config.xml";
		if ( configFile.existsAsFile() )
		{
			if ( ScopedPointer<XmlElement> arenaXml = XmlDocument::parse( configFile ) )
			{
				if ( XmlElement* appXml = arenaXml->getChildByName( "Application" ) )
				{
					forEachXmlChildElement( *appXml, paramsXml )
					{
						if ( paramsXml->getStringAttribute( "name" ) == "Settings" )
						{
							forEachXmlChildElement( *paramsXml, paramXml )
							{
								if ( paramXml->getStringAttribute( "name" ) == "CurrentCompositionFile" )
									return File( paramXml->getStringAttribute( "value" ) );
							}
						}
					}
				}
			}
		}
		return File();
		break;
	default:
		return File();
		break;
	}
}

File FileHelper::getAssFileByVersion( File advancedFile, int version, bool showDialog )
{
	if ( showDialog ? AlertWindow::showOkCancelBox( AlertWindow::AlertIconType::QuestionIcon,
		"Arena " + String( version ) + " setup file found!",
		"Would you like to load the current Arena setup file?",
		"OK", "Cancel" ) : true )

	{
		//parse the advanced.xml file and see if it contains the file name of the xml that is currently loaded
		String advancedName;
		if ( ScopedPointer<XmlElement> mainXmlElement = XmlDocument::parse( advancedFile ) )
			if ( mainXmlElement->hasAttribute( "presetFile" ) )
				advancedName = mainXmlElement->getStringAttribute( "presetFile", String() );

		//if we get a name, make a File out of it and return it
		if ( advancedName != String() )
		{
			String namedPreset = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName();// 
			switch ( version )
			{
			case 5:
				namedPreset += "/Resolume Arena 5/presets/screensetup/" + advancedName + ".xml";
				break;
			case 6:
				namedPreset += "/Resolume Arena 6/Presets/Advanced Output/" + advancedName + ".xml";
				break;
			}
			return File( namedPreset );
		}
		else
			//return the advanced file itself
			//when the user does not save a named preset
			//this file can also contains ass data
			return advancedFile;
	}

	return File();
}

bool FileHelper::isFileValid( juce::File fileToCheck, bool giveFeedback )
{
	if ( fileToCheck.existsAsFile() && fileToCheck != File() )
		return true;

	if ( giveFeedback )
	{
		AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon,
			"Whoops!",
			"That file can't be read! It looks like it's an invalid file!",
			"Ok" );
	}
	return false;
}

void FileHelper::throwLoadError()
{
	AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon,
		"Sorry!",
		"Something went wrong reading that file.",
		"Ok" );
	DBG( "Error loading file..." );
}

void FileHelper::throwSaveError()
{
	AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon,
		"Sorry!",
		"Could not save data.",
		"Ok" );
}

void FileHelper::throwVersionError()
{
	AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon,
		"Sorry!",
		"This file can't be loaded, because it was created using a different version.",
		"Ok" );
	DBG( "Error loading file..." );
}

void FileHelper::throwEmptyError()
{
	AlertWindow::showMessageBoxAsync( AlertWindow::AlertIconType::WarningIcon,
		"Whoops!",
		"This file appears to be empty.",
		"Ok" );
}
