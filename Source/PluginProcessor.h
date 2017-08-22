/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "TapManager.hpp"

//==============================================================================
/**
*/
class MetroAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MetroAudioProcessor();
    ~MetroAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateMIDIFile(String midiInput);
    
private:
    
    // MIDI Input Data...
    File inputmidifile;
    ScopedPointer<InputStream> MIDIData; // to HoldtheIncoming MIDI Data.
    OwnedArray<MidiMessageSequence> inputMIDISeq;
    
    // tappers...
    int numSynthesizedTappers    = 3;
    int numInputTappers          = 1;
    ScopedPointer<TapGenerator>  tapManager;
    
    bool tappersAlreadyAllocated = false;
    Counter globalCounter, frameCounter;
    
    Synthesiser synth;
    AudioPlayHead::CurrentPositionInfo playhead;
    double currentBPM = 0, prevBPM = 0;
    
    bool bpmValueChanged();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetroAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
