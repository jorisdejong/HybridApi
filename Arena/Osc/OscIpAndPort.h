/*
  ==============================================================================

    OscIpAndPort.h
    Created: 17 Feb 2019 9:29:22pm
    Author:  Joris

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/** a quick class to hold an IPAddress and a port together
	and to go to and from a ValueTree */
class OscIpAndPort
{
public:
	OscIpAndPort();
	~OscIpAndPort();

	IPAddress ip;
	int port;

	static Identifier OscIpAndPortIdentifier;
	ValueTree toValueTree();
	static OscIpAndPort createFromValueTree( ValueTree oscIpPortTree );

	static OscIpAndPort parseFromXml( XmlElement* oscXml );
};


