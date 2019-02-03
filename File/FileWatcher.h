/*
  ==============================================================================

    FileWatcher.h
    Created: 3 Feb 2019 3:51:23pm
    Author:  Joris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/** Objects derived from FileWatcher 
can specify a file
and will be notified when it changes
*/

class FileWatcher : private Timer
{
public:
	FileWatcher();
	~FileWatcher();

	/** setting a new file will never trigger a callback */
	void watchFile( File newFile );

	virtual void fileUpdated( File file ) = 0;
	
private:
	File theFile;
	int64 lastUpdateTime;

	virtual void timerCallback() override;
};