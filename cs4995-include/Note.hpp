#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include "Pitch.hpp"

namespace smf {

using std::map;
using std::vector;

class Chord {
private:
    vector<Pitch> pitches;
    float length; // quarter note == 1.0

public:

    // Constructs a chord out of a collection of Pitches.
    Chord(vector<Pitch> pitches, float length = 1.0);

    // Construct a single note.
    Chord(Pitch pitch, float length = 1.0);

    // Construct an empty note, i.e. a rest.
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
