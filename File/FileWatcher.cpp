/*
  ==============================================================================

	FileWatcher.cpp
	Created: 3 Feb 2019 3:51:23pm
	Author:  Joris

  ==============================================================================
*/

#include "FileWatcher.h"

FileWatcher::FileWatcher() :
	theFile( File() ),
	lastUpdateTime( 0 )
{
	startTimerHz( 1 );
}

FileWatcher::~FileWatcher() 
{
	
}

void FileWatcher::watchFile( File newFile )
{
	theFile = newFile;
	lastUpdateTime = theFile.getLastModificationTime().toMilliseconds();
}



void FileWatcher::timerCallback()
{
	int64 updateTime = theFile.getLastModificationTime().toMilliseconds();
	if ( updateTime > lastUpdateTime )
	{
		lastUpdateTime = updateTime;
		fileUpdated( theFile );
	}
}

