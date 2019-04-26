#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "Note.hpp"

using std::vector;
using std::string;

namespace smf {

vector<string> tokenize(string str, char delimiter);

vector<string> tokenizeChordstr(string str);

vector<Chord> parseChords(string s);

} // namespace smf
