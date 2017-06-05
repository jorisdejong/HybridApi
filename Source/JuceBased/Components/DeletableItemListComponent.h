/*
  ==============================================================================

    DeletableItemListComponent.h
    Created: 13 May 2016 2:37:46pm
    Author:  Joris de Jong

  ==============================================================================
*/

#ifndef DELETABLEITEMLISTCOMPONENT_H_INCLUDED
#define DELETABLEITEMLISTCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
/*
*/

class DeletableItemListComponent;

class DeletableItem : 
	public Component,
	public juce::Button::Listener
{
public:
    DeletableItem( String itemName, DeletableItemListComponent* parent );
    ~DeletableItem();

	void buttonClicked( Button* b ) override;
    
    ScopedPointer<Label> label;
    
    void paint (Graphics&) override;
    void resized() override;

	ScopedPointer<TextButton> deleteButton;
    
private:
	DeletableItemListComponent* parent;
    
    
};

class DeletableItemListComponent    : public Component, public ListBoxModel
{
public:
    DeletableItemListComponent();
    ~DeletableItemListComponent();

	void itemDeleteButtonClicked( String removedName );
    
    void addItem ( String newItemName );

	/** the listener is reponsible for removing the item
	it will be notified via a callback that the user wants to delete an item, 
	then when it has succesfully removed the object the item is linked to
	it can call this function to remove it from the list
	*/
	void removeItem( String removedName );
	void removeAllItems();

	/** not implemented yet */
	void renameItem( int index, String newName );

    void paint (Graphics&) override;
    void resized() override;
    
    //Listbox methods
    int getNumRows() override;
    void paintListBoxItem (int rowNumber,
                                   Graphics& g,
                                   int width, int height,
                                   bool rowIsSelected) override;
    Component* refreshComponentForRow (int rowNumber, bool isRowSelected,
                                               Component* existingComponentToUpdate) override;
	void listBoxItemClicked( int row, const MouseEvent& ) override;
	void selectedRowsChanged( int lastRowSelected ) override;
	void backgroundClicked( const MouseEvent& ) override;
    
    class JUCE_API Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
		virtual void itemRemoved( int index ) = 0;
    };
    
    void addListener (Listener* listener) { listeners.add( listener ); };
    void removeListener (Listener* listener) { listeners.remove( listener ); };
    
private:
    ListBox itemListBox;
    OwnedArray<DeletableItem> items;
    ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeletableItemListComponent)
};


#endif  // DELETABLEITEMLISTCOMPONENT_H_INCLUDED
