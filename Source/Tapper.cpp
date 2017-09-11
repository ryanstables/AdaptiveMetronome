//
//  Tapper.cpp
//  Metro
//
//  Created by Ryan Stables on 07/09/2017.
//
//
#include "Tapper.hpp"

//==============================================================================
//========= Tapper =============================================================
//==============================================================================
Tapper::Tapper()
{
    // some default values to be overwritten by either the tapGenerator or the editor...
    TKNoiseStd      = 5;     // 25 ms timekeeper noise
    MNoiseStd       = 5;     // 10 ms Motor noise
    MNoisePrevValue = 0;      // no prev motor noise
    setNoteLen(256);          // in samples
    setVel(127);              // in MidiNotes
    setFreq(60);              // ...
}

Tapper::~Tapper()
{
    // nothing to free up here yet...
}


void Tapper::reset()
{
    // reset to default vals...
    TKNoiseStd      = 25;     // 25 ms timekeeper noise
    MNoiseStd       = 10;     // 10 ms Motor noise
    MNoisePrevValue = 0;      // no prev motor noise
    setNoteLen(256);          // in samples
    setVel(127);              // in MidiNotes
    setFreq(60);              // ...
    
    // reset all the counters...
    countdownToOffset.reset();
    onsetTime.reset();
    numberOfNoteOns.reset();
    numberOfNoteOffs.reset();
}

void Tapper::turnNoteOn(MidiBuffer &midiMessages, int sampleNo, Counter globalCounter, bool updateMidiInOutputBuffer)
{
    if(!noteActive)
    {
        onsetTime.set(globalCounter.inSamples());
        // report the noteOn time in samples...
        printTapTime(globalCounter, "NoteOn");
        
        if(updateMidiInOutputBuffer) // this can be set to false so the input tapper doesn't write out midi messages
        {
            midiMessages.addEvent(MidiMessage::noteOn (MIDIChannel, tapperFreq, (uint8)tapperVel), sampleNo);
        }
        noteActive=true;
        numberOfNoteOns.iterate(); //update the number of NoteOffs to have happened to this tapper
    }
}

void Tapper::turnNoteOff(MidiBuffer &midiMessages, int sampleNo, Counter globalCounter, bool updateMidiInOutputBuffer)
{
    // report thee noteOff time in samples...
    if(noteActive)
    {
        if(updateMidiInOutputBuffer) // this can be set to false so the input tapper doesn't write out midi messages
            midiMessages.addEvent(MidiMessage::noteOff (MIDIChannel, tapperFreq, (uint8)tapperVel), sampleNo);
        
        noteActive=false;
        resetOffsetCounter();
        numberOfNoteOffs.iterate(); //update the number of NoteOffs to have happened to this tapper
    }
}

// set all notes off in the current midi channel...
void Tapper::kill(MidiBuffer &midiMessages)
{
    int lastEventTime = midiMessages.getLastEventTime();
    // find the last tap time and add a note-off after it
    midiMessages.addEvent(MidiMessage::allNotesOff(MIDIChannel), lastEventTime+1);
    noteActive=false;
}

bool Tapper::requiresNoteOn(Counter inputCounter)
{
    if(inputCounter.inSamples()%interval==0 && inputCounter.inSamples() && !noteActive)
        return true;
    else
        return false;
}

bool Tapper::requiresNoteOff()
{
    if (countdownToOffset.inSamples()>=noteLen && noteActive)
        return true;
    else
        return false;
}

void Tapper::iterate(MidiBuffer & midiMessages, int sampleNum, Counter &globalCounter, std::vector <bool> &notesTriggered)
{
    // check to see if noteOns/Offs need to be added....
    if(requiresNoteOn(globalCounter))
    {
        turnNoteOn(midiMessages, sampleNum, globalCounter, true);
        notesTriggered[tapperID-1] = true;  // this gets turned off by the tapManager when all notes have been triggered
        // thiis is i-1 as these are only synths and 0 is the input.
    }
    else if(requiresNoteOff())
    {
        turnNoteOff(midiMessages, sampleNum, globalCounter, true);
    }
    
    if(noteActive) // if note is turned on, start counting towards the noteOff...
        countdownToOffset.iterate();
}


void Tapper::updateParameters(int ID, int channel, int freq, int noteLen, int interval, int velocity)
{
    // function to update all parameters at once. Used for tapGen initialisation and in processBlock
    setID(ID);
    setChannel(channel); // the first one is the input tapper, start from chan 2
    setFreq(freq); //octaves above middle C
    setNoteLen(noteLen);
    setInterval(interval);
    setVel(velocity);
}

void Tapper::printTapTime(Counter globalCounter, String eventType)
{
    // use this to write the event to the log...
    Logger::outputDebugString("ID: "+String(tapperID)+", Channel: "+String(MIDIChannel)+",  "+eventType+": "+String(globalCounter.inSamples()));
}
