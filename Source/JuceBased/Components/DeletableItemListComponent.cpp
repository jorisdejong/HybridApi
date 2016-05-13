/*
  ==============================================================================

    DeletableItemListComponent.cpp
    Created: 13 May 2016 2:37:46pm
    Author:  Joris de Jong

  ==============================================================================
*/

#include "DeletableItemListComponent.h"

//==============================================================================


////ITEM!!!!

DeletableItem::DeletableItem( String itemName )
{
    label = new Label();
    label->setText( itemName, dontSendNotification );
    label->setColour( Label::textColourId, Colours::whitesmoke );
    addAndMakeVisible( label );
    
    deleteButton = new TextButton("-");
    addAndMakeVisible( deleteButton );
}

DeletableItem::~DeletableItem()
{
    label = nullptr;
    deleteButton = nullptr;
}

void DeletableItem::paint(juce::Graphics &g)
{
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void DeletableItem::resized()
{
    label->setBoundsRelative ( 0.01, 0.01, 0.74, 0.98 );
    deleteButton->setBoundsRelative ( 0.75, 0.01, 0.24, 0.98 );
}

///////// ITEMLIST

DeletableItemListComponent::DeletableItemListComponent()
{
    itemListBox.setModel( this );
    itemListBox.setColour( ListBox::backgroundColourId, Colours::transparentBlack );
    addAndMakeVisible( itemListBox );
}

DeletableItemListComponent::~DeletableItemListComponent()
{
    items.clear();
}

void DeletableItemListComponent::addItem(juce::String newItemName)
{
    DeletableItem* newItem = new DeletableItem ( newItemName );
    items.add( newItem );
    itemListBox.updateContent();
    repaint();
}

int DeletableItemListComponent::getNumRows()
{
    return items.size();
}

void DeletableItemListComponent::paintListBoxItem(int, juce::Graphics &g, int, int, bool)
{

}

Component* DeletableItemListComponent::refreshComponentForRow(int rowNumber, bool, juce::Component *existingComponentToUpdate)
{
    
    DeletableItem* item = (DeletableItem*) existingComponentToUpdate;
    
    //the components in the listbox are not tied to the list of items, it's only the ones that are visible
    if ( rowNumber < items.size() )
    {
        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if ( item == 0)
            item = new DeletableItem( String() );
        
        item->label->setText(items[rowNumber]->label->getText(), dontSendNotification);
    }
    else
    {
        // Nothing to display, free the custom component
        delete existingComponentToUpdate;
        item = nullptr;
    }
    
    return item;
}

void DeletableItemListComponent::paint (Graphics& g)
{
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void DeletableItemListComponent::resized()
{
    itemListBox.setBoundsInset({1,1,1,1});
}
