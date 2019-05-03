#include "MidiOutput.hpp"
#include <exception>
#include <climits>

static void assert_no_uchar_overflow(int i){
    if (i > UCHAR_MAX){
	throw std::underflow_error(
	    "provided integer values to write() exceed"
	    "8-bit maximum for midifile event format"
	    );
    }
}

namespace smf {

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

// writes notes from current collection of notes to midifile
// does not assume single notes are a special case
// does not handle incrementing actionTime
void MidiOutput::writeNotes(
    MidiFile& outputFile, Note note, Track trk, int trackNum, int actionTime)
{
    for (Pitch p : note.getPitches()) {

	// if provided int values exceed uchar maximum, 
	//throw exception to avoid overflow
        assert_no_uchar_overflow(p.toInt() + OCTAVE_WIDTH * trk.getOctave());
	assert_no_uchar_overflow(trk.getVelocity());

	vector<uchar> midievent = {
            NOTE_ON,
            static_cast<uchar>(p.toInt() + OCTAVE_WIDTH * trk.getOctave()),
            static_cast<uchar>(trk.getVelocity())
        };

        outputFile.addEvent(trackNum + 1, actionTime, midievent);

        midievent[0] = NOTE_OFF;
        outputFile.addEvent(
            trackNum + 1,
            actionTime + TICKS_PER_QUARTER * note.getLength(),
            midievent);
    }
}

MidiOutput::MidiOutput(vector<Track> tracks, int tempo) :
    tempo(tempo), tracks(tracks) {}

MidiOutput::MidiOutput(Track trk, int tempo) : tempo(tempo) {
    tracks.push_back(trk);
}

int MidiOutput::getTempo() { return tempo; }

void MidiOutput::setTempo(int t) { tempo = t; }

void MidiOutput::addTrack(Track trk) {
    tracks.push_back(trk);
}

void MidiOutput::transpose(int delta) {
    for (Track &trk : tracks) {
        trk.transpose(delta);
    }
}

void MidiOutput::resize(float factor) {
    for (Track &trk : tracks) {
        trk.resize(factor);
    }
}

void MidiOutput::modulate(const Scale &src, const Scale &dest) {
    for (Track &trk : tracks) {
        trk.modulate(src, dest);
    }
}

// can throw std::overflow_error from writeNotes()
void MidiOutput::write(string filename) {
    MidiFile outputFile;
    outputFile.absoluteTicks();
    outputFile.setTicksPerQuarterNote(TICKS_PER_QUARTER);
    outputFile.addTracks(tracks.size());

    // Define tempo
    auto tempoMsg = getTempoMsg(tempo);
    outputFile.addEvent(0, 0, tempoMsg);

    // Write tracks
    for (int trackNum = 0; trackNum < tracks.size(); trackNum++) {
        int actionTime = 0;
        Track trk = tracks[trackNum];

        // write all notes to midifile
        // if it's a rest, just increment actionTime
        for (Note note : trk.getNotes()) {
            if (!(note.isRest())) {
                writeNotes(outputFile, note, trk, trackNum, actionTime);
            }
            actionTime += TICKS_PER_QUARTER * note.getLength();
        }
    }
    outputFile.sortTracks();
    outputFile.write(filename);
}

} // namespace smf
