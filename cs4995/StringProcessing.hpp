#ifndef STRING_PROCESSING_HPP_
#define STRING_PROCESSING_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "Note.hpp"

using std::vector;
using std::string;

namespace smf {

vector<string> tokenize(string str, char delimiter) {
    vector<string> tokens;
    std::stringstream stream(str);
    string intermediate;
    while(getline(stream, intermediate, delimiter)) {
        if (intermediate.length() > 0) {
            tokens.push_back(intermediate);
        }
    }
    return tokens;
}

// assumes valid input
vector<string> tokenize_chordstr(string str) {
    // Token containing '/' means we're dealing with a chord
    // split using / as a delimiter
    if (str.find('/') != string::npos) {
        return tokenize(str, '/');
    }

    vector<string> tokens;
    unsigned int start = 0, end = 1;
    // Create new token every time it encounters one of
    // {'A','B','C','D','E','F','G'}
    while (end < str.size()){
        while (!(isBasePitch(str[end])) && end < str.size() ) {
            end++;
        }
        tokens.push_back(str.substr(start, end - start));
        start = end;
        end++;
    }
    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }
    return tokens;
}

// Parse an input string and convert to the corresponding chords
vector<Chord> parseChords(string s) {
    vector<Chord> result;

    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return result;
    }

    // reserve space >= the amount needed
    result.reserve(tokens.size());

    float chordLength = 1.0;
    auto it = tokens.begin();
    while (it < tokens.end()) {
        string tok = *it;
        Chord c{chordLength};

        if (tok[tok.length() - 1] == '(') {
            // Start specifying note length
            int subdivision = std::stoi(tok.substr(0, tok.length() - 1));
            chordLength = WHOLE_NOTE_LENGTH / subdivision;
            ++it;
        } else if (tok.compare(")") == 0) {
            // Stop specifying note length
            chordLength = 1.0;
            ++it;
        } else {
            if (it->compare(REST) == 0) {
                // Do nothing; a chord without notes is a rest
            } else {
                // Construct chord using vector of pitches
                vector<string> pitch_tokens = tokenize_chordstr(tok);
                for (auto& pitch_token: pitch_tokens) {
                    c << Pitch{pitch_token};
                }
            }
            while(++it < tokens.end() && it->compare(EXTEND) == 0) {
                c.setLength(c.getLength() + chordLength);
            }
            result.push_back(c);
        }
    }

    return result;
}

} // namespace smf

#endif
