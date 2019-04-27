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

/*
 * A Note object can represent a single note, a chord (multiple notes played
 * simultaneously), or a rest.
 */
class Note {
private:
    // Empty vector represents a rest, and a vector with more than one
    // element represents a chord.
    vector<Pitch> pitches;
    float length;

public:

    // Construct a chord out of a vector of Pitches.
    Note(const vector<Pitch> &pitches, float length = DEFAULT_LENGTH);

    // Construct a single note.
    Note(Pitch pitch, float length = DEFAULT_LENGTH);

    // Construct an empty note, i.e. a rest.
    Note(float length = DEFAULT_LENGTH);

    vector<Pitch> getPitches();
    float getLength() const;
    void setLength(float l);
    bool isRest() const;
    bool isSingleNote() const;
    const Pitch& operator[](int index) const;
    Pitch& operator[](int index);
    Note& operator+=(int delta);
    Note& operator-=(int delta);
    Note& operator^=(int delta);
    friend Note operator+(const Note &c, int delta);
    friend Note operator+(int delta, const Note &p);
    friend Note operator-(const Note &c, int delta);
    friend Note operator^(const Note &c, int delta);
    friend Note& operator<<(Note &c, Pitch p);

    // Modify each Pitch represented by a key in the map. Each value
    // represents the delta for changing the corresponding Pitch.
    void transform(const map<int, int> &deltas);

};

} // namespace smf
