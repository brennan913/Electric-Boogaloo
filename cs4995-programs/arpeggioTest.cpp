#include "MidiOutput.hpp"

using namespace smf;

int main() {

    Track arpeggio{"8( C E G )"};
    // create tracks
    Track trk;
    for (int i = 0; i < 25; i++) {
        trk += arpeggio;
        arpeggio.transpose(1);
        arpeggio.resize(0.9);
    }
    MidiOutput out{ trk };
    out.write("arpeggios.mid");

    return 0;
}
