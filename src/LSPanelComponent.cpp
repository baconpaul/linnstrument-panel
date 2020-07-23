#include "LSPanelComponent.h"
#include <iostream>

//==============================================================================
LSPanelComponent::LSPanelComponent()
{
    setSize (600, 400);

    auto availableDevices = juce::MidiOutput::getAvailableDevices();
    for( auto &device : availableDevices )
    {
        std::cout << "FOUND DEVICE WITH NAME " << device.name << std::endl;
        if( device.name.startsWith( "LinnStrument" ))
        {
            std::cout << "ITS A LINN" << std::endl;
            linnout = juce::MidiOutput::openDevice( device.identifier );
            if( linnout )
            {
                std::cout << "And it's open" << std::endl;
                linTimer = std::make_unique<FlipCellTimer>(this);
                linTimer->startTimer( 25 ); // 4 times a second
            }
        }
    }
}

LSPanelComponent::~LSPanelComponent()
{
    if( linTimer ) linTimer->stopTimer();
}

void LSPanelComponent::FlipCellTimer::timerCallback()
{
    int col = rand() % 25 + 1;
    int row = rand() % 8;
    int color = rand() % 11;

    // CC20 - column
    // CC21 - row
    // CC22 - color
    juce::MidiBuffer mb;
    mb.addEvent( juce::MidiMessage::controllerEvent( 0, 20, col ), 0 );
    mb.addEvent( juce::MidiMessage::controllerEvent( 0, 21, row ), 1 );
    mb.addEvent( juce::MidiMessage::controllerEvent( 0, 22, color ), 2 );
    parent->linnout->sendBlockOfMessagesNow( mb );
}

//==============================================================================
void LSPanelComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void LSPanelComponent::resized()
{
    // This is called when the LSPanelComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
