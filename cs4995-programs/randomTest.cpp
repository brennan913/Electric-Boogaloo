#include <algorithm>
#include <utility>
#include <set>
#include <random>
#include "MidiOutput.hpp"
#include <cassert>
#include <chrono>

using std::pair;
using std::set;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::geometric_distribution;

using namespace smf;

int count = 0;
bool debug = false;
std::random_device r;
std::default_random_engine gen{r()};
uniform_int_distribution<int> coin(0, 1);
uniform_real_distribution<float> percent(0, 1);

/*
 * Functions for generating scale degrees and notes
 */
vector<int> generateUniform(int numElements, int min, int max);

vector<int> generateMelodic(Scale key, int numElements, int min, int max,
    float smoothness, float directionalBias);

vector< vector<int> > generateTriads(
    const vector<int> &melody,
    int notesPerHarmony,
    int scaleSize,
    float dissonance);

vector<int> generateTriad(
    const vector<int> &degreeGroup, int scaleSize, float dissonance);

vector<int> generateHarmony(const vector<int> &degreeGroup, int scaleSize,
    float dissonance);

vector<Note> degreesToNotes(
    Scale key, const vector<int> &degrees, float length);

vector<Note> generateChords(
    Scale key,
    const vector<int> &melody,
    int notesPerHarmony,
    float length,
    float dissonance);

/*
 * Helpers
 */
int flipCoin();

float randomPercent();

int leastMultiple(int factor, int min);

vector<int> getMultiples(int factor, int min, int max);

int nonnegativeRemainder(int num, int modulus);

vector<int> getEquivalenceClass(int baseNum, int modulus, int min, int max);

vector<int> getNextInversion(const vector<int> &current,
    const vector<int> &next, int scaleSize, int maxDiff, int min, int max);

void testInversion(vector<int> current, vector<int> next);

bool isDissonant(int degree1, int degree2, int scaleSize);

int dissonanceScore(int degree1, const vector<int> &degreeVec, int scaleSize);

int dissonanceScore(const vector<int> &degreeVec1,
    const vector<int> &degreeVec2, int scaleSize);

int main() {

    using namespace std::chrono;
    auto start = system_clock::now();

    Scale bMajor{B, MINOR};
    int cycles = 100;

    Track glitter{7, 40};
    Track melody;
    Track bass{2};

    vector<int> uniformMelody = generateUniform(
        cycles * 64, 0, bMajor.size() - 1);
    glitter << degreesToNotes(bMajor, uniformMelody, EIGHTH_LENGTH);

    vector<int> geometricMelody = generateMelodic(
        bMajor,
        cycles * 32,
        -1 * bMajor.size(),
        1.5 * bMajor.size(),
        0.5,
        0.7);
    melody << degreesToNotes(bMajor, geometricMelody, QUARTER_LENGTH);

    bass << generateChords(bMajor, geometricMelody, 8, WHOLE_LENGTH * 2, 0.3);

    MidiOutput out{ {glitter, melody, bass}, 120};

    auto duration = system_clock::now() - start;
    std::cout << "\tGeneration time elapsed: "
              << duration_cast<milliseconds>(duration).count() << " ms\n";
    start = system_clock::now();

    out.write("random.mid");

    duration = system_clock::now() - start;
    std::cout << "\tWrite time elapsed: "
              << duration_cast<milliseconds>(duration).count() << " ms\n";
}

vector<Note> degreesToNotes(Scale key, const vector<int> &degrees, float length)
{
    vector<Note> notes;
    notes.reserve(degrees.size());
    for (int deg : degrees) {
        notes.push_back(Note{ key.getPitch(deg), length });
    }
    return notes;
}

/*
 * Generate numbers from a uniform random distribution, with no identical
 * consecutive elements
 */
vector<int> generateUniform(int numElements, int min, int max)
{
    vector<int> degrees;
    degrees.reserve(numElements);
    uniform_int_distribution<int> uniform(min, max);
    int val = uniform(gen);
    for (int i = 0; i < numElements; i++) {
        degrees.push_back(val);
        // guarantee that each note is different from the previous
        int next = val;
        while (next == val) {
            next = uniform(gen);
        }
        val = next;
    }
    return degrees;
}

