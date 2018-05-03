/*
  ==============================================================================

    OscOutput.h
    Created: 9 Jan 2016 11:50:09pm
    Author:  Joris de Jong

  ==============================================================================
*/

#ifndef OSCOUTPUT_H_INCLUDED
#define OSCOUTPUT_H_INCLUDED

#include "../../../JuceLibraryCode/JuceHeader.h"

class OscOutput : private OSCSender
{
public:
	OscOutput();
	~OscOutput();

	void setOutput( IPAddress ip, int port );

	void sendMessage( OSCAddressPattern address, float value );
	void sendMessage( OSCMessage m );
	void sendMessageArray( Array<OSCMessage> );

	IPAddress getIP();

private:
	bool connected;
	IPAddress ip;
	int port;
};



#endif  // OSCOUTPUT_H_INCLUDED
