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

vector<string> tokenizeNotestr(string str);

// Parse a string into a list of Pitch tokens,
// e.g. CEG or C/E/G --> { C, E, G }
vector<Pitch> parsePitches(string s);

// Parse a string representing a series of notes.
vector<Note> parseNotes(string s);

} // namespace smf
