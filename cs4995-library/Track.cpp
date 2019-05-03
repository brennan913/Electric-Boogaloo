#include <string>
#include "Track.hpp"
#include <exception>

namespace smf {

using std::string;
using std::vector;

void Track::transformPitch(const map<int, int> &deltas) {
    for (Note &n : notes) {
        n.transform(deltas);
    }
}

Track::Track(int octave, int velocity) : octave(octave), velocity(velocity) {}

Track::Track(string str, int octave, int velocity) :
    octave(octave), velocity(velocity)
{
    *this << str;
}

const vector<Note>& Track::getNotes() const { return notes; }

int Track::getVelocity() const { return velocity; }

void Track::setVelocity(int v) { velocity = v; }

int Track::getOctave() const { return octave; }

void Track::setOctave(int o) { octave = o; }

void Track::transpose(int delta) {
    for (Note &n : notes) {
        n += delta;
    }
}

void Track::modulate(const Scale &src, const Scale &dest) {
    transformPitch(src.createMappingTo(dest));
}

void Track::resize(float factor) {
    for (Note &n : notes) {
        n.setLength(n.getLength() * factor);
    }
}

const Note& Track::operator[](int index) const { return notes[index]; }

Note& Track::operator[](int index) { return notes[index]; }

Track& operator<<(Track &trk, Note c) {
    trk.notes.push_back(c);
    return trk;
}

Track& operator<<(Track &trk, const vector<Note> &v) {
    trk.notes.reserve(trk.notes.size() + v.size());
    trk.notes.insert(trk.notes.end(), v.begin(), v.end());
    return trk;
}

Track& operator<<(Track &trk, string s) {
    vector<Note> notes = parseNotes(s);
    trk << notes;
    return trk;
}

Track& Track::operator+=(const Track &t2) {
    int octaveDiff = t2.octave - this->octave;
    Track temp{t2};
    temp.transpose(octaveDiff * OCTAVE_WIDTH);
    for (Note c : temp.notes) {
        this->notes.push_back(c);
    }
    return *this;
}

Track& Track::operator*=(int factor) {
    if (factor < 0) {
        throw std::invalid_argument("Invalid factor < 0 for operator*=()");
    }
    if (factor == 0) {
        notes = vector<Note>{};
    } else {
        Track temp = *this;
        for (int i = 0; i < factor - 1; i++) {
            *this += temp;
        }
    }
    return *this;
}

Track operator+(const Track &t1, const Track &t2) {
    Track sum{t1};
    sum += t2;
    return sum;
}

Track operator*(const Track &t, int factor) {
    Track multiple{t};
    multiple *= factor;
    return multiple;
}

Track operator*(int factor, const Track &t) {
    return t * factor;
}

} // namespace smf
