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
#include "ThumbReader.h"

#include <map>

//*helper class to parse xml data in the various resolume files */
class ResXmlParser
{
public:
	/*finds the files needed to get the data
	and parses them */
	ResXmlParser();
	~ResXmlParser();

	File getAssFile();

	Point<int> getCompSize();
	String getCompName();

	/**get all the slices from the ass*/
	Array<hybrid::Slice> getSlices();

	struct Clip //basic info about the clip
	{
		int64 uniqueId = 0;
		int column = -1;
		int layer = -1;
		int deck = -1;
		String name = String();
		String defaultName = String();
	};

	/**	Scrape the comp for all the clips	*/
	Array<Clip> getClips();
	/** Scrape the comp for thumbnails */
	Array<ThumbReader::Thumbnail> getThumbs();

private:	
	ScopedPointer<XmlElement> resData;
	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType );

	File getAppFolder();
	File getPrefsFolder();
	XmlElement* getConfigXml();
	void setAssXml();
	void setCompXml();

	void parseClips();
	Array<Clip> clips;
	Array<ThumbReader::Thumbnail> thumbs;

	ScopedPointer<XmlElement> compXml;
	ScopedPointer<XmlElement> assXml;
	File assFile;

	/** helper function to get the value of an xmlelement's "name" attribute */
	String getElementName( XmlElement* element );
};



#endif  // RESXMLPARSER_H_INCLUDED
