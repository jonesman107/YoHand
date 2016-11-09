#include "musictheory.h"
#include "math.h"
#include <vector>


Harmony::Harmony() {
    key = 0;
    int KEY_C[7] = {0, 2, 4, 5, 7, 9, 11};
    int index[KEYS];
    // creates a map of each key to arrays of midi notes in that key
    for (int i = 0; i < KEYS; i++) {
        index[i] = 0; // for following loop
        int j = (7 - root_note_index[i]) % 7;
        // assign note values
        for (int k = 0; k < ACCESSIBLE_NOTES_PER_KEY; k++) {
            int octaveInRange = (k / 7) * NOTES_PER_OCTAVE;
            int indexOfScale = (KEY_C[(j + k) % 7] + i) % NOTES_PER_OCTAVE;
            KEYMAP[i][k] = indexOfScale + octaveInRange;
        }
    }

    for (int i = 0; i < KEYS; i++) {
        for (int j = 0; j < 7; j++) {
            int min_ = 120;
            int minIndex = 0;
            int chordIndex = (root_note_index[i] + j) % 7;
            for (int k = 0; k < RANGE; k++) {
                for (int l = 0; l < 3; l++) {
                    int val = KEYMAP[i][chordIndex + k * 7 + l * 2] % 24 + MIN_NOTE;
                    if (val < min_) {
                        min_ = val;
                        minIndex = k * 3 + l;
                    }
                    ARPEGGIOS[i][chordIndex][k * 3 + l] = val;
                }
            }
            if (minIndex > 0) {
                if (i < 2) {
                    std::cout << key_names[i] << std::endl;
                    std::cout << "rotating " << j << " by " << (RANGE * 3 - minIndex) << std::endl;
                }
                rotate(ARPEGGIOS[i][chordIndex], RANGE * 3 - minIndex);
            }
        }
    }

    // prints all computed notes and arpeggios
//    for (int i = 0; i < 3; i++) {
//        std::cout << key_names[i] << std::endl;
//        for (int j = 0; j < ACCESSIBLE_NOTES_PER_KEY; j++) {
//            std::cout << KEYMAP[i][j] << ", ";
//            if (j > 0 && j % 7 == 6) {
//                std::cout << std::endl;
//            }
//        }

//        std::cout << "Arpeggios" << std::endl;
//        for (int j = 0; j < 7; j++) {
//            std::cout << "from root " << key_names[j] << std::endl;
//            for (int k = 0; k < RANGE * 3; k++) {
//                std::cout << ARPEGGIOS[i][j][k] << ", ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//    }
}

Harmony::~Harmony() {

}

void Harmony::rotate(int nums[RANGE * 3], int k) {
    int len = RANGE * 3;
    if (k > 0 && len > 1) {
        k %= len;
        reverse(nums, 0, len - k - 1);
        reverse(nums, len - k, len - 1);
        reverse(nums, 0, len - 1);
    }
}

void Harmony::reverse(int nums[RANGE * 3], int i, int j) {
    while (i < j) {
        int k = nums[i];
        nums[i++] = nums[j];
        nums[j--] = k;
    }
}

std::string Harmony::getKeyName(int key) {
    return key_names[key % NOTES_PER_OCTAVE];
}

void Harmony::changeKeyIfScheduled(int changeKey) {
    if (changeKey > 0) {
        // up one fifth
        key = (key + 7) % NOTES_PER_OCTAVE;
        std::cout << "key changed to " << key_names[key] << std::endl;
    } else if (changeKey < 0) {
        // down one fifth
        key = (key + 5) % NOTES_PER_OCTAVE;
        std::cout << "key changed to " << key_names[key] << std::endl;
    }
}

int Harmony::getRootNote(float height) {
    // rounds note to the nearest root note that is not the 7 chord
    float preIndex = (float) NOTES_PER_KEY * height;
    int index = (int) floor(preIndex);
    if (index % 7 == (root_note_index[key] + 6) % 7)
        index = (preIndex < index) ? (index - 1) : (index + 1);
    index += BUFFER;
    return index;
}

// returns a sequence of length n
void Harmony::getNoteSequence(int root, float mu, int *sequence, int seqLength) {
  int *ARP = ARPEGGIOS[key][root % 7];
  int arpLength = RANGE * 3;
  int arpIndex = (int) round(1.0 * (root % 7) / (double) NOTES_PER_KEY * (double) arpLength);
  arpIndex = fmax(0, fmin(arpLength - 1, arpIndex));
  int x = (mu >= 0.0) ? 1 : -1;
  for (int i = 0; i < seqLength; i++) {
    sequence[i] = ARP[arpIndex] + NOTES_PER_OCTAVE;

    if (arpIndex == 0) {
      x = 1;
    } else if (arpIndex == arpLength - 1) {
      x = -1;
    }
    arpIndex = (arpIndex + x) % arpLength;
  }
}

static int indx[8][3] = {{0,1,2},{0,4,2},{0,1,5},{0,4,5},{3,1,2},{3,1,5},{3,4,2},{3,4,5}};
static int combos[8][3];
void Harmony::getTriad(int note, bool open, int triad[3], int prev[3]) {
    int root, third, fifth;

    if (open) {
        triad[0] = root = KEYMAP[key][note];
        triad[1] = third = KEYMAP[key][note - 5];
        triad[2] = fifth = KEYMAP[key][note + 4];
    } else {
        triad[0] = root = KEYMAP[key][note];
        triad[1] = third = KEYMAP[key][note + 2];
        triad[2] = fifth = KEYMAP[key][note - 3];
    }
}

void Harmony::getAccentNotes(int currentRoot, int triad[3], int noteInSequence, int accentNotes[2]) {
    accentNotes[0] = (rand() % 2) ? KEYMAP[key][currentRoot + 1] : KEYMAP[key][currentRoot + 3];
    int i = RANGE * 3 - 2;
    accentNotes[1] = 0;
    while (i >= 0 && accentNotes[1] != 0) {
        int noteToCheck = ARPEGGIOS[key][currentRoot % 7][i];
        if (noteToCheck != noteInSequence) { // check current note in sequence
            bool found = false;
            for (int j = 0; j < 3 && !found; j++) { // check the triad
                found = noteToCheck == triad[i];
            }
            if (!found) // if not found in either one assign the note
                accentNotes[1] = noteToCheck;
        }
    }
}
