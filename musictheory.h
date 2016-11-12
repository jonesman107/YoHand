#ifndef MUSICTHEORY_H
#define MUSICTHEORY_H

#include <string>
#include <iostream>
#include <vector>

struct Note {
    char note;

};

class Harmony {
    public:
        static const int NOTES_PER_OCTAVE = 12;
        static const int RANGE = 2;
        static const int MIN_RANGE = 4;
        static const int KEYS = NOTES_PER_OCTAVE;
        static const int BUFFER = 7 * MIN_RANGE;
        static const int NUM_MIDI_NOTES = 120;
        static const int NOTES_PER_KEY = RANGE * 7;
        static const int ACCESSIBLE_NOTES_PER_KEY = NUM_MIDI_NOTES / NOTES_PER_OCTAVE * 7;
        static const int MIN_NOTE = NOTES_PER_OCTAVE * MIN_RANGE;

        int standard_key_intervals[7] = {2, 2, 1, 2, 2, 2, 1};
        int root_note_index[12] = {0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6};
        bool is_minor[6] = {false, true, true, false, false, true};
        virtual ~Harmony();
        Harmony();

        std::string getKeyName(int key);
        int getRootNote(float height);
        int getNoteInKeyAtHeight(float height);
        void getTriad(int note, bool closed, int triad[3], int prev[3]);
        void getNoteSequence(int root, float mu, int *sequence, int length);
        void getAccentNotes(int currentRoot, int triad[3], int noteInSequence, int accentNotes[2]);
        void changeKeyIfScheduled(int changeKey);

        int getKey() {
            return key;
        }

    private:
        std::string key_names[12] = {"C", "C#", "D", "Eb", "E", "F",
                                     "F#", "G", "G#", "A", "Bb", "B"};
        void rotate(int nums[RANGE * 3], int k);
        void reverse(int nums[RANGE * 3], int i, int j);
        int KEYMAP[KEYS][ACCESSIBLE_NOTES_PER_KEY];
        int ARPEGGIOS[KEYS][7][RANGE * 3];
        int key;
};

#endif // MUSICTHEORY_H
