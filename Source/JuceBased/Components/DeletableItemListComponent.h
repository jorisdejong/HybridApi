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

class DeletableItem : public Component
{
public:
    DeletableItem( String itemName );
    ~DeletableItem();
    
    ScopedPointer<Label> label;
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    ScopedPointer<TextButton> deleteButton;
    
};

class DeletableItemListComponent    : public Component, public ListBoxModel
{
public:
    DeletableItemListComponent();
    ~DeletableItemListComponent();
    
    void addItem ( String newItemName );

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
    
    class JUCE_API Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
        virtual void oscOutputRemoved ( String ip, int port ) = 0;
        virtual void oscOutputAdded ( String ip, int port ) = 0;
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
