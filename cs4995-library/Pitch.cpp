#include <map>
#include <string>
#include "Pitch.hpp"

namespace smf {

using std::map;
using std::string;
using std::vector;

bool isBasePitch(char c) {
    return charPitchMap.find(c) != charPitchMap.end();
}

BasePitch toBasePitch(char c) {
    return charPitchMap.find(c)->second;
}

bool isAccidental(char c) {
    return charAccidentalMap.find(c) != charAccidentalMap.end();
}

int toAccidental(char c) {
    return charAccidentalMap.find(c)->second;
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

Pitch::Pitch(BasePitch base, int accidental, int octave) :
    base(base), accidental(accidental), octave(octave) {}

Pitch::Pitch(string s) {
    if (s.length() == 0) {
        std::cerr << "Invalid conversion from empty string to Pitch.\n";
        exit(1);
    }
    base = baseFromString(s);
    accidental = accidentalFromString(s);
    octave = octaveFromString(s);
}

Pitch::Pitch(int p) {
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

BasePitch Pitch::getBasePitch() {
    return base;
}

int Pitch::toInt() {
    return base + accidental + OCTAVE_WIDTH * octave;
}

Pitch& Pitch::operator+=(int delta) {
    Pitch result{toInt() + delta};
    base = result.base;
    accidental = result.accidental;
    octave = result.octave;
    return *this;
}

Pitch& Pitch::operator-=(int delta) {
    return *this += -1 * delta;
}

Pitch& Pitch::operator^=(int delta) {
    octave += delta;
    return *this;
}

void Pitch::transform(const map<int, int> &deltas) {
    auto val = deltas.find(base + accidental);
    if (val != deltas.end()) {
        *this += val->second;
    }
}

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

} // namespace smf
