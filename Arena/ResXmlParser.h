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
	void getVersionInfo( String& appName, int& majorVersion, int& minorVersion, int& microVersion, int& revision );

	std::unique_ptr<XmlElement> getOscXml();

	XmlElement getCompXml();

protected:
	/** the resolume comp file as xml
	can be accessed by other parser implementations */
	std::unique_ptr<XmlElement> compXml;

	/** the resolume ass file as xml
	can be accessed by other parsers implementations */
	std::unique_ptr<XmlElement> assXml;

	/** helper function to get the value of an xmlelement's "name" attribute */
	String getElementName( XmlElement* element );

private:

	/** functions that quickly get the correct file based on Res' folder structure */
	File getAppFolder();
	File getPrefsFolder();
	std::unique_ptr<XmlElement> getConfigXml();

	void setAssXml();
	void setCompXml();

	File assFile;
};



#endif  // RESXMLPARSER_H_INCLUDED
