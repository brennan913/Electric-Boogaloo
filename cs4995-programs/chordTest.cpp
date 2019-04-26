#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    melody << "CEG - B_1/D/G - C/E/A - C/F/A -";

    // combine tracks
    MidiOutput out;
    out.addTrack(melody);

    // write tracks
    out.write("chords.mid");

    return 0;
}
