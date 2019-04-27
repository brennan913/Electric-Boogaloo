#pragma once

#include <string>
#include <vector>
#include "Key.hpp"
#include "MidiFile.h"
#include "Note.hpp"
#include "Pitch.hpp"
#include "Track.hpp"

namespace smf {

constexpr uint8_t TICKS_PER_QUARTER = 120;
constexpr uint8_t META_MSG = 0xff;
constexpr uint8_t TEMPO_CHANGE = 0x51;
constexpr uint8_t NOTE_ON = 0x90;
constexpr uint8_t NOTE_OFF = 0x80;

vector<uchar> getTempoMsg(int tempo);

/*
 * MIDI representation that can be written into an output file.
 */
class MidiOutput {
private:
    int tempo;
    vector<Track> tracks;

    void writeNotes(MidiFile& outputFile, Note c, Track trk, int trackNum,
        int actionTime);

public:

    MidiOutput(vector<Track> tracks = {}, int tempo = 120);

    MidiOutput(Track trk, int tempo = 120);

    int getTempo();

    void setTempo(int t);

    void addTrack(Track trk);

    void transpose(int delta);

    void modulate(const Scale &src, const Scale &dest);

    void write(string filename);
};

} // namespace smf