// 0 < smoothness < 1 defines the geometric distribution for interval size. The higher the
// smoothness, the more likely that consecutive notes will be closer together.
//
// 0 < directionalBias < 1 defines the bias for interval direction. 0 means no
// bias, 1 means once a direction is chosen, the following notes will all go in
// that direction.
vector<int> generateMelodic(
    Scale key, int numElements,
    int min, int max,
    float smoothness, float directionalBias)
{
    vector<int> degrees;
    degrees.reserve(numElements);
    // Used to determine the distance between a note and the next.
    geometric_distribution<int> geometric(smoothness);

    int prevInterval = 0;
    // Used to determine whether the next note is higher or lower.
    int prevCoin = flipCoin();
    int val = 0;
    for (int i = 0; i < numElements; i++) {
        int interval = geometric(gen);
        // No repeated consecutive degrees and no leaps of an octave or more.
        while (interval < 1 || interval >= key.size()) {
            interval = geometric(gen);
        }
        if (randomPercent() > directionalBias) {
            // no bias
            int toss = flipCoin();
            if (toss == 0) {
                interval *= -1;
            }
            prevCoin = toss;
        } else {
            // if previous interval was a leap, bias stepwise motion in the
            // other direction.
            // otherwise if it was a step, do nothing (bias in same direction)
            if (abs(prevInterval) >= 2) {
                interval = 1;
                prevCoin = (prevCoin + 1) % 2;
            }
            if (prevCoin == 0) {
                interval *= -1;
            }
        }
        // Reverse the direction if out of bounds
        if (val + interval < min || val + interval > max) {
            interval *= -1;
        }
        val += interval;
        prevInterval = interval;
        degrees.push_back(val);
    }
    return degrees;
}

vector<Note> generateChords(
    Scale key,
    const vector<int> &melody,
    int notesPerHarmony,
    float length,
    float dissonance)
{
    vector< vector<int> > harmonies = generateTriads(
        melody, notesPerHarmony, key.size(), dissonance);
    vector<Note> chords;
    chords.reserve(harmonies.size());

    // generate chords from the harmonies
    chords.push_back(key.getChord(harmonies[0], length));
    auto it = harmonies.begin();
    while (it + 1 < harmonies.end()) {
        chords.push_back(key.getChord(
            getNextInversion(*it, *(it + 1), key.size(), 3, -5, 8),
            WHOLE_LENGTH * 2));
        it++;
    }
    return chords;
}

vector< vector<int> > generateTriads(
    const vector<int> &melody,
    int notesPerHarmony,
    int scaleSize,
    float dissonance)
{
    // build harmonies for the bass
    vector< vector<int> > degrees;
    degrees.reserve(melody.size() / notesPerHarmony);
    auto melody_it = melody.begin();
    while (melody_it < melody.end()) {
        vector<int> degreeGroup;
        for (int i = 0; i < notesPerHarmony; i++) {
            degreeGroup.push_back(*melody_it++);
        }
        degrees.push_back(generateTriad(degreeGroup, scaleSize, dissonance));
    }
    return degrees;
}

vector<int> generateTriad(
    const vector<int> &degreeGroup, int scaleSize, float dissonance)
{
    geometric_distribution<int> geometric(dissonance);
    int bestRoot = 0;
    int lowestDissonanceScore = dissonanceScore(
        { 0, 2, 4 }, degreeGroup, scaleSize);
    lowestDissonanceScore *= 10 + geometric(gen);
    for (int i = 1; i < scaleSize; i++) {
        int newScore = dissonanceScore(
            { i, i+2, i+4 }, degreeGroup, scaleSize);
        newScore *= 10 + geometric(gen);
        if (newScore < lowestDissonanceScore) {
            lowestDissonanceScore = newScore;
            bestRoot = i;
        }
    }

    if (debug) {
        std::cout << "lowest dissonance for {";
        for (int n : degreeGroup) {
            std::cout << n << ", ";
        }
        std::cout << "} was root="
            << bestRoot << " with value " << lowestDissonanceScore << "\n";
    }

    return { bestRoot, bestRoot+2, bestRoot+4 };
}

