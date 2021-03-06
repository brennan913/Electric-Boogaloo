#pragma once

#include <map>
#include <iostream>
#include <string>
#include <sstream>

namespace smf {

using std::map;
using std::string;
using std::vector;

constexpr int OCTAVE_WIDTH = 12;

enum BasePitch {
    C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
};

const map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

const map<char, int> charAccidentalMap {
    {'#', 1} /* sharp */, {'b', -1} /* flat */
};


/*
 * Helper functions for processing character and string representations for
 * Pitch-related concepts.
 */

bool isBasePitch(char c);
bool isAccidental(char c);

BasePitch toBasePitch(char c);
int toAccidental(char c);
int toOctave(char c);

BasePitch baseFromString(string token);
int accidentalFromString(string token);
int octaveFromString(string token);

/*
 * A Pitch is composed of a base note, an optional accidental, and an octave.
 * For example, middle C is { C, 0, 5 }.
 */
class Pitch {
private:
    BasePitch base;
    int accidental; // e.g. sharp or flat
    int octave;

public:
    Pitch(BasePitch base = C, int accidental = 0, int octave = 0);

    // Construct a Pitch from a string representation, e.g. "C#5".
    Pitch(string s);

    // Construct a Pitch from an integer representation corresponding to the
    // MIDI pitch format.
    Pitch(int midiValue);

    BasePitch getBasePitch() const;
    int toInt() const;

    // If the Pitch is represented by a key in the delta map, apply the
    // corresponding delta.
    void transform(const map<int, int> &deltas);

    Pitch& operator+=(int delta);
    friend Pitch operator+(const Pitch &p, int delta);
    friend Pitch operator+(int delta, const Pitch &p);
    Pitch& operator-=(int delta);
    friend Pitch operator-(const Pitch &p, int delta);

    // Raise or lower the octave of the Pitch.
    Pitch& operator^=(int delta);
    friend Pitch operator^(const Pitch &p, int delta);

    friend std::ostream& operator<<(std::ostream &os, const Pitch &p);
};

} // namespace smf
