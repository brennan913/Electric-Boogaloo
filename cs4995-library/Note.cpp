#include <map>
#include <vector>
#include "Note.hpp"

namespace smf {

using std::map;
using std::vector;

Note::Note(const vector<Pitch> &pitches, float length) :
    pitches(pitches), length(length) {}

Note::Note(Pitch pitch, float length) : pitches(), length(length)
{
    pitches.push_back(pitch);
}

Note::Note(float length) : length(length) {}

vector<Pitch> Note::getPitches() { return pitches; }

float Note::getLength() const { return length; }

void Note::setLength(float l) { length = l; }

bool Note::isRest() const { return pitches.size() == 0; }

bool Note::isSingleNote() const { return pitches.size() == 1; }

const Pitch& Note::operator[](int index) const { return pitches[index]; }

Pitch& Note::operator[](int index) { return pitches[index]; }

Note& Note::operator+=(int delta) {
    for (Pitch &p : pitches) {
        p += delta;
    }
    return *this;
}

Note& Note::operator-=(int delta) {
    for (Pitch &p : pitches) {
        p -= delta;
    }
    return *this;
}

Note& Note::operator^=(int delta) {
    for (Pitch &p : pitches) {
        p ^= delta;
    }
    return *this;
}

Note operator+(const Note &note, int delta) {
    Note temp{note};
    temp += delta;
    return temp;
}

Note operator+(int delta, const Note &note) {
    return note + delta;
}

Note operator-(const Note &note, int delta) {
    return note + (-1 * delta);
}

Note operator^(const Note &note, int delta) {
    Note temp{note};
    temp ^= delta;
    return temp;
}

Note& operator<<(Note &note, Pitch p) {
    note.pitches.push_back(p);
    return note;
}

Note& operator<<(Note &note, vector<Pitch> v) {
    note.pitches.reserve(note.pitches.size() + v.size());
    for (Pitch p : v) {
        note << p;
    }
    return note;
}

void Note::transform(const map<int, int> &deltas) {
    for (Pitch &p : pitches) {
        p.transform(deltas);
    }
}

} // namespace smf