int flipCoin() {
    return coin(gen);
}

float randomPercent() {
    return percent(gen);
}

// get the least multiple > min
int leastMultiple(int factor, int min) {
    return min + ((factor - min % factor) % factor);
}

// get all n in [min, max] | n % factor == 0. EXCEPT if none exist, break
// the max.
vector<int> getMultiples(int factor, int min, int max) {
    int multiple = leastMultiple(factor, min);
    vector<int> multiples;
    multiples.reserve((max - min) / factor);
    while (multiple <= max) {
        multiples.push_back(multiple);
        multiple += factor;
    }
    // We need at least one multiple to exist. Break the max if necessary.
    if (multiples.empty()) {
        multiples.push_back(multiple);
    }

    if (debug) {
        std::cout << "getMultiples(" <<
            min << ", " << max << ", " << factor << "): ";
        for (int v : multiples) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    }

    return multiples;
}

int nonnegativeRemainder(int num, int modulus) {
    return ((num % modulus) + modulus) % modulus;
}

// get all n in [min, max] | n % modulus == baseNum
vector<int> getEquivalenceClass(int baseNum, int modulus, int min, int max)
{
    if (debug) {
        std::cout << "getEquivalenceClass(" <<
            baseNum << ", " << modulus << ", " << min << ", " << max << "): ";
    }

    int remainder = nonnegativeRemainder(baseNum, modulus);
    vector<int> values = getMultiples(modulus, min, max - remainder);
    for (int &v : values) {
        v += remainder;
    }

    if (debug) {
        for (int v : values) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    }
    return values;
}

vector<int> getNextInversion(
    const vector<int> &current,
    const vector<int> &next,
    int scaleSize,
    int maxDiff, /* max distance between corresponding notes in the two chords */
    int min,
    int max)
{
    // in the future, maybe allow for chords of different sizes
    assert(current.size() == next.size());

    vector<int> nextInversion;
    nextInversion.reserve(next.size());
    set<int> unusedDegrees{ next.begin(), next.end() };

    for (int currentDegree : current) {
        int currentMin = std::max(min, currentDegree - maxDiff);
        int currentMax = std::min(max, currentDegree + maxDiff);

        if (debug) {
            std::cout << "get degree following " << currentDegree << "...\n";
        }

        vector< pair<int, int> > candidates;
        for (int degree : unusedDegrees) {
            vector<int> candidateValues = getEquivalenceClass(
                degree, scaleSize, currentMin, currentMax);
            for (int value : candidateValues) {
                candidates.push_back(pair<int, int>{ value, degree });
            }
        }
        uniform_int_distribution<int> uniform(0, candidates.size() - 1);
        pair<int, int> nextDegree = candidates[uniform(gen)];
        nextInversion.push_back(nextDegree.first);
        unusedDegrees.erase(nextDegree.second);

        if (debug) {
            std::cout << "...got " << nextDegree.first <<
                " from " << nextDegree.second << ".\n";
        }
    }

    // make sure notes are in ascending order
    sort(nextInversion.begin(), nextInversion.end());
    return nextInversion;
}

void testInversion(vector<int> current, vector<int> next) {
    vector<int> nextInversion = getNextInversion(current, next, 7, 4, -26, 29);
    std::cout << "Got inversion: ";
    for (int i : nextInversion) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

bool isDissonant(int degree1, int degree2, int scaleSize) {
    int diff =  abs(degree1 - degree2) % scaleSize;
    return (diff == 1 || diff == 6);
}

int dissonanceScore(int degree1, const vector<int> &degreeVec, int scaleSize) {
    int score = 0;
    for (int degree2 : degreeVec) {
        if (isDissonant(degree1, degree2, scaleSize)) score++;
    }
    return score;
}

int dissonanceScore(
    const vector<int> &degreeVec1, const vector<int> &degreeVec2, int scaleSize)
{
    int score = 0;
    for (int degree : degreeVec1) {
        score += dissonanceScore(degree, degreeVec2, scaleSize);
    }
    return score;
}

