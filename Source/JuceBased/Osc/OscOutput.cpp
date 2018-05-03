/*
  ==============================================================================

	OscOutput.cpp
	Created: 9 Jan 2016 11:50:09pm
	Author:  Joris de Jong

  ==============================================================================
*/

#include "OscOutput.h"

OscOutput::OscOutput()
{
	connected = false;
	ip = IPAddress();
	port = -1;
}

OscOutput::~OscOutput()
{
	disconnect();
}

void OscOutput::setOutput( IPAddress newIp, int newPort )
{
	if ( connected )
	{
		if ( !disconnect() ) //try to disconnect
		{
			DBG( "Could not disconnect from current UDP port" );
			return;
		}
	}

	if ( connected = connect( newIp.toString(), port ) )
	{
		ip = newIp;
		port = newPort;
		DBG( "Opened OSC output to " + ip.toString() + " and port " + String( port ) );
	}
	else
		DBG( "Could not open OSC output!" );
}



void OscOutput::sendMessage( OSCAddressPattern address, float value )
{
	OSCMessage m = OSCMessage( address );
	m.addFloat32( value );
	sendMessage( m );
}

void OscOutput::sendMessage( juce::OSCMessage m )
{
	if ( port < 0 )
		return;

	if ( !m.isEmpty() )
	{
		if ( m.begin()->isFloat32() )
		{
			DBG( "Sending OSC: " + m.getAddressPattern().toString() + ": " + String( m.begin()->getFloat32() ) );
		}
		else if ( m.begin()->isInt32() )
		{
			DBG( "Sending OSC: " + m.getAddressPattern().toString() + ": " + String( m.begin()->getInt32() ) );
		}
		else if ( m.begin()->isString() )
		{
			DBG( "Sending OSC: " + m.getAddressPattern().toString() + ": " + String( m.begin()->getString() ) );
		}

		send( m );
	}
}

void OscOutput::sendMessageArray( Array<juce::OSCMessage> messages )
{
	for ( int i = 0; i < messages.size(); i++ )
	{
		sendMessage( messages.getUnchecked( i ) );
	}
}

IPAddress OscOutput::getIP()
{
	return ip;
}
