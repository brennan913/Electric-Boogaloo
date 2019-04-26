#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include "Pitch.hpp"

namespace smf {

constexpr float QUARTER_NOTE = 1.0; // unit is quarter notes
constexpr float WHOLE_NOTE = 4.0; // unit is quarter notes

using std::map;
using std::vector;

class Chord {
private:
    vector<Pitch> pitches;
    float length;

public:

    // Constructs a chord out of a collection of Pitches.
    Chord(const vector<Pitch> &pitches, float length = QUARTER_NOTE);

    // Construct a single note.
    Chord(Pitch pitch, float length = QUARTER_NOTE);

    // Construct an empty note, i.e. a rest.
    Chord(float length = QUARTER_NOTE);

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

    // Modify each Pitch corresponding to a key in the map. The values
    // represent the delta to change the Pitch.
    void transform(const map<int, int> &deltas);

};

} // namespace smf
