/*
  ==============================================================================

  FileHelper.cpp
  Created: 20 Feb 2016 5:58:44pm
  Author:  Joris de Jong

  ==============================================================================
  */

#include "FileHelper.h"
#include "../Xml/ResXmlParser.h"

FileHelper::FileHelper()
{

}

FileHelper::~FileHelper()
{

}

File FileHelper::getAssFileFromUser()
{
	//give the user a nice place to start looking
	//first try Arena 5's screensetup location
	File presetsLocation = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/presets/screensetup/";
	////if it doesn't exist, then check Arena 4's screensetup location
	if ( !presetsLocation.exists() )
		presetsLocation = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 4/presets/screensetup/";
	//if neither of those exist, start at the documents folder
	if ( !presetsLocation.exists() )
		presetsLocation = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName();

	//ask the user to pick a file
	FileChooser fc( "Pick an Arena setup file...", presetsLocation, "*.xml", true );
	if ( fc.browseForFileToOpen() )
		return fc.getResult();
	else
		return File();
}

File FileHelper::getAssFileAutomagically( bool showDialog )
{
	//check for the Arena 5 preset, this is stored in a file called advanced.xml in the res 5 preference directory
	File advancedFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/preferences/screensetup/advanced.xml";

	if ( advancedFile.exists() )
	{
		if ( showDialog ? AlertWindow::showOkCancelBox( AlertWindow::AlertIconType::QuestionIcon,
			"Res 5 setup file found!",
			"Would you like to load the current Arena setup file?",
			"OK", "Cancel" ) : true )

		{
			//parse the advanced.xml file and see if it contains the file name of the xml that is currently loaded
			String advancedName = ResXmlParser::getAdvancedPresetNameFromAdvancedXml( advancedFile );

			//if we get a name, make a File out of it and return it
			if ( advancedName != String().empty )
			{
				File namedPreset = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/presets/screensetup/" + advancedName + ".xml";
				return namedPreset;
			}
			else
				//return the advanced file itself
				//when the user does not save a named preset
				//this file can also contains ass data
				return advancedFile;
		}

	}
	//if we can't find the advanced.xml file, we're probably dealing with arena 4
	//so check for the Arena 4 preset
	else
	{
		advancedFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 4/preferences/config.xml";
		if ( advancedFile.exists() )
		{

			if ( showDialog ? AlertWindow::showOkCancelBox( AlertWindow::AlertIconType::QuestionIcon,
				"Res 4 setup file found!",
				"Would you like to load the current Arena setup file?",
				"OK", "Cancel" ) : true )
			{
				//in Arena 4, the entire current ass preset is stored inside the config xml
				return advancedFile;
			}
		}

	}
	//if everything has failed, we return an empty file
	return File();
}

File FileHelper::getArenaCompFile()
{
	File configFile = File::getSpecialLocation( File::SpecialLocationType::userDocumentsDirectory ).getFullPathName() + "/Resolume Arena 5/preferences/config.xml";
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
		"This Chaser file can't be loaded, because it was created using a different version of Chaser.",
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
