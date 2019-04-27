#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "Note.hpp"

using std::vector;
using std::string;

const string EXTEND = "-";
const string REST = ".";

namespace smf {

vector<string> tokenize(string str, char delimiter);

// Parse a token representing a chord, e.g. CEG or C/E/G.
vector<string> tokenizeChordstr(string str);

// Parse a string representing a series of notes.
vector<Note> parseNotes(string s);

} // namespace smf
