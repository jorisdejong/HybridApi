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
	enum App
	{
		Chaser,
		KbkController,
		TemplateTool
	};

	PopupMenu getMenu();

	File getNewFile( App app );
	File saveAs( App app );
	File load( App app );
	
	File getLastUsedFileName( App app );
	void writeLastUsedFileName( App app, File file );

	File getAppFolder( App app );
}



#endif  // FILELESS_H_INCLUDED
