/*
  ==============================================================================

	FileLess.h
	Created: 12 Mar 2017 6:00:52pm
	Author:  Joris

  ==============================================================================
*/

#ifndef FILELESS_H_INCLUDED
#define FILELESS_H_INCLUDED

#include "JuceHeader.h"

/* FileLess helps with everything needed to manage a fileless save state
- creating the menu
- handling actions from menu
- keeping track of the preferences file
- keeping track of the savefile */

namespace FileLess
{
	PopupMenu getMenu();

	File getNewFile();
	File saveAs();
	File load();

	File getLastUsedFileName();
	void writeLastUsedFileName( File file );

	File getAppFolder();
}



#endif  // FILELESS_H_INCLUDED
