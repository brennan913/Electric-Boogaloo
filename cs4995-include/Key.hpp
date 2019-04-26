#pragma once

#include <map>
#include <string>
#include <vector>
#include "Pitch.hpp"
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

const vector<int> MAJOR = { 0, 2, 4, 5, 7, 9, 11 };
const vector<int> MINOR = { 0, 2, 3, 5, 7, 8, 11 };
const vector<int> HARMONIC_MINOR = MINOR;
const vector<int> NATURAL_MINOR = { 0, 2, 3, 5, 7, 8, 10 };

class Scale {
private:
    vector<int> scaleDegrees;

    void makeAscending();

public:
    Scale(const vector<Pitch> &pitches);
    Scale(const string &input);
    Scale(int key, const vector<int> &intervals);

    int size() const;
    const int& operator[](int index) const;
    int& operator[](int index);

    // Get the distance between each pair of notes in the two scales.
    map<int, int> getDifferences(const Scale &s) const;

    // Constructs a chord from the specified scale degree.
    Pitch getPitch(int degree) const;

    // Constructs a chord from the specified degrees in a scale.
    Chord getChord(vector<int> degrees, float length = QUARTER_NOTE) const;
};

} // namespace smf
