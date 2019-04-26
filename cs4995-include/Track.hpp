#pragma once

#include <string>
#include "Key.hpp"
#include "Note.hpp"
#include "Pitch.hpp"
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

    void transformPitch(const map<int, int> &deltas);

public:
    Track(int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY);

    Track(string str, int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY);

    const vector<Chord>& getChords() const;

    const Chord& operator[](int index) const;

    Chord& operator[](int index);

    int getVelocity() const;

    void setVelocity(int v);

    int getOctave() const;

    void setOctave(int o);

    void transpose(int delta);

    void modulate(const Scale &src, const Scale &dest);

    void resize(float factor);

    friend Track& operator<<(Track &trk, Chord c);
    friend Track& operator<<(Track &trk, const vector<Chord> &c);
    friend Track& operator<<(Track &trk, string s);
    Track &operator+=(const Track &t2);
    Track &operator*=(int factor);
    friend Track operator+(const Track &t1, const Track &t2);
    friend Track operator*(const Track &t, int factor);
    friend Track operator*(int factor, const Track &t);
};

} // namespace smf
