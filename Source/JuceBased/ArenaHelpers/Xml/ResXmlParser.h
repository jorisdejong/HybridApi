/*
  ==============================================================================

  ResXmlParser.h
  Created: 9 Sep 2015 8:45:41pm
  Author:  Joris de Jong

  ==============================================================================
  */

#ifndef RESXMLPARSER_H_INCLUDED
#define RESXMLPARSER_H_INCLUDED

#include "JuceHeader.h"
#include "../Slice/Slice.h"
#include <map>

//helper class to parse xml data in the various resolume files

class ResXmlParser
{
public:
	//automatically fetches the files needed to get the data
	//and tries to parse them
	ResXmlParser();
	~ResXmlParser();

	/* parses the file we pass in, and checks if it's a Res 4 or Res 5 type
	then it calls the approriate parsing routine, which will then fill
	the passed in slice and resolution vars */
	//static bool parseAssFile( File f, OwnedArray<hybrid::Slice>& slices, Array<hybrid::Screen>& screens, Point<int>& resolution );
	File getAssFile();
	void setAssFile();
	void setSlices( OwnedArray<hybrid::Slice>& slices );


	/*
	todo, implement these as seperate functions
	static OwnedArray<hybrid::Slice>& getSlices(File f);
	static Array<hybrid::Screen>& getScreens(File f);
	*/

	/* parses a comp file, scanning for clips that are set to smpte
	the resulting xml will contain a list of the clips': 
	name, layer, column, input, offset */
	static XmlElement* getSmpteData(File f);

	
	/* parses the res 5 advanced.xml file, and returns which file was used last as the preset
	static String getAdvancedPresetNameFromAdvancedXml( File res5XmlFile );
	*/

	Array< hybrid::NamedUniqueId > getScreenNames();



private:
	/**
	This function can take both the advanced.xml as well as a saved preset file, and extract the screensetup element
	
	static juce::XmlElement* getMainPresetElement( File assFile );
	*/

	static bool parseRes5Xml( XmlElement& xmlTreeToParse, OwnedArray<hybrid::Slice>& slices, Array<hybrid::Screen>& screens, Point<int>& resolution );
	
	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType, Point<int> resolution );

	File getAppFolder();
	File getPrefsFolder();
	XmlElement* getConfigXml();
	void setAssXml();
	void setCompXml();

	ScopedPointer<XmlElement> compXml;
	ScopedPointer<XmlElement> assXml;
	File assFile;
};



#endif  // RESXMLPARSER_H_INCLUDED
