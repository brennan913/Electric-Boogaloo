#include <algorithm>
#include <utility>
#include <set>
#include <random>
#include "MidiOutput.hpp"

using std::pair;
using std::set;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::geometric_distribution;

using namespace smf;

std::random_device r;
std::default_random_engine gen{r()};

vector<Note> getChords(Scale &s, vector< vector<int> > &degrees, float length) {
    vector<Note> chords(degrees.size());
    for (int i = 0; i < degrees.size(); i++) {
        chords[i] = s.getChord(degrees[i], length);
    }
    return chords;
}

// floor and ceiling indicate the minimum and maximum scale degree allowed in
// the melody.
vector<int> generateUniformMelody(int numNotes, int floor, int ceiling)
{
    vector<int> notes;
    notes.reserve(numNotes);
    uniform_int_distribution<int> uniform(floor, ceiling);
    int val = uniform(gen);
    for (int i = 0; i < numNotes; i++) {
        notes.push_back(val);
        // guarantee that each note is different from the previous
        int next = val;
        while (next == val) {
            next = uniform(gen);
        }
        val = next;
    }
    return notes;
}

// 0 < smoothness < 1 defines the geometric distribution for interval size. The higher the
// smoothness, the more likely that consecutive notes will be closer together.
//
// 0 < directionalBias < 1 defines the bias for interval direction. 0 means no
// bias, 1 means once a direction is chosen, the following notes will all go in
// that direction.
vector<int> generateGeometricMelody(
    Scale key,
    int numNotes,
    int floor,
    int ceiling,
    float smoothness,
    float directionalBias)
{
    vector<int> notes;
    notes.reserve(numNotes);
    // Used to determine the distance between a note and the next.
    geometric_distribution<int> geometric(smoothness);
    // Used to determine whether the next note is higher or lower.
    uniform_int_distribution<int> coin(0, 1);
    uniform_real_distribution<float> bias(0, 1);

    int prevInterval = 0;
    int prevCoin = coin(gen);
    int val = 0;
    for (int i = 0; i < numNotes; i++) {
        int interval = geometric(gen);
        // No repeated consecutive notes and no leaps of an octave or more.
        while (interval < 1 || interval >= key.size()) {
            interval = geometric(gen);
        }
        if (bias(gen) < directionalBias) {
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
        } else {
            int toss = coin(gen);
            if (toss == 0) {
                interval *= -1;
            }
            prevCoin = toss;
        }
        // Reverse the direction if necessary.
        if (val + interval < floor || val + interval > ceiling) {
            interval *= -1;
        }
        val += interval;
        prevInterval = interval;
        notes.push_back(val);
    }
    return notes;
}

// get all n in [floor, ceiling] | n % factor == 0. EXCEPT if none exist, break
// the ceiling.
vector<int> getMultiples(int floor, int ceiling, int factor) {
     // get the first multiple > floor
    int multiple = floor + ((factor - floor % factor) % factor);
    vector<int> multiples;
    multiples.reserve((ceiling - floor) / factor);
    while (multiple <= ceiling) {
        multiples.push_back(multiple);
        multiple += factor;
    }
    // We need at least one multiple to exist. Break the ceiling if necessary.
    if (multiples.empty()) {
        multiples.push_back(multiple);
    }
    std::cout << "getMultiples(" << floor << ", " << ceiling << ", " << factor << "): ";
    for (int v : multiples) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
    return multiples;
}

// get all n in [floor, ceiling] | n % modulus == baseNum
vector<int> getEquivalentValues(
    int baseNum, int modulus, int floor, int ceiling)
{
    std::cout << "getEquivalentValues(" << baseNum << ", " << modulus << ", " << floor << ", " << ceiling << "): ";
    int remainder = ((baseNum % modulus) + modulus) % modulus;
    vector<int> values = getMultiples(floor, ceiling - remainder, modulus);
    for (int &v : values) {
        v += remainder;
    }
    for (int v : values) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
    return values;
}

