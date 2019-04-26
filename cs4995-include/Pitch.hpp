#pragma once

#include <map>
#include <iostream>
#include <string>
#include <sstream>

namespace smf {

using std::map;
using std::string;
using std::vector;

constexpr float WHOLE_NOTE_LENGTH = 4.0; // unit is quarter notes
constexpr int OCTAVE_WIDTH = 12;
const string EXTEND = "-";
const string REST = ".";

enum BasePitch {
    C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
};

const map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

const map<char, int> charAccidentalMap {
    {'#', 1} /* sharp */, {'b', -1} /* flat */
};


bool isBasePitch(char c);

BasePitch toBasePitch(char c);

bool isAccidental(char c);

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

    Pitch(string s);

    // MIDI pitch representation to Pitch object
    Pitch(int p);

    BasePitch getBasePitch();

    int toInt();

    Pitch& operator+=(int delta);

    Pitch& operator-=(int delta);

    Pitch& operator^=(int delta);

    void transform(const map<int, int> &deltas);

    friend Pitch operator+(const Pitch &p, int delta);
    friend Pitch operator+(int delta, const Pitch &p);
    friend Pitch operator-(const Pitch &p, int delta);
    friend Pitch operator^(const Pitch &p, int delta);
    friend std::ostream& operator<<(std::ostream &os, const Pitch &p);
};

} // namespace smf
