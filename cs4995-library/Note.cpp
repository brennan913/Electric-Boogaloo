#include <map>
#include <vector>
#include "Note.hpp"

namespace smf {

using std::map;
using std::vector;

Chord::Chord(const vector<Pitch> &pitches, float length) :
    pitches(pitches), length(length) {}

Chord::Chord(Pitch pitch, float length) : pitches(), length(length)
{
    pitches.push_back(pitch);
}

Chord::Chord(float length) : length(length) {}

vector<Pitch> Chord::getPitches() { return pitches; }

float Chord::getLength() const { return length; }

void Chord::setLength(float l) { length = l; }

bool Chord::isRest() const { return pitches.size() == 0; }

bool Chord::isNote() const { return pitches.size() == 1; }

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

void Chord::transform(const map<int, int> &deltas) {
    for (Pitch &p : pitches) {
        p.transform(deltas);
    }
}

} // namespace smf
