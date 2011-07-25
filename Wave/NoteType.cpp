// NoteType class: Handles information about a single musical note. 
//                 (Essentially a glorifed struct.) 
// Evan Fox and Doug Hogan
// Last Modified July 14, 2009. 

#include "NoteType.h"
using namespace std;

map<string, Note> NoteType::noteMap;        // An associative array mapping string 
                                            //   representation of notes to Note types

NoteType::NoteType(Note nNote, int iOctave, double dDuration)
// PRE:  Assigned(note), iOctave > 0, dDuration > 0
// POST: Class data members note, octave, and duration have been set to
//         nNote, iOctave, and dDuration, respectively, and frequency has
//         been set to the correct frequency in Hz for the note given by nNote
//         at octave iOctave
{
    note = nNote;
    octave = iOctave;
    duration = dDuration;
    frequency = Frequency(note, octave);
}

void NoteType::InitializeMap()
// POST: noteMap has been initalized with entries for all valid values in the 
//         Note enum
{
    noteMap["C"] = C;
    noteMap["Cs"] = Cs;
    noteMap["Db"] = Db;
    noteMap["D"] = D;
    noteMap["Ds"] = Ds;
    noteMap["Eb"] = Eb;
    noteMap["E"] = E;
    noteMap["F"] = F;
    noteMap["Fs"] = Fs;
    noteMap["Gb"] = Gb;
    noteMap["G"] = G;
    noteMap["Gs"] = Gs;
    noteMap["Ab"] = Ab;
    noteMap["A"] = A;
    noteMap["As"] = As;
    noteMap["Bb"] = Bb;
    noteMap["B"] = B;
    noteMap["R"] = REST;
}

Note NoteType::NoteFromString(string note)
// PRE:  note is any of the following: C, Cs, Db, D, Ds, Eb, E, F, Fs, Gb, G
//          Ab, A, As, Bb, B, R
// POST: FCTVAL == Note type equivalent of note
{
    if (noteMap.size() == 0)
        InitializeMap();
    return noteMap[note];
}

double NoteType::Frequency(Note note, int octave)
// PRE:  Assigned(note); octave > 0
// POST: FCTVAL == frequency in Hz of note "note" played at octave "octave" 
//                 for C <= note <= B. Otherwise, FCTVAL == 0 when note is 
//                 REST and -1 when note is ERROR
{
    if(note == REST)            // Rest == silence
        return 0;
    if(note == ERROR)           // Return obvious error value
        return -1;

    return 440*pow(2, -9/12.0)*pow(2, (note+(octave-4)*12)/12.0);

    // A4 is 440 Hz. There are 12 notes in an octave. We multiply by 2^-(9/12) to
    // shift our "base note" to C. We base our calculation on the 4th octave, so we
    // must subtract 4 from the octave.
}
