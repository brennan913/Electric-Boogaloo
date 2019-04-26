#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include "Pitch.hpp"
#include "Key.hpp"

namespace smf {

using std::map;
using std::vector;

class Chord {
private:
    vector<Pitch> pitches;
    float length; // quarter note == 1.0

public:

    // constructs a chord out of a collection of pitches
    Chord(vector<Pitch> pitches, float length = 1.0);

    // individual notes are not a special case, just chords
    // with only one pitch
    Chord(Pitch pitch, float length = 1.0);

    // constructs a chord from the specified notes in a scale
    Chord(Scale s, vector<int> degrees, float length = 1.0);

    Chord(float length = 1.0);

    vector<Pitch> getPitches();
    float getLength();
    void setLength(float l);
    bool isRest();
    bool isNote();

    const Pitch& operator[](int index) const;
    Pitch& operator[](int index);
    Chord& operator+=(int delta);
    Chord& operator-=(int delta);
    Chord& operator^=(int delta);

    void transform(const map<int, int> &deltas);

    friend Chord operator+(const Chord &c, int delta);
    friend Chord operator+(int delta, const Chord &p);
    friend Chord operator-(const Chord &c, int delta);
    friend Chord operator^(const Chord &c, int delta);
    friend Chord& operator<<(Chord &c, Pitch p);
};

vector<string> tokenizeChordstr(string str);

vector<Chord> parseChords(string s);

} // namespace smf
