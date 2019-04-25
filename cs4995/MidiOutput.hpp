#ifndef MIDI_OUTPUT_HPP_
#define MIDI_OUTPUT_HPP_

#include <string>
#include <vector>
#include "Key.hpp"
#include "MidiFile.h"
#include "Note.hpp"
#include "Track.hpp"

namespace smf {

constexpr uint8_t TICKS_PER_QUARTER = 120;
constexpr uint8_t META_MSG = 0xff;
constexpr uint8_t TEMPO_CHANGE = 0x51;
constexpr uint8_t NOTE_ON = 0x90;
constexpr uint8_t NOTE_OFF = 0x80;

vector<uchar> getTempoMsg(int tempo) {
    vector<uchar> tempoMsg(6);
    tempoMsg[0] = META_MSG;
    tempoMsg[1] = TEMPO_CHANGE;
    tempoMsg[2] = 0x03; // three bytes to follow
    int microseconds = (int)(60.0 / tempo * 1000000.0);
    tempoMsg[3] = (microseconds >> 16) & 0xff;
    tempoMsg[4] = (microseconds >> 8)  & 0xff;
    tempoMsg[5] = (microseconds >> 0)  & 0xff;
    return tempoMsg;
}

/*
 * MIDI representation that can be written into an output file.
 */
class MidiOutput {
private:
    int tempo;
    vector<Track> tracks;

    // writes notes from current collection of notes to midifile
    // does not assume single notes are a special case
    // does not handle incrementing actionTime
    // TODO integer overflow checking
    void write_notes(
        MidiFile& outputFile, Chord c, Track trk, int track_num, int actionTime)
    {
        for (Pitch p : c.getPitches()) {
            vector<uchar> midievent = {
                NOTE_ON,
                static_cast<uchar>(p.toInt() + OCTAVE_WIDTH * trk.getOctave()),
                static_cast<uchar>(trk.getVelocity())
            };

            outputFile.addEvent(track_num + 1, actionTime, midievent);

            midievent[0] = NOTE_OFF;
            outputFile.addEvent(
                track_num + 1,
                actionTime + TICKS_PER_QUARTER * c.getLength(),
                midievent);
        }
    }

public:

    MidiOutput(int tempo = 120) : tempo(tempo) {}

    int getTempo() { return tempo; }

    void setTempo(int t) { tempo = t; }

    void addTrack(Track trk) {
        tracks.push_back(trk);
    }

    void transpose(int delta) {
        for (Track &trk : tracks) {
            trk.transpose(delta);
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        for (Track &trk : tracks) {
            trk.modulate(src, dest);
        }
    }

    void write(string filename) {
        MidiFile outputFile;
        outputFile.absoluteTicks();
        outputFile.setTicksPerQuarterNote(TICKS_PER_QUARTER);
        outputFile.addTracks(tracks.size());

        // Define tempo
        auto tempoMsg = getTempoMsg(tempo);
        outputFile.addEvent(0, 0, tempoMsg);

        // Write tracks
        for (int track_num = 0; track_num < tracks.size(); track_num++) {
            int actionTime = 0;
            Track trk = tracks[track_num];

            // write all notes to midifile
            // if it's a rest, just increment actionTime
            for (Chord c : trk.getChords()) {
                if (!(c.isRest())) {
                    write_notes(outputFile, c, trk, track_num, actionTime);
                }
                actionTime += TICKS_PER_QUARTER * c.getLength();
            }
        }
        outputFile.sortTracks();
        outputFile.write(filename);
    }

};

} // namespace smf

#endif
