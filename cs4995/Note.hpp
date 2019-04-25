#ifndef NOTE_HPP_
#define NOTE_HPP_

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

map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

map<char, int> charAccidentalMap {
    {'#', 1} /* sharp */, {'b', -1} /* flat */
};

bool isBasePitch(char c) {
    return charPitchMap.find(c) != charPitchMap.end();
}

BasePitch toBasePitch(char c) {
    return charPitchMap[c];
}

bool isAccidental(char c) {
    return charAccidentalMap.find(c) != charAccidentalMap.end();
}

int toAccidental(char c) {
    return charAccidentalMap[c];
}

int toOctave(char c) {
    return (int) (c - '0');
}

/* no error-checking at the moment for string-processing functions */

BasePitch baseFromString(string token) {
    return toBasePitch(token[0]);
}

int accidentalFromString(string token) {
    int pos = token.find("#");
    if (pos != string::npos) {
        return 1;
    }
    pos = token.find("b");
    if (pos != string::npos) {
        return -1;
    }
    return 0;
}

int octaveFromString(string token) {
    int pos = token.find("^");
    if (pos != string::npos) {
        return toOctave(token[pos + 1]);
    }
    pos = token.find("_");
    if (pos != string::npos) {
        return -1 * (toOctave(token[pos + 1]));
    }
    return 0;
}

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
    Pitch(BasePitch base = C, int accidental = 0,
          int octave = 0) :
        base(base), accidental(accidental), octave(octave) {}

    Pitch(string s) {
        if (s.length() == 0) {
            std::cerr << "Invalid conversion from empty string to Pitch.\n";
            exit(1);
        }
        base = baseFromString(s);
        accidental = accidentalFromString(s);
        octave = octaveFromString(s);
    }

    // MIDI pitch representation to Pitch object
    Pitch(int p) {
        int remainder = p % OCTAVE_WIDTH;
        octave = p / OCTAVE_WIDTH;
        if(remainder < 0) {
            remainder += OCTAVE_WIDTH;
            octave--;
        }
        switch(remainder) {
            case 0:  base = C; accidental = 0;  break;
            case 1:  base = C; accidental = 1;  break;
            case 2:  base = D; accidental = 0;  break;
            case 3:  base = E; accidental = -1; break;
            case 4:  base = E; accidental = 0;  break;
            case 5:  base = F; accidental = 0;  break;
            case 6:  base = F; accidental = 1;  break;
            case 7:  base = G; accidental = 0;  break;
            case 8:  base = A; accidental = -1; break;
            case 9:  base = A; accidental = 0;  break;
            case 10: base = B; accidental = -1; break;
            case 11: base = B; accidental = 0;  break;
            default: break;
        }
    }

    BasePitch getBasePitch() {
        return base;
    }

    int toInt() {
        return base + accidental + OCTAVE_WIDTH * octave;
    }

    Pitch& operator+=(int delta) {
        Pitch result{toInt() + delta};
        base = result.base;
        accidental = result.accidental;
        octave = result.octave;
        return *this;
    }

    Pitch& operator-=(int delta) {
        return *this += -1 * delta;
    }

    Pitch& operator^=(int delta) {
        octave += delta;
        return *this;
    }

    void transform(const map<int, int> &deltas) {
        auto val = deltas.find(base + accidental);
        if (val != deltas.end()) {
            *this += val->second;
        }
    }

    friend Pitch operator+(const Pitch &p, int delta);
    friend Pitch operator+(int delta, const Pitch &p);
    friend Pitch operator-(const Pitch &p, int delta);
    friend Pitch operator^(const Pitch &p, int delta);
    friend std::ostream& operator<<(std::ostream &os, const Pitch &p);
};

Pitch operator+(const Pitch &p, int delta) {
    Pitch temp{p};
    temp += delta;
    return temp;
}

Pitch operator+(int delta, const Pitch &p) {
    return p + delta;
}

Pitch operator-(const Pitch &p, int delta) {
    return p + (-1 * delta);
}

Pitch operator^(const Pitch &p, int delta) {
    Pitch temp{p};
    temp ^= delta;
    return temp;
}

std::ostream& operator<<(std::ostream &os, const Pitch &p) {
    os << p.base <<
        (p.accidental == 1 ? "#" : p.accidental == -1 ? "b" : "") << p.octave;
    return os;
}

class Chord {
private:
    vector<Pitch> pitches;
    float length; // quarter note == 1.0

public:

    // constructs a chord out of a collection of pitches
    Chord(vector<Pitch> pitches, float length = 1.0) :
        pitches(pitches), length(length) {}

    // individual notes are not a special case, just chords
    // with only one pitch
    Chord(Pitch pitch, float length = 1.0) : pitches(), length(length)
    {
        pitches.push_back(pitch);
    }

    Chord(float length = 1.0) : length(length) {}

    vector<Pitch> getPitches() { return pitches; }

    float getLength() { return length; }

    void setLength(float l) { length = l; }

    bool isRest() { return pitches.size() == 0; }

    bool isNote() { return pitches.size() == 1; }

    const Pitch& operator[](int index) const { return pitches[index]; }

    Pitch& operator[](int index) { return pitches[index]; }

    Chord& operator+=(int delta) {
        for (Pitch &p : pitches) {
            p += delta;
        }
        return *this;
    }

    Chord& operator-=(int delta) {
        for (Pitch &p : pitches) {
            p -= delta;
        }
        return *this;
    }

    Chord& operator^=(int delta) {
        for (Pitch &p : pitches) {
            p ^= delta;
        }
        return *this;
    }

    void transform(const map<int, int> &deltas) {
        for (Pitch &p : pitches) {
            p.transform(deltas);
        }
    }

    friend Chord operator+(const Chord &c, int delta);
    friend Chord operator+(int delta, const Chord &p);
    friend Chord operator-(const Chord &c, int delta);
    friend Chord operator^(const Chord &c, int delta);
    friend Chord& operator<<(Chord &c, Pitch p);
};

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


} // namespace smf

#endif
