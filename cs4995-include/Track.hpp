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

    // Apply the given mapping to each pitch in the Track.
    void transformPitch(const map<int, int> &deltas);

public:
    Track(int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY);
    Track(string str, int octave = DEFAULT_OCTAVE,
        int velocity = DEFAULT_VELOCITY);

    const vector<Chord>& getChords() const;
    int getVelocity() const;
    void setVelocity(int v);
    int getOctave() const;
    void setOctave(int o);

    // Apply the given delta to each pitch in the Track.
    void transpose(int delta);

    // Change the key of the Track from one Scale to another.
    void modulate(const Scale &src, const Scale &dest);

    // Stretch or compress the note lengths of the Track by the given factor.
    void resize(float factor);

    const Chord& operator[](int index) const;
    Chord& operator[](int index);

    // Operations for appending.
    friend Track& operator<<(Track &trk, Chord c);
    friend Track& operator<<(Track &trk, const vector<Chord> &c);
    friend Track& operator<<(Track &trk, string s);

    // Adding Tracks appends them to each other.
    Track &operator+=(const Track &t2);
    friend Track operator+(const Track &t1, const Track &t2);

    // Multiplying Tracks creates a loop.
    Track &operator*=(int factor);
    friend Track operator*(const Track &t, int factor);
    friend Track operator*(int factor, const Track &t);
};

} // namespace smf
