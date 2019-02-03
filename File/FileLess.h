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

class FileLess
{
public:
	FileLess();
	~FileLess();

	PopupMenu getMenu();

	/**will return true when the file was loaded and parsed succesfully*/
	bool loadExistingFile();
	void saveAsFile();
	void createNewFile();

	/**will replace the ValueTree if it already exists*/
	void addTree( ValueTree tree );
	void removeTree( Identifier toRemove );
	ValueTree loadTree( Identifier id );

	void saveToFile();

private:
	ValueTree mainTree;
	File saveFile;

	void setLastUsedFile();
	void writeLastUsedFileName();

	File getAppFolder();
};



#endif  // FILELESS_H_INCLUDED
