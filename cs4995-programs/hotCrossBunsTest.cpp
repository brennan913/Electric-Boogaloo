#include "MidiOutput.hpp"

using namespace smf;

int main() {

    Track melody{"E D C . E D C . C C D D E D C ."};
    MidiOutput out{ melody };
    out.write("hot_cross_buns.mid");

    return 0;
}
