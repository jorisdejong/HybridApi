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
#include "Osc/OscIpAndPort.h"


/** base helper class to parse xml data in the various resolume files */
class ResXmlParser
{
public:
	/**finds the files needed to get the data
	and parses them */
	ResXmlParser();
	~ResXmlParser();

	File getAssFile();

	Point<int> getCompSize();
	String getCompName();

	/**get all the slices from the ass*/
	Array<hybrid::Slice> getSlices();

	OscIpAndPort getIpAndPort();

protected:
	/** the resolume comp file as xml
	can be accessed by other parser implementations */
	ScopedPointer<XmlElement> compXml;

	/** the resolume ass file as xml
	can be accessed by other parsers implementations */
	ScopedPointer<XmlElement> assXml;
	
	/** helper function to get the value of an xmlelement's "name" attribute */
	String getElementName( XmlElement* element );

private:	
	static void addPointToSlice( XmlElement* sliceElement, Array<Point<float>>& pointType );

	/** functions that quickly get the correct file based on Res' folder structure */
	File getAppFolder();
	File getPrefsFolder();
	XmlElement* getConfigXml();
	XmlElement* getOscXml();

	void setAssXml();
	void setCompXml();

	File assFile;
};



#endif  // RESXMLPARSER_H_INCLUDED
