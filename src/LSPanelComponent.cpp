#include "LSPanelComponent.h"
#include <iostream>

struct Flip : public LSForever
{
    Flip( LSPanelComponent *c ) : LSForever(c) {
        clearPanel();
    }
    virtual void updatePanel( std::unique_ptr<juce::MidiOutput> &out ) override
    {
        int col = rand() % 25 + 1;
        int row = rand() % 8;
        int color = rand() % 11;
        juce::MidiBuffer mb;
        addChangeToBuffer( mb, col, row, color );
        if( parent->linnout )
            parent->linnout->sendBlockOfMessagesNow( mb );
    }
};
struct Wave : public LSForever {
    Wave( LSPanelComponent *c ) : LSForever(c) {
        clearPanel();
    }

    int t = 2;
    virtual void updatePanel( std::unique_ptr<juce::MidiOutput> &out ) override {
        int pcol = t-1;
        if( pcol < 0 ) pcol = 24;
        int ppcol = pcol - 1;
        if( ppcol < 0 ) ppcol = 24;
        pcol ++;
        ppcol ++;
        int col = t+1;

        for( int j=0; j<8; ++j )
        {
            {
                juce::MidiBuffer mb;
                addChangeToBuffer(mb, ppcol, j, 7 );
                if( parent->linnout )
                    parent->linnout->sendBlockOfMessagesNow( mb );
            }

            {
                juce::MidiBuffer mb;
                addChangeToBuffer(mb, pcol, j, 4 );
                if( parent->linnout )
                    parent->linnout->sendBlockOfMessagesNow( mb );
            }

            {
                juce::MidiBuffer mb;
                addChangeToBuffer(mb, col, j, 5 );
                if( parent->linnout )
                    parent->linnout->sendBlockOfMessagesNow( mb );
            }
        }
        
        t++;
        if( t == 25 ) t = 0;
    }
};

struct Life : public LSForever {
    int cells[25][8], ncells[25][8];
    int colors[6] = { 2, 3, 4, 5, 10, 8 };
    
    Life( LSPanelComponent *c ) : LSForever(c) {
        clearPanel();
        for( int i=0; i<25; ++i )
            for( int j=0; j<8; ++j )
            {
                cells[i][j] = 0;
                
                if( rand() % 100 > 50 )
                {
                    cells[i][j] = 1;
                }
            }
    }

    int ct = 3;
    virtual void updatePanel( std::unique_ptr<juce::MidiOutput> &out ) override {
        if( ct++ != 3 )
            return;
        ct = 0;
        // OK so implement life from cells -> ncells
        for( int i=0; i<25; ++i )
            for( int j=0; j<8; ++j )
            {
                auto im = i-1; if( im < 0 ) im = 24;
                auto ip = i+1; if( ip > 24 ) ip = 0;
                auto jm = j-1; if( jm < 0 ) jm = 24;
                auto jp = j+1; if( jp > 7 ) ip = 0;

                auto n = 
                    (cells[im][jm]?1:0) + (cells[im][j]?1:0) + (cells[im][jp]?1:0) +
                    (cells[i ][jm]?1:0) +                      (cells[i ][jp]?1:0) +
                    (cells[ip][jm]?1:0) + (cells[ip][j]?1:0) + (cells[ip][jp]?1:0);
                auto v = cells[i][j];
                if( v )
                {
                    if( n == 2 || n == 3 )
                    {
                        ncells[i][j] = v + 1;
                    }
                    else
                    {
                        ncells[i][j] = 0;
                    }
                }
                else if( n == 3 )
                {
                    ncells[i][j] = 1;
                }
            }

        // and now blast it out - for now in just white
        for( int i=0; i<25; ++i )
        {
            for( int j=0; j<8; ++j )
            {
                if( ncells[i][j] != cells[i][j] )
                {
                    juce::MidiBuffer mb;
                    auto clr = 7;
                    if( ncells[i][j] )
                    {
                        auto ci = ncells[i][j];
                        if( ci > 5 ) ci = 5;
                        clr = colors[ci];
                    }
                    addChangeToBuffer(mb, i, j, clr );
                    if( parent->linnout )
                        parent->linnout->sendBlockOfMessagesNow( mb );
                }
                cells[i][j] = ncells[i][j];
            }
        }
    }
};

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
                // linTimer = std::make_unique<FlipCellTimer>(this);
                // linTimer->startTimer( 25 ); // 4 times a second
            }
        }
    }

    float y0 = 10;
    float dy = 25;
    flipButton = std::make_unique<juce::TextButton>( "Flip" );
    flipButton->setBounds( 10, y0, 70, dy );
    flipButton->addListener(this);
    flipButton->setButtonText( TRANS( "Flip" ) );
    addAndMakeVisible( flipButton.get() );
    y0 += dy+3;

    lifeButton = std::make_unique<juce::TextButton>( "Life" );
    lifeButton->setBounds( 10, y0, 70, dy );
    lifeButton->addListener(this);
    lifeButton->setButtonText( TRANS( "Life" ) );
    addAndMakeVisible( lifeButton.get() );
    y0 += dy+3;

    waveButton = std::make_unique<juce::TextButton>( "Wave" );
    waveButton->setBounds( 10, y0, 70, dy );
    waveButton->addListener(this);
    waveButton->setButtonText( TRANS( "Wave" ) );
    addAndMakeVisible( waveButton.get() );
    y0 += dy+3;

    
}

void LSPanelComponent::buttonClicked( juce::Button *b )
{
    if( b == flipButton.get() )
    {
        updater = std::make_unique<Flip>( this );
    }
    if( b == lifeButton.get() )
    {
        updater = std::make_unique<Life>(this);
    }
    if( b == waveButton.get() )
    {
        updater = std::make_unique<Wave>( this );
    }
    
    if( updater )
        updater->startTimer( 100 );
}

LSPanelComponent::~LSPanelComponent()
{
}


//==============================================================================
void LSPanelComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    if( linnout )
        g.drawText ("LinnStrument Found and Connected", getLocalBounds(), juce::Justification::centred, true);
    else
        g.drawText ("No Device Connected", getLocalBounds(), juce::Justification::centred, true);
}

void LSPanelComponent::resized()
{
    // This is called when the LSPanelComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
