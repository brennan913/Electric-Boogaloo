#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include "Pitch.hpp"

namespace smf {

// In MIDI format, the unit of length is a quarter note.
constexpr float QUARTER_LENGTH = 1.0;
constexpr float HALF_LENGTH = 2.0;
constexpr float WHOLE_LENGTH = 4.0;
constexpr float EIGHTH_LENGTH = 0.5;
constexpr float SIXTEENTH_LENGTH = 0.25;
constexpr float DEFAULT_LENGTH = QUARTER_LENGTH;

using std::map;
using std::vector;

class Chord {
private:
    vector<Pitch> pitches;
    float length;

public:

    // Constructs a chord out of a collection of Pitches.
    Chord(const vector<Pitch> &pitches, float length = DEFAULT_LENGTH);

    // Construct a single note.
    Chord(Pitch pitch, float length = DEFAULT_LENGTH);

    // Construct an empty note, i.e. a rest.
    Chord(float length = DEFAULT_LENGTH);

    vector<Pitch> getPitches();
    float getLength() const;
    void setLength(float l);
    bool isRest() const;
    bool isNote() const;
    const Pitch& operator[](int index) const;
    Pitch& operator[](int index);
    Chord& operator+=(int delta);
    Chord& operator-=(int delta);
    Chord& operator^=(int delta);
    friend Chord operator+(const Chord &c, int delta);
    friend Chord operator+(int delta, const Chord &p);
    friend Chord operator-(const Chord &c, int delta);
    friend Chord operator^(const Chord &c, int delta);
    friend Chord& operator<<(Chord &c, Pitch p);

    // Modify each Pitch represented by a key in the map. Each value
    // represents the delta for changing the corresponding Pitch.
    void transform(const map<int, int> &deltas);

};

} // namespace smf
