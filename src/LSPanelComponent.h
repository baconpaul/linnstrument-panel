#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class LSPanelComponent;

struct LSUpdate : public juce::Timer
{
    LSUpdate(LSPanelComponent *c) : parent(c){}
    virtual ~LSUpdate() = default;
    virtual void addChangeToBuffer( juce::MidiBuffer &mb, int column, int row, int color ) {
        mb.addEvent( juce::MidiMessage::controllerEvent( 0, 20, column ), 0 );
        mb.addEvent( juce::MidiMessage::controllerEvent( 0, 21, row ), 1 );
        mb.addEvent( juce::MidiMessage::controllerEvent( 0, 22, color ), 2 );
    }
    virtual void clearPanel() {
        for( int i=1; i<26; ++i )
        {
            for( int j=0; j<8; ++j )
            {
                // FIXME - this API should really let us compress messages into a single buffer
                juce::MidiBuffer mb;
                addChangeToBuffer(mb, i, j, 7 );
                sendBuffer(mb);
            }
        }
    }
    virtual void sendBuffer( juce::MidiBuffer &mb );
    virtual void updatePanel( std::unique_ptr<juce::MidiOutput> &out ) = 0;

    LSPanelComponent *parent;
};


struct LSOneShot : public LSUpdate {
    LSOneShot( LSPanelComponent *c ) : LSUpdate( c ) {}
    ~LSOneShot() = default;
    void timerCallback();
};

struct LSForever : public LSUpdate {
    LSForever( LSPanelComponent *c ) : LSUpdate( c ) {}
    ~LSForever() { stopTimer(); };

    void timerCallback();
};

class LSPanelComponent   : public juce::Component,
                           public juce::Button::Listener
{
public:
    //==============================================================================
    LSPanelComponent();
    ~LSPanelComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked( juce::Button *b ) override;
    
    std::unique_ptr<juce::MidiOutput> linnout;

private:
    std::unique_ptr<LSUpdate> updater;

    std::unique_ptr<juce::Button> flipButton, lifeButton, waveButton;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LSPanelComponent)
};

inline void LSOneShot::timerCallback() {
    updatePanel( parent->linnout );
    stopTimer();
}

inline void LSForever::timerCallback() {
    updatePanel( parent->linnout );
}

inline void LSUpdate::sendBuffer(juce::MidiBuffer &mb)
{
    if( parent->linnout )
        parent->linnout->sendBlockOfMessagesNow( mb );
}
