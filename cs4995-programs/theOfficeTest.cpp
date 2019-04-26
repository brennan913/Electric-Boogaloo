#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    melody << "16( EG C EG - - - EG C EG B_1 EG - - - EG B_1 "
                  "EA C EA - - - EA C FA - F - F E D E ) C";

    // combine tracks
    MidiOutput out;
    out.setTempo(80);
    out.addTrack(melody);

    // write tracks
    out.write("the_office.mid");

    // modulate and write
    out.modulate({C, MAJOR}, {C, MINOR});
    out.write("the_office_sad.mid");

    return 0;
}
