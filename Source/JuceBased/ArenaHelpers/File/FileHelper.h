/*
  ==============================================================================

    FileHelper.h
    Created: 20 Feb 2016 5:58:44pm
    Author:  Joris de Jong

  ==============================================================================
*/

#ifndef FILEHELPER_H_INCLUDED
#define FILEHELPER_H_INCLUDED

#include "JuceHeader.h"

//FileHelper takes care of all the reading of the different files

class FileHelper
{
public:
	FileHelper();
	~FileHelper();
	
	static File getAssFileFromUser();

	/*this will return either the Arena 5 or Arena 4 ass file*/
	static File getAssFileAutomagically( bool showDialog );

	static File getChaserPreferencesFile();
	static File getLastUsedChaserFile( File prefFile );

	
	static void reloadAssFile();
	
    static bool isFileValid ( File fileToCheck );
	
private:
	static void throwLoadError();
	static void throwSaveError();
};



#endif  // FILEHELPER_H_INCLUDED
