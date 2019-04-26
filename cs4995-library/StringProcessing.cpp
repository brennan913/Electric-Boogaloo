#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "StringProcessing.hpp"

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

} // namespace smf
