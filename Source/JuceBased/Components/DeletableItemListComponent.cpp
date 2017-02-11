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

DeletableItem::DeletableItem( String itemName, DeletableItemListComponent* parent ) : parent( parent )
{
    label = new Label();
    label->setText( itemName, dontSendNotification );
    label->setColour( Label::textColourId, Colours::whitesmoke );
    addAndMakeVisible( label );
    
    deleteButton = new TextButton("-");
    addAndMakeVisible( deleteButton );
	deleteButton->addListener( this );
}

DeletableItem::~DeletableItem()
{
    label = nullptr;
    deleteButton = nullptr;
}

void DeletableItem::buttonClicked( Button * )
{
	//only one button, call the parent
	parent->removeItem( label->getText() );
}

void DeletableItem::paint(juce::Graphics &g)
{
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void DeletableItem::resized()
{
    label->setBoundsRelative ( 0.01f, 0.01f, 0.74f, 0.98f );
    deleteButton->setBoundsRelative ( 0.75f, 0.01f, 0.24f, 0.98f );
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
    DeletableItem* newItem = new DeletableItem ( newItemName, this );
    items.add( newItem );
    itemListBox.updateContent();
    repaint();
}

void DeletableItemListComponent::removeItem( String removedName )
{
	//figure out which item matches the name
	for ( auto item : items )
	{
		if ( item->label->getText() == removedName )
		{
			//call the listeners
			BailOutChecker checker( this );
			if ( !checker.shouldBailOut() )
				listeners.callChecked( checker, &DeletableItemListComponent::Listener::itemRemoved, items.indexOf( item ) );
		}
	}
	
}

int DeletableItemListComponent::getNumRows()
{
    return items.size();
}

void DeletableItemListComponent::paintListBoxItem(int, juce::Graphics &, int, int, bool)
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
            item = new DeletableItem( String(), this );
        
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