vector<int> getNextInversion(
    const vector<int> &current,
    const vector<int> &next,
    int scaleSize,
    int maxDiff, /* max distance between corresponding notes in the two chords */
    int floor,
    int ceiling)
{
    // in the future, maybe allow for chords of different sizes
    assert(current.size() == next.size());

    vector<int> nextInversion;
    nextInversion.reserve(next.size());
    set<int> unusedDegrees{ next.begin(), next.end() };

    for (int currentDegree : current) {
        int trueFloor = std::max(floor, currentDegree - maxDiff);
        int trueCeiling = std::min(ceiling, currentDegree + maxDiff);
        std::cout << "getting degree to follow " << currentDegree << "...\n";
        vector< pair<int, int> > candidates;
        for (int degree : unusedDegrees) {
            vector<int> candidateValues = getEquivalentValues(
                degree, scaleSize, trueFloor, trueCeiling);
            for (int value : candidateValues) {
                candidates.push_back(pair<int, int>{ value, degree });
            }
        }
        uniform_int_distribution<int> uniform(0, candidates.size() - 1);
        pair<int, int> nextDegree = candidates[uniform(gen)];
        nextInversion.push_back(nextDegree.first);
        std::cout << "...got " << nextDegree.first << " from " << nextDegree.second << ".\n";
        unusedDegrees.erase(nextDegree.second);
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

vector<int> generateHarmony(vector<int> notes, int scaleSize, float dissonance) {
    geometric_distribution<int> geometric(dissonance);
    int bestRoot = 0;
    int lowestDissonanceScore = dissonanceScore({ 0, 2, 4 }, notes, scaleSize);
    lowestDissonanceScore *= 10 + geometric(gen);
    for (int i = 1; i < scaleSize; i++) {
        int newScore = dissonanceScore({ i, i+2, i+4 }, notes, scaleSize);
        newScore *= 10 + geometric(gen);
        if (newScore < lowestDissonanceScore) {
            lowestDissonanceScore = newScore;
            bestRoot = i;
        }
    }
    std::cout << "lowest dissonance for {";
    for (int n : notes) {
        std::cout << n << ", ";
    }
    std::cout << "} was root="
        << bestRoot << " with value " << lowestDissonanceScore << "\n";
    return { bestRoot, bestRoot+2, bestRoot+4 };
}

int main() {

    Scale bMajor{B, MINOR};
    int cycles = 10;

    Track glitter{7, 40};
    vector<int> uniformMelody = generateUniformMelody(
        cycles * 64, 0, bMajor.size() - 1);
    for (int n : uniformMelody) {
        glitter << Note{ bMajor.getPitch(n), EIGHTH_LENGTH };
    }

    Track melody;
    vector<int> geometricMelody = generateGeometricMelody(
        bMajor,
        cycles * 32,
        -1 * bMajor.size(),
        1.5 * bMajor.size(),
        0.5,
        0.7);
    for (int n : geometricMelody) {
        melody << Note{ bMajor.getPitch(n), QUARTER_LENGTH };
    }

    // build harmonies for the bass
    vector< vector<int> > harmonies;
    auto melody_it = geometricMelody.begin();
    while (melody_it < geometricMelody.end()) {
        vector<int> melodyGroup;
        for (int i = 0; i < 8; i++) {
            melodyGroup.push_back(*melody_it++);
        }
        harmonies.push_back(generateHarmony(melodyGroup, bMajor.size(), 0.3));
    }

    // generate chords from the harmonies
    vector<int> prev = harmonies[0];
    vector<Note> chords;
    chords.push_back(bMajor.getChord(harmonies[0], WHOLE_LENGTH * 2));
    auto it = harmonies.begin();
    while (it + 1 < harmonies.end()) {
        Note chord = bMajor.getChord(
            getNextInversion(prev, *(it + 1), bMajor.size(), 3, -5, 8),
            WHOLE_LENGTH * 2);
        chords.push_back(chord);
        it++;
        prev = *it;
    }
    Track bass{2};
    bass << chords;

    MidiOutput out{ {glitter, melody, bass}, 120};
    out.write("random.mid");
}
