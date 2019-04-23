#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include "Key.hpp"
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

int DEFAULT_VELOCITY = 64;
int DEFAULT_OCTAVE = 5;
float WHOLE_NOTE_LENGTH = 4.0; // unit is quarter notes

/* unimplemented
void transformLength(
    vector<Note> &notes, const map<float, float> &transformation);
    */

class Track {
private:
    vector<Chord> chords;
    int octave;
    int velocity;

    void transformPitch(const map<int, int> &deltas) {
        for (Chord &c : chords) {
            c.transformPitch(deltas);
        }
    }

    // returns true if changed
    bool changeChordDuration(float &chordLength, string token) {
        // Check for note subdivision and parentheses
        if (token[token.length() - 1] == '(') {
            int subdivision = std::stoi(token.substr(0, token.length() - 1));
            chordLength = WHOLE_NOTE_LENGTH / subdivision;
            return true;
        } else if (token.compare(")") == 0) {
            chordLength = 1.0;
            return true;
        }
        return false;
    }

public:
    Track(int octave = DEFAULT_OCTAVE, int velocity = DEFAULT_VELOCITY) :
        octave(octave), velocity(velocity) {}

    const vector<Chord>& getChords() {
        return chords;
    }

    const Chord& operator[](int index) const {
        return chords[index];
    }

    Chord& operator[](int index) {
        return chords[index];
    }

    int getVelocity() {
        return velocity;
    }

    int getOctave() {
        return octave;
    }

    void transpose(int delta) {
        for (Chord &c : chords) {
            c.transformPitch(delta);
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        transformPitch(src.getDifferences(dest));
    }

    friend void operator<<(Track &trk, Chord c);
    friend void operator<<(Track &trk, const vector<Chord> &c);
    friend void operator<<(Track &trk, string s);
};

void operator<<(Track &trk, Chord c) {
    trk.chords.push_back(c);
}

void operator<<(Track &trk, const vector<Chord> &cv) {
    trk.chords.reserve(trk.chords.size() + cv.size());
    for (Chord c : cv) {
        trk.chords.push_back(c);
    }
}

void operator<<(Track &trk, string s) {
    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return;
    }

    // reserve space >= the amount needed
    trk.chords.reserve(trk.chords.size() + tokens.size());

    float chordLength = 1.0;
    auto it = tokens.begin();
    while (it < tokens.end()) {
        Chord c; // default: quarter rest

        // Returns true if a parentheses is found (chordLength is thus also changed)
        if (trk.changeChordDuration(chordLength, *it)) {
          // Move the pointer and continue so that we don't write the empty chord
          //   down (which is still a quarter rest)
          ++it;
          continue;
        }

        if (it->find('/') != string::npos) {
            	// assume that containing '/' means we're dealing with a chord
            	// split using / as a delimiter
            	// construct pitch with each token, then construct chord
            	// using vector of pitches
      		vector<Pitch> pitches;
      		vector<string> pitch_tokens = tokenize(*it, '/');
      		for (auto& pitch_token: pitch_tokens){
      			pitches.push_back(Pitch{pitch_token});
      		}
      		c = Chord{pitches, chordLength};

      	// creates single note
      	} else if (it->compare(REST) != 0) {
          c = Chord{Pitch{*it}, chordLength};

        } else if (it->compare(REST) == 0) {
            // default chord is quarter rest
      	    c = Chord{chordLength};
      	}

        // does this assume valid input?
        while(++it < tokens.end() && it->compare(EXTEND) == 0) {
            c.incrementLength();
        }
        trk << c;
    }

    return;
}

} // namespace smf

#endif
