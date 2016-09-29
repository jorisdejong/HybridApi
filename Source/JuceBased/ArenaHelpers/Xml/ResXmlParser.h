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

typedef std::pair<String, int64> UniquePreset;

//helper class to parse xml data in the various resolume files

class ResXmlParser
{
public:
	ResXmlParser();
	~ResXmlParser();
	
	/* parses the file we pass in, and checks if it's a Res 4 or Res 5 type
	then it calls the approriate parsing routine, which will then fill
	the passed in slice and resolution vars */
	static bool parseAssFile ( File f, OwnedArray<Slice>& slices, Point<int>& resolution );
	
	/* parses the res 5 advanced.xml file, and returns which file was used last as the preset*/
	static String getAdvancedPresetNameFromAdvancedXml( File res5XmlFile );
    
    static std::map<int, UniquePreset> getScreenNames ( File assFile );
	
	
	
private:
    /**
    This function can take both the advanced.xml as well as a saved preset file, and extract the screensetup element
     */
    static juce::XmlElement* getMainPresetElement ( File assFile );
    
	
    /*
    
	static bool parseRes4Xml( XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution );
	static bool parseRes4ConfigXml ( XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution );
	
	*/
	static bool parseRes5Xml( XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution );
	//static bool parseRes5Xml( XmlElement& xmlTreeToParse, OwnedArray<Slice>& slices, Point<int>& resolution );
	
    
	
	
	
	static void addPointToSlice ( XmlElement* sliceElement, Array<Point<float>>& pointType, Point<int> resolution);
	
	
};



#endif  // RESXMLPARSER_H_INCLUDED
