/*
  ==============================================================================

	ClipParser.cpp
	Created: 30 Jan 2019 10:16:20pm
	Author:  Joris

  ==============================================================================
*/

#include "ClipParser.h"

ClipParser::ClipParser()
{
	//at this point, ResXmlParser should have created the compXml already
	parseClips();
}

ClipParser::~ClipParser()
{
}

Array<Clip> ClipParser::getClips()
{
	return clips;
}

void ClipParser::parseClips()
{
	jassert( compXml );
	if ( !compXml )
		return;

	clips.clear();
	forEachXmlChildElementWithTagName( *compXml, deckXml, "Deck" )
	{
		forEachXmlChildElementWithTagName( *deckXml, clipXml, "Clip" )
		{
			Clip clip;
			clip.deck = deckXml->getIntAttribute( "deckIndex" );
			clip.uniqueId = clipXml->getStringAttribute( "uniqueId" ).getLargeIntValue();
			
			clip.column = clipXml->getIntAttribute( "columnIndex" );
			clip.layer = clipXml->getIntAttribute( "layerIndex" );
			if ( XmlElement* preloadDataXml = clipXml->getChildByName( "PreloadData" ) )
				//this means we're dealing with a file
				clip.thumbFileData = preloadDataXml->getChildElement( 0 )->getStringAttribute( "value" );
			clip.name = getElementName( clipXml );
			forEachXmlChildElementWithTagName( *clipXml, paramsXml, "Params" )
			{
				forEachXmlChildElementWithTagName( *paramsXml, paramXml, "Param" )
				{
					if ( paramXml->getStringAttribute( "name" ) == "Name" )
						clip.defaultName = paramXml->getStringAttribute( "default" );
				}
			}

			clips.add( clip );
		}
	}
}
