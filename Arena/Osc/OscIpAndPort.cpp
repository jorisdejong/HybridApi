/*
  ==============================================================================

	OscIpAndPort.cpp
	Created: 17 Feb 2019 9:29:22pm
	Author:  Joris

  ==============================================================================
*/

#include "OscIpAndPort.h"

Identifier OscIpAndPort::OscIpAndPortIdentifier = Identifier( "OscIpPort" );

OscIpAndPort::OscIpAndPort()
{
}

OscIpAndPort::~OscIpAndPort()
{
}

ValueTree OscIpAndPort::toValueTree()
{
	ValueTree ipPortTree( OscIpAndPortIdentifier );
	ipPortTree.setProperty( "ip", ip.toString(), nullptr );
	ipPortTree.setProperty( "port", port, nullptr );
	return ipPortTree;
}

OscIpAndPort OscIpAndPort::createFromValueTree( ValueTree oscIpPortTree )
{
	OscIpAndPort ipPort;
	ipPort.ip = IPAddress( oscIpPortTree.getProperty( "ip" ).toString() );
	ipPort.port = oscIpPortTree.getProperty( "port" );
	return ipPort;
}

OscIpAndPort OscIpAndPort::parseFromXml( XmlElement * oscXml )
{
	OscIpAndPort ipPort;
	//Resolume binds the OSC port on the first available port that's not localhost
	//so get all the addresses
	auto ips = IPAddress::getAllAddresses();
	for ( auto ip : ips )
	{
		//try to open each port
		DatagramSocket socket;
		if ( socket.bindToPort( 0, ip.toString() ) && ip != IPAddress::local() )
		{
			DBG( "found " + ip.toString() );
			ipPort.ip = ip;
			break; //stop when we've found the first one
		}
	}

	jassert( !ipPort.ip.isNull() );//strange, we couldn't find an ip address

	//dig out the osc port from the osc preferences
	if ( oscXml )
	{
		//<Input Enabled="1" Port="7000"/>
		if ( XmlElement* inputXml = oscXml->getChildByName( "Input" ) )
			if ( inputXml->getBoolAttribute( "Enabled" ) )
				ipPort.port = inputXml->getIntAttribute( "Port" );
		delete oscXml;
	}
	return ipPort;
}
