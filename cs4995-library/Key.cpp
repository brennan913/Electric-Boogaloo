#include <map>
#include <string>
#include <vector>
#include "Key.hpp"
#include <exception>

namespace smf {

using std::string;
using std::vector;
using std::map;


template <typename KeyType, typename ValueType>
map<KeyType, ValueType> generateMap(
    vector<KeyType> keys, vector<ValueType> values)
{
    if (keys.size() != values.size()) {
	throw std::invalid_argument("Different number of keys and values in generateMap()");
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
	throw std::invalid_argument(
	    "Provided scales in createMappingTo()"
	    "have different number of notes"
	    );
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
    if (deg < 0) {
        int remainder = deg % size();
        if (remainder < 0) {
            remainder += size();
        }
        int octave = (deg - remainder) / size();
        Pitch p{scaleDegrees[remainder]};
        p ^= octave;
        return p;
    }
    Pitch p{scaleDegrees[deg % size()]};
    p ^= deg / size();
    return p;
}

Note Scale::getChord(vector<int> degrees, float length) const {
    Note c{length};
    for (int deg : degrees) {
        c << getPitch(deg);
    }
    return c;
}

} // namespace smf
