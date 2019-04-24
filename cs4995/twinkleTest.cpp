#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody, bass(4);
    melody << "C C G G A A G . F F E E D D C -";
    bass << "C_1 C E C F C E C D B_1 C A_1 F_1 G_1 C_1 -";

    // combine tracks
    MidiOutput out;
    out.addTrack(melody);
    out.addTrack(bass);

    // write tracks
    out.write("twinkle_major.mid");

    // modulate and write
    out.modulate({C, MAJOR}, {F+1, MINOR});
    out.write("twinkle_minor.mid");

    out.modulate({F+1, MINOR}, {"C Db Eb F G Ab Bb"});
    out.write("twinkle_weird.mid");

    MidiOutput thrice;
    Track t1 = melody + melody + 1 * bass;
    Track t2 = bass * 2 + melody + 0 * melody;
    t1.setVelocity(100);
    t2.setVelocity(50);
    t1.resize(0.5);
    t2.resize(0.5);
    thrice.addTrack(t1);
    thrice.addTrack(t2);
    thrice.write("twinkle_thrice.mid");

    return 0;
}
