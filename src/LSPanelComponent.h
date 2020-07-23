#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class LSPanelComponent   : public juce::Component
{
public:
    //==============================================================================
    LSPanelComponent();
    ~LSPanelComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    std::unique_ptr<juce::MidiOutput> linnout;

    class FlipCellTimer : public juce::Timer
    {
    public:
        FlipCellTimer( LSPanelComponent *pc ) : parent(pc) { }
        virtual void timerCallback() override;
        LSPanelComponent *parent;
    };
    std::unique_ptr<FlipCellTimer> linTimer;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LSPanelComponent)
};
