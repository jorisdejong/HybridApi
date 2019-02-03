/*
  ==============================================================================

	FileLess.cpp
	Created: 12 Mar 2017 6:00:52pm
	Author:  Joris

  ==============================================================================
*/

#include "FileLess.h"
#include "FileHelper.h"

FileLess::FileLess()
{
	mainTree = ValueTree( Identifier( ProjectInfo::projectName ) );
	
	setLastUsedFile();

	//try to load the previous file
	if ( ScopedPointer<XmlElement> mainXml = XmlDocument::parse( saveFile ) )
		if ( mainXml->getTagName() == ProjectInfo::projectName )
			mainTree = ValueTree::fromXml( *mainXml );

	writeLastUsedFileName();
}

FileLess::~FileLess()
{

}

PopupMenu FileLess::getMenu()
{
	PopupMenu menu;
	menu.addItem( 1, "New" );
	menu.addSeparator();

	menu.addItem( 2, "Load" );
	menu.addItem( 3, "Save as..." );
	return menu;
}

bool FileLess::loadExistingFile()
{
	juce::FileChooser loader( "Load new...", getAppFolder(), "*.xml" );
	if ( loader.browseForFileToOpen() )
	{
		File result = loader.getResult();
		if ( result == File() )
		{
			FileHelper::throwEmptyError();
			return false;
		}
		else
		{
			ScopedPointer<XmlElement> mainXml = XmlDocument::parse( result );
			if ( !mainXml )
			{
				FileHelper::throwLoadError();
				return false;
			}
			else
			{
				if ( mainXml->getTagName() == ProjectInfo::projectName )
				{
					mainTree = ValueTree::fromXml( *mainXml );
					return true;
				}
				else
				{
					FileHelper::throwLoadError();
					return false;
				}
			}
		}
	}
	else
		return false;
}

void FileLess::saveAsFile()
{
	String dialog = "Save file as...";
	juce::FileChooser saver( dialog, getAppFolder(), "*.xml" );
	if ( saver.browseForFileToSave( true ) )
	{
		File result = saver.getResult();
		if ( !result.existsAsFile() )
			result.create();

		saveFile = result;
		writeLastUsedFileName();
	}
}

void FileLess::createNewFile()
{
	mainTree.removeAllChildren( nullptr );

	File folder = getAppFolder();
	saveFile = folder.getChildFile( "New" + String( ProjectInfo::projectName ) + "Document.xml" );

	if ( !saveFile.exists() )
		saveFile.create();

	writeLastUsedFileName();
}

void FileLess::addTree( ValueTree newTree )
{
	mainTree.removeChild( mainTree.getChildWithName( newTree.getType() ), nullptr );
	mainTree.addChild( newTree, -1, nullptr );
}

void FileLess::removeTree( Identifier toRemove )
{
	ValueTree deleteMe = mainTree.getChildWithName( toRemove );
	mainTree.removeChild( deleteMe, nullptr );
}

ValueTree FileLess::loadTree( Identifier id )
{
	return mainTree.getChildWithName( id );
}

void FileLess::saveToFile()
{
	ScopedPointer<XmlElement> mainXml = mainTree.createXml();
	mainXml->writeToFile( saveFile, "" );
	writeLastUsedFileName();
}


void FileLess::setLastUsedFile()
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
			if ( savedFile.existsAsFile() && savedFile != File() )
			{
				saveFile = savedFile;
				return;
			}
		}
	}

	//if all else fails, make a default file and save it
	createNewFile();
	saveToFile();
}

void FileLess::writeLastUsedFileName()
{
	File appFolder = getAppFolder();

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

	lastUsedFile->setAttribute( "fullpathname", saveFile.getFullPathName() );

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
