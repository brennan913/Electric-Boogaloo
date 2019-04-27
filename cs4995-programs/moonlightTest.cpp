#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    Track arpeggio{"8( F A D^1 )"};
    Track chord1{"80( F A D^1 ) 13( . )"};
    Track chord2{"80( E A D^1 ) 13( . )"};
    Track chord3{"80( E A C^1 ) 13( . )"};

    // create tracks
    Track melody{"8( E A C^1 )"};
    melody *= 8;
    for (int i = 0; i < 8; i++) {
        melody += arpeggio;
        arpeggio.resize(0.8);
    }
    melody += 2 * (chord1 * 8 + chord2 * 3 + chord3 * 5);
    Track bass{"2( A/A^1 - - G/G^1 - - D/D^1 - - )", 2};

    // combine tracks
    MidiOutput out{ {melody, bass}, 80 };

    // write tracks
    out.write("moonlight.mid");

    return 0;
}
