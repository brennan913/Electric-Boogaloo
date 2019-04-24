#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include "Key.hpp"
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

int DEFAULT_VELOCITY = 64;
int DEFAULT_OCTAVE = 5;
float WHOLE_NOTE_LENGTH = 4.0; // unit is quarter notes

class Track {
private:
    vector<Chord> chords;
    int octave;
    int velocity;

    void transformPitch(const map<int, int> &deltas) {
        for (Chord &c : chords) {
            c.transformPitch(deltas);
        }
    }

public:
    Track(int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY) :
        octave(octave), velocity(velocity) {}

    const vector<Chord>& getChords() { return chords; }

    const Chord& operator[](int index) const { return chords[index]; }

    Chord& operator[](int index) { return chords[index]; }

    int getVelocity() const { return velocity; }

    int getOctave() const { return octave; }

    void transpose(int delta) {
        for (Chord &c : chords) {
            c.transformPitch(delta);
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        transformPitch(src.getDifferences(dest));
    }

    void resize(float factor) {
        for (Chord &c : chords) {
            c.setLength(c.getLength() * factor);
        }
    }

    friend Track& operator<<(Track &trk, Chord c);
    friend Track& operator<<(Track &trk, const vector<Chord> &c);
    friend Track& operator<<(Track &trk, string s);
    Track &operator+=(const Track &t2);
    Track &operator*=(int factor);
    friend Track operator+(const Track &t1, const Track &t2);
    friend Track operator*(const Track &t, int factor);
};

Track& operator<<(Track &trk, Chord c) {
    trk.chords.push_back(c);
    return trk;
}

Track& operator<<(Track &trk, const vector<Chord> &cv) {
    trk.chords.reserve(trk.chords.size() + cv.size());
    for (Chord c : cv) {
        trk.chords.push_back(c);
    }
    return trk;
}

Track& operator<<(Track &trk, string s) {
    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return trk;
    }

    // reserve space >= the amount needed
    trk.chords.reserve(trk.chords.size() + tokens.size());

    float chordLength = 1.0;
    auto it = tokens.begin();
    while (it < tokens.end()) {
        string tok = *it;
        Chord c{chordLength};

        if (tok[tok.length() - 1] == '(') {
            // Start specifying note length
            int subdivision = std::stoi(tok.substr(0, tok.length() - 1));
            chordLength = WHOLE_NOTE_LENGTH / subdivision;
            ++it;
        } else if (tok.compare(")") == 0) {
            // Stop specifying note length
            chordLength = 1.0;
            ++it;
        } else {
            if (it->compare(REST) == 0) {
                // Do nothing; a chord without notes is a rest
            } else {
                // Construct chord using vector of pitches
                vector<string> pitch_tokens = tokenize_chordstr(tok);
                for (auto& pitch_token: pitch_tokens) {
                    c << Pitch{pitch_token};
                }
            }
            while(++it < tokens.end() && it->compare(EXTEND) == 0) {
                c.setLength(c.getLength() + chordLength);
            }
            trk << c;
        }
    }

    return trk;
}

Track& Track::operator+=(const Track &t2) {
    int octaveDiff = t2.octave - this->octave;
    Track temp{t2};
    temp.transpose(octaveDiff * OCTAVE_WIDTH);
    for (Chord c : temp.chords) {
        this->chords.push_back(c);
    }
    return *this;
}

Track& Track::operator*=(int factor) {
    if (factor < 0) {
        std::cerr << "Invalid factor < 0 for operator*=.\n";
        exit(1);
    }
    if (factor == 0) {
        chords = vector<Chord>{};
    } else {
        for (int i = 0; i < factor - 1; i++) {
            *this += *this;
        }
    }
    return *this;
}

Track operator+(const Track &t1, const Track &t2) {
    Track sum{t1};
    sum += t2;
    return sum;
}

Track operator*(const Track &t, int factor) {
    Track multiple{t};
    multiple *= factor;
    return multiple;
}

Track operator*(int factor, const Track &t) {
    return t * factor;
}

} // namespace smf

#endif
