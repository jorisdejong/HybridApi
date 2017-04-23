/*
  ==============================================================================

	FileLess.cpp
	Created: 12 Mar 2017 6:00:52pm
	Author:  Joris

  ==============================================================================
*/

#include "FileLess.h"
#include "../ArenaHelpers/File/FileHelper.h"

PopupMenu FileLess::getMenu()
{
	PopupMenu menu;
	menu.addItem( 1, "New" );
	menu.addSeparator();

	menu.addItem( 2, "Load", false );
	menu.addItem( 3, "Save as...", false );
	return menu;
}

File FileLess::getNewFile( App app )
{
	File folder = getAppFolder( app );
	switch ( app )
	{
	case Chaser:
		return folder.getChildFile( "DefaultChaser.xml" );
		break;
	case KbkController:
		return folder.getChildFile( "config.xml" );
		break;
	case TemplateTool:
		return folder.getChildFile( "template.xml" );
		break;
	}

	return File();
}

File FileLess::saveAs( App app )
{
	String dialog = "Save file as...";
	switch ( app )
	{
	case Chaser:
		dialog = "Save Chaser as...";
		break;
	default:
		break;
	}
	
	juce::FileChooser saver( dialog, getAppFolder( app ), "*.xml" );
	if ( saver.browseForFileToSave( true ) )
		return saver.getResult();

	return File();
}

File FileLess::load( App app )
{
	juce::FileChooser loader( "Load new...", getAppFolder( app ), "*.xml" );
	if ( loader.browseForFileToOpen() )
		return loader.getResult();

	return File();
}




File FileLess::getLastUsedFileName( App app )
{
	File appFolder = getAppFolder( app );
	if ( !appFolder.exists() )
		appFolder.create();

	File prefFile = appFolder.getChildFile( "preferences/preferences.xml" );

	if ( !FileHelper::isFileValid( prefFile, false ) )
		prefFile.create();

	XmlDocument lastUsedFile( prefFile );
	ScopedPointer<XmlElement> lastUsedFileData = lastUsedFile.getDocumentElement();
	if ( lastUsedFileData )
	{
		XmlElement* lastUsedFileXml = lastUsedFileData->getChildByName( "lastusedfile" );
		if ( lastUsedFileXml )
		{
			File savedFile = File( lastUsedFileXml->getStringAttribute( "fullpathname" ) );
            if (savedFile != File() )
                return savedFile;
		}
	}

	//if all else fails, return a default file
	File newFile = getNewFile( app );
	writeLastUsedFileName( app, newFile );
	return newFile;
}

void FileLess::writeLastUsedFileName( App app, File newSavefile )
{
	File appFolder = getAppFolder( app );

	File prefFile = appFolder.getChildFile( "preferences/preferences.xml" );

	if ( !FileHelper::isFileValid( prefFile, false ) )
		prefFile.create();

	//try to parse itn
	ScopedPointer<XmlElement> lastUsedFileData( XmlDocument::parse( prefFile ) );

	if ( !lastUsedFileData )
		lastUsedFileData = new XmlElement( "preferences" );

	//try to get the lastusedfile child
	XmlElement* lastUsedFile = lastUsedFileData->getChildByName( "lastusedfile" );
	if ( !lastUsedFile )
	{
		lastUsedFile = new XmlElement( "lastusedfile" );
		lastUsedFileData->addChildElement( lastUsedFile );
	}

	lastUsedFile->setAttribute( "fullpathname", newSavefile.getFullPathName() );

	lastUsedFileData->writeToFile( prefFile, "" );
}

File FileLess::getAppFolder( App app )
{
	//the preferences file is stored in the userDocs
	File docDir = File::getSpecialLocation( File::userDocumentsDirectory );

	File appFolder;
	switch ( app )
	{
	case Chaser:
		appFolder = docDir.getChildFile( "Chaser" );
		break;
	case KbkController:
		appFolder = docDir.getChildFile( "KBK Controller" );
		break;
	case TemplateTool:
		appFolder = docDir.getChildFile( "TemplateTool" );
		break;
	}
	if ( !appFolder.exists() )
		appFolder.createDirectory();

	return appFolder;
}
