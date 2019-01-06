/*
  ==============================================================================

	FileLess.cpp
	Created: 12 Mar 2017 6:00:52pm
	Author:  Joris

  ==============================================================================
*/

#include "FileLess.h"
#include "FileHelper.h"

PopupMenu FileLess::getMenu()
{
	PopupMenu menu;
	menu.addItem( 1, "New" );
	menu.addSeparator();

	menu.addItem( 2, "Load" );
	menu.addItem( 3, "Save as..." );
	return menu;
}

File FileLess::getNewFile()
{
	File folder = getAppFolder();
	return folder.getChildFile( "New" + String( ProjectInfo::projectName ) + "Document.xml" );
}

File FileLess::saveAs()
{
	String dialog = "Save file as...";
	juce::FileChooser saver( dialog, getAppFolder(), "*.xml" );
	if ( saver.browseForFileToSave( true ) )
	{
		if ( !saver.getResult().existsAsFile() )
			saver.getResult().create();
		return saver.getResult();
	}
	return File();
}

File FileLess::load()
{
	juce::FileChooser loader( "Load new...", getAppFolder(), "*.xml" );
	if ( loader.browseForFileToOpen() )
		return loader.getResult();

	return File();
}


File FileLess::getLastUsedFileName()
{
	File appFolder = getAppFolder();
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
	File newFile = getNewFile();
	writeLastUsedFileName( newFile );
	return newFile;
}

void FileLess::writeLastUsedFileName(File newSavefile )
{
	File appFolder = getAppFolder( );

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

File FileLess::getAppFolder()
{
	//the preferences file is stored in the userDocs
	File docDir = File::getSpecialLocation( File::userDocumentsDirectory );

	File appFolder = docDir.getChildFile( ProjectInfo::projectName );
	if ( !appFolder.exists() )
		appFolder.createDirectory();

	return appFolder;
}
