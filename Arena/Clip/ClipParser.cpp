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
	if ( !compXml )
		return;

	clips.clear();
	for ( auto* deckXml : compXml->getChildWithTagNameIterator( "Deck" ) )
	{
		for ( auto* clipXml : deckXml->getChildWithTagNameIterator( "Clip" ) )
		{
			Clip clip;
			clip.deck = deckXml->getIntAttribute( "deckIndex" );
			clip.uniqueId = clipXml->getStringAttribute( "uniqueId" ).getLargeIntValue();

			clip.column = clipXml->getIntAttribute( "columnIndex" );
			clip.layer = clipXml->getIntAttribute( "layerIndex" );
			if ( XmlElement* preloadDataXml = clipXml->getChildByName( "PreloadData" ) )
				//this means we're dealing with a file
				clip.thumbFileData = preloadDataXml->getChildElement( 0 )->getStringAttribute( "value" );
			else
				//we're dealing with a generator // not 100% sure if this is full proof
			{
				/*<PrimarySource>
					<VideoSource storage="0" name="VideoSource" width="1920" height="1088" type="GeneratorVideoSource">
						<RenderPass name="RenderPassChain" type="RenderPassChain" uniqueId="RenderPassChain" baseType="RenderPassChain"/>
						<RenderPass name="Checkered" type="Checkered" uniqueId="736772465707583394" baseType="Generator">
							<Params name="Params">
								<ParamColor name="Color 1" channelmode="2" paletteEnabled="0" color="4278190080"/>
								<ParamColor name="Color 2" channelmode="2" paletteEnabled="0" color="4294967295"/>
							</Params>
						</RenderPass>
					</VideoSource>*/
				if ( XmlElement* videoTrackXml = clipXml->getChildByName( "VideoTrack" ) )
					if ( XmlElement* primarySourceXml = videoTrackXml->getChildByName( "PrimarySource" ) )
						if ( XmlElement* videoSourceXml = primarySourceXml->getChildByName( "VideoSource" ) )
						{
							for ( auto* renderPassXml : videoSourceXml->getChildWithTagNameIterator( "RenderPass" ) )
							{
								if ( renderPassXml->getStringAttribute( "baseType" ) == "Generator" )
									clip.thumbFileData = renderPassXml->getStringAttribute( "uniqueId" );
							}
						}
			}
			clip.name = getElementName( clipXml );
			for ( auto* paramsXml : clipXml->getChildWithTagNameIterator( "Params" ) )
			{
				for ( auto* paramXml : paramsXml->getChildWithTagNameIterator( "Param" ) )
				{
					if ( paramXml->getStringAttribute( "name" ) == "Name" )
						clip.defaultName = paramXml->getStringAttribute( "default" );
				}
			}

			clips.add( clip );
		}
	}
}
