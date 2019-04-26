#include <map>
#include "Note.hpp"

namespace smf {

using std::map;
using std::vector;

// constructs a chord out of a collection of pitches
Chord::Chord(vector<Pitch> pitches, float length) :
    pitches(pitches), length(length) {}

// individual notes are not a special case, just chords
// with only one pitch
Chord::Chord(Pitch pitch, float length) : pitches(), length(length)
{
    pitches.push_back(pitch);
}

// constructs a chord from the specified notes in a scale
Chord::Chord(Scale s, vector<int> degrees, float length) : length(length) {
    for (int deg : degrees) {
        pitches.push_back(Pitch{s[deg % s.size()]});
    }
}

Chord::Chord(float length) : length(length) {}

vector<Pitch> Chord::getPitches() { return pitches; }

float Chord::getLength() { return length; }

void Chord::setLength(float l) { length = l; }

bool Chord::isRest() { return pitches.size() == 0; }

bool Chord::isNote() { return pitches.size() == 1; }

const Pitch& Chord::operator[](int index) const { return pitches[index]; }

Pitch& Chord::operator[](int index) { return pitches[index]; }

Chord& Chord::operator+=(int delta) {
    for (Pitch &p : pitches) {
        p += delta;
    }
    return *this;
}

Chord& Chord::operator-=(int delta) {
    for (Pitch &p : pitches) {
        p -= delta;
    }
    return *this;
}

Chord& Chord::operator^=(int delta) {
    for (Pitch &p : pitches) {
        p ^= delta;
    }
    return *this;
}

void Chord::transform(const map<int, int> &deltas) {
    for (Pitch &p : pitches) {
        p.transform(deltas);
    }
}

Chord operator+(const Chord &c, int delta) {
    Chord temp{c};
    temp += delta;
    return temp;
}

Chord operator+(int delta, const Chord &c) {
    return c + delta;
}

Chord operator-(const Chord &c, int delta) {
    return c + (-1 * delta);
}

Chord operator^(const Chord &c, int delta) {
    Chord temp{c};
    temp ^= delta;
    return temp;
}

Chord& operator<<(Chord &c, Pitch p) {
    c.pitches.push_back(p);
    return c;
}

// assumes valid input
vector<string> tokenizeChordstr(string str) {
    // Token containing '/' means we're dealing with a chord
    // split using / as a delimiter
    if (str.find('/') != string::npos) {
        return tokenize(str, '/');
    }

    vector<string> tokens;
    unsigned int start = 0, end = 1;
    // Create new token every time it encounters one of
    // {'A','B','C','D','E','F','G'}
    while (end < str.size()){
        while (!(isBasePitch(str[end])) && end < str.size() ) {
            end++;
        }
        tokens.push_back(str.substr(start, end - start));
        start = end;
        end++;
    }
    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }
    return tokens;
}

// Parse an input string and convert to the corresponding chords
vector<Chord> parseChords(string s) {
    vector<Chord> result;

    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return result;
    }

    // reserve space >= the amount needed
    result.reserve(tokens.size());

    float chordLength = 1.0;
    auto it = tokens.begin();
    while (it < tokens.end()) {
        string tok = *it;
        Chord c{chordLength};

        if (tok[tok.length() - 1] == '(') {
            // Start specifying note length
            int subdivision = std::stoi(tok.substr(0, tok.length() - 1));
            chordLength = WHOLE_NOTE_LENGTH / subdivision;
            ++it;
        } else if (tok.compare(")") == 0) {
            // Stop specifying note length
            chordLength = 1.0;
            ++it;
        } else {
            if (it->compare(REST) == 0) {
                // Do nothing; a chord without notes is a rest
            } else {
                // Construct chord using vector of pitches
                vector<string> pitchTokens = tokenizeChordstr(tok);
                for (auto& pitchToken: pitchTokens) {
                    c << Pitch{pitchToken};
                }
            }
            while(++it < tokens.end() && it->compare(EXTEND) == 0) {
                c.setLength(c.getLength() + chordLength);
            }
            result.push_back(c);
        }
    }

    return result;
}


} // namespace smf
