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
	ipPort.ip = IPAddress( oscIpPortTree.getProperty( "ip" ) );
	ipPort.port = oscIpPortTree.getProperty( "port" );
	return ipPort;
}
