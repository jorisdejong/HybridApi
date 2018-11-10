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

	File getAssFile();

	Point<int> getCompSize();
	String getCompName();

	/*get all the slices from the ass*/
	Array<hybrid::Slice> getSlices();

	/*neatly arrange the relevant data into an xmlelement*/
	XmlElement* getXml();

	struct Clip
	{
		int64 uniqueId;
		int column;
		int layer;
		int deck;
		String name;
		//to reconstruct the thumbfile name: filename for files, uid for generators
		String thumbFileData; 
		//the actual pixels as base64
		String thumbData; 
	};
	/*
	Scrape the comp for all the clips
	*/
	Array<Clip> getClips();

	
	
private:	
	ScopedPointer<XmlElement> resData;
	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType );

	File getAppFolder();
	File getPrefsFolder();
	XmlElement* getConfigXml();
	void setAssXml();
	void setCompXml();

	ScopedPointer<XmlElement> compXml;
	ScopedPointer<XmlElement> assXml;
	File assFile;

	struct XmlStorable
	{ 
		String name;
		String type;
		String value;

		XmlElement* toXml() 
		{ 
			XmlElement* r = new XmlElement( "Property" );
			r->setAttribute( "name", name );
			r->setAttribute( "type", type );
			r->setAttribute( "value", value );
			return r;
		}
	};

	String getElementName( XmlElement* element );

	
};



#endif  // RESXMLPARSER_H_INCLUDED
