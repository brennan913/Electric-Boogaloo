#include <map>
#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

using std::map;

    string pi = "31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495";
    string e = "2718281828459045235360287471352662497757247093699959574966967627724076630353 547594571382178525166427427466391932003059921817413596629043572900334295260 595630738132328627943490763233829880753195251019011573834187930702154089149 934884167509244761460668082264800168477411853742345442437107539077744992069 551702761838606261331384583000752044933826560297606737113200709328709127443 747047230696977209310141692836819025515108657463772111252389784425056953696";

int main() {
    map<char, Pitch> digitToPitch{
        { '0', Pitch{"C"} },
        { '1', Pitch{"D"} },
        { '2', Pitch{"E"} },
        { '3', Pitch{"F"} },
        { '4', Pitch{"G"} },
        { '5', Pitch{"A"} },
        { '6', Pitch{"B"} },
        { '7', Pitch{"C6"} },
        { '8', Pitch{"D6"} },
        { '9', Pitch{"E6"} },
    };

    Track piTrack;
    for (int i = 0; i < pi.length(); i++) {
        piTrack << Chord{digitToPitch[pi[i]], 0.25};
    }
    Track eTrack;
    for (int i = 0; i < e.length(); i++) {
        eTrack << Chord{digitToPitch[e[i]]};
    }
    eTrack.transpose(-24);

    MidiOutput pieOut;
    pieOut.addTrack(piTrack);
    pieOut.addTrack(eTrack);
    pieOut.write("pie_major.mid");
    pieOut.modulate({C, MAJOR}, {C, MINOR});
    pieOut.write("pie_minor.mid");
    return 0;
}
