#include <map>
#include <string>
#include <vector>
#include "Key.hpp"

namespace smf {

using std::string;
using std::vector;
using std::map;

template <typename KeyType, typename ValueType>
map<KeyType, ValueType> generateMap(
    vector<KeyType> keys, vector<ValueType> values)
{
    if (keys.size() != values.size()) {
        std::cerr << "Different number of keys and values in generateMap().\n";
        exit(1);
    }
    map<KeyType, ValueType> m;
    for (int i = 0; i < keys.size(); i++) {
        if (values[i] != 0) {
            m[keys[i]] = values[i];
        }
    }
    return m;
}

void Scale::makeAscending() {
    auto prev = scaleDegrees.begin();
    if (prev == scaleDegrees.end()) {
        return;
    }

    auto it = prev + 1;
    while(it != scaleDegrees.end()) {
        while (*it < *prev) {
            *it += OCTAVE_WIDTH;
        }
        it++;
    }
}

Scale::Scale(const vector<Pitch> &pitches) {
    for (Pitch p : pitches) {
        scaleDegrees.push_back(p.toInt() % OCTAVE_WIDTH);
    }
    makeAscending();
}

Scale::Scale(const string &input) {
    vector<string> tokens = tokenize(input, ' ');
    for (string tok : tokens) {
        Pitch p{tok};
        scaleDegrees.push_back(p.toInt());
    }
    makeAscending();
}

Scale::Scale(int key, const vector<int> &intervals) {
    for (int i : intervals) {
        scaleDegrees.push_back(key + i);
    }
    makeAscending();
}

map<int, int> Scale::createMappingTo(const Scale &s) const {
    if (scaleDegrees.size() != s.scaleDegrees.size()) {
        std::cerr <<
            "Different number of notes in scales in createMappingTo.\n";
        exit(1);
    }
    vector<int> diffs;
    for (int i = 0; i < scaleDegrees.size(); i++) {
        diffs.push_back((int) s.scaleDegrees[i] - (int) scaleDegrees[i]);
    }
    // Need to round down scale degrees to their common denominator to be
    // used for transformations.
    vector<int> baseDegrees;
    for (int deg : scaleDegrees) {
        baseDegrees.push_back(deg % OCTAVE_WIDTH);
    }
    return generateMap(baseDegrees, diffs);
}

int Scale::size() const { return scaleDegrees.size(); };

const int& Scale::operator[](int index) const { return scaleDegrees[index]; }

int& Scale::operator[](int index) { return scaleDegrees[index]; }

Pitch Scale::getPitch(int deg) const {
    Pitch p{scaleDegrees[deg % scaleDegrees.size()]};
    p ^= deg / scaleDegrees.size();
    return p;
}

Chord Scale::getChord(vector<int> degrees, float length) const {
    Chord c{length};
    for (int deg : degrees) {
        c << getPitch(deg);
    }
    return c;
}

} // namespace smf
