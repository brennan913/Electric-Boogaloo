#include <iostream>
#include <string>
#include <vector>
#include "MidiOutput.hpp"
#include <cassert>

using std::vector;
using namespace smf;

void test(string input, vector<string> expected) {
    vector<string> tokens = tokenizeNotestr(input);
    for (int i = 0; i < tokens.size(); i++) {
        assert(tokens[i].compare(expected[i]) == 0);
    }
}

int main() {
    std::cout << "Testing note tokenizer...\n";
    test("ACE", {"A", "C", "E"});
    test("A#CbF", {"A#", "Cb", "F"});
    test("A1234B1234C1234", {"A1234", "B1234", "C1234"});
    std::cout << "...Passed\n";
}
