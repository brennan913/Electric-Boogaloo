#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include "Key.hpp"
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

constexpr int DEFAULT_VELOCITY = 64;
constexpr int DEFAULT_OCTAVE = 5;

class Track {
private:
    vector<Chord> chords;
    int octave;
    int velocity;

    void transformPitch(const map<int, int> &deltas) {
        for (Chord &c : chords) {
            c.transform(deltas);
        }
    }

public:
    Track(int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY) :
        octave(octave), velocity(velocity) {}

    Track(string str, int octave = DEFAULT_OCTAVE,
        int velocity = DEFAULT_VELOCITY) : octave(octave), velocity(velocity)
    {
        *this << str;
    }

    const vector<Chord>& getChords() { return chords; }

    const Chord& operator[](int index) const { return chords[index]; }

    Chord& operator[](int index) { return chords[index]; }

    int getVelocity() const { return velocity; }

    void setVelocity(int v) { velocity = v; }

    int getOctave() const { return octave; }

    void setOctave(int o) { octave = o; }

    void transpose(int delta) {
        for (Chord &c : chords) {
            c += delta;
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        transformPitch(src.getDifferences(dest));
    }

    void resize(float factor) {
        for (Chord &c : chords) {
            c.setLength(c.getLength() * factor);
        }
    }

    friend Track& operator<<(Track &trk, Chord c);
    friend Track& operator<<(Track &trk, const vector<Chord> &c);
    friend Track& operator<<(Track &trk, string s);
    Track &operator+=(const Track &t2);
    Track &operator*=(int factor);
    friend Track operator+(const Track &t1, const Track &t2);
    friend Track operator*(const Track &t, int factor);
};

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

#endif
