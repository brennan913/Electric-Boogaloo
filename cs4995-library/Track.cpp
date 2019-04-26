#include <string>
#include "Track.hpp"

namespace smf {

using std::string;
using std::vector;

void Track::transformPitch(const map<int, int> &deltas) {
    for (Chord &c : chords) {
        c.transform(deltas);
    }
}

Track::Track(int octave, int velocity) : octave(octave), velocity(velocity) {}

Track::Track(string str, int octave, int velocity) :
    octave(octave), velocity(velocity)
{
    *this << str;
}

const vector<Chord>& Track::getChords() const { return chords; }

int Track::getVelocity() const { return velocity; }

void Track::setVelocity(int v) { velocity = v; }

int Track::getOctave() const { return octave; }

void Track::setOctave(int o) { octave = o; }

void Track::transpose(int delta) {
    for (Chord &c : chords) {
        c += delta;
    }
}

void Track::modulate(const Scale &src, const Scale &dest) {
    transformPitch(src.getDifferences(dest));
}

void Track::resize(float factor) {
    for (Chord &c : chords) {
        c.setLength(c.getLength() * factor);
    }
}

const Chord& Track::operator[](int index) const { return chords[index]; }

Chord& Track::operator[](int index) { return chords[index]; }

Track& operator<<(Track &trk, Chord c) {
    trk.chords.push_back(c);
    return trk;
}

Track& operator<<(Track &trk, const vector<Chord> &cv) {
    trk.chords.reserve(trk.chords.size() + cv.size());
    trk.chords.insert(trk.chords.end(), cv.begin(), cv.end());
    return trk;
}

Track& operator<<(Track &trk, string s) {
    vector<Chord> chords = parseChords(s);
    trk << chords;
    return trk;
}

Track& Track::operator+=(const Track &t2) {
    int octaveDiff = t2.octave - this->octave;
    Track temp{t2};
    temp.transpose(octaveDiff * OCTAVE_WIDTH);
    for (Chord c : temp.chords) {
        this->chords.push_back(c);
    }
    return *this;
}

Track& Track::operator*=(int factor) {
    if (factor < 0) {
        std::cerr << "Invalid factor < 0 for operator*=.\n";
        exit(1);
    }
    if (factor == 0) {
        chords = vector<Chord>{};
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
