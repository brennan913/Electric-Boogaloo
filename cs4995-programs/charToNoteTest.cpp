#include <map>
#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"
#include "Pitch.hpp"

using namespace smf;

using std::map;

const Scale scale{A, MAJOR};

Pitch pitchOfScale(int degree) {
    Pitch p = scale[degree % scale.size()];
    p += (degree / scale.size()) * OCTAVE_WIDTH;
    return p;
}

Pitch charToPitch(char c, char base) {
    return pitchOfScale(c - base);
}

Pitch charToPitch(char c) {
    Pitch p;
    if (c >= 'a' && c <= 'z') {
        p = charToPitch(c, 'a');
    } else if (c >= 'A' && c <= 'Z') {
        p = charToPitch(c, 'A');
    } else if (c >= '0' && c <= '9') {
        p = charToPitch(c, '0');
    } else {
        p = Pitch{scale[c % scale.size()]};
    }
    return p;
}

Chord charToNote(char c, float length) {
    if (c == ' ' || c == ',' || c == '.') {
        return Chord{length};
    }
    return Chord{charToPitch(c), length};
}

Chord charToChord(char c, vector<int> degrees, float length) {
    if (c == ' ' || c == ',' || c == '.') {
        return Chord{length};
    }
    int delta;
    if (c >= 'a' && c <= 'z') {
        delta = c - 'a';
    } else if (c >= 'A' && c <= 'Z') {
        delta = c - 'A';
    } else if (c >= '0' && c <= '9') {
        delta = c - '0';
    } else {
        delta = c;
    }
    delta %= scale.size();
    for (int &x : degrees) {
        x += delta;
    }
    return scale.getChord(degrees, length);
}

int main() {
    string speaker = "Aaron Copland";
    Track speakerTrack(3);
    for (int i = 0; i < speaker.length(); i++) {
        speakerTrack << charToChord(speaker[i], { 0, 4, 9 }, WHOLE_LENGTH);
    }

    string quote =
        "So long as the human spirit thrives on this planet, "
        "music in some living form will accompany and sustain it";
    Track quoteTrack;
    for (int i = 0; i < quote.length(); i++) {
        quoteTrack << charToNote(quote[i], EIGHTH_LENGTH);
    }

    MidiOutput out;
    out.addTrack(speakerTrack);
    out.addTrack(quoteTrack);
    out.modulate({A, MAJOR}, {C, MINOR});
    out.write("quote.mid");
    return 0;
}
