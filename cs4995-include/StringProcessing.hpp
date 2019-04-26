#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using std::vector;
using std::string;

namespace smf {

vector<string> tokenize(string str, char delimiter);

} // namespace smf
