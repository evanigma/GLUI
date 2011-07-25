// NoteType class: Handles information about a single musical note. 
//                 (Essentially a glorifed struct.) 
// Evan Fox and Doug Hogan
// Last Modified July 14, 2009. 

#pragma once
#include <math.h>
#include <string>
#include <map>
#include <utility>
using namespace std;

// All possible musical notes. Integer values start from 0 at C since octaves 
//   change at C. Cs and Db denote the same frequency and hence have the same
//   integer value. 
enum Note{C = 0, Cs = 1, Db = 1, D = 2, Ds = 3, Eb = 3, E = 4, F = 5, Fs = 6,
          Gb = 6, G = 7, Gs = 8, Ab = 8, A = 9, As = 10, Bb = 10, B = 11, 
          REST = 12, ERROR = -1};

struct NoteType
{
   Note note;                               // The note's "letter" (see above enum)
   int octave;                              // Octave of the note
   double duration;                         // The length of the note as a fraction of 
                                            //   a whole note (e.g. 0.25 for quarter note)
   double frequency;                        // The frequency of sound for this note, in Hz
   
   static map<string, Note> noteMap;        // An associative array mapping string 
                                            //   representation of notes to Note types

   NoteType(Note nNote, int iOctave, double dDuration);
   // PRE:  Assigned(note), iOctave > 0, dDuration > 0
   // POST: Class data members note, octave, and duration have been set to
   //         nNote, iOctave, and dDuration, respectively, and frequency has
   //         been set to the correct frequency in Hz for the note given by nNote
   //         at octave iOctave

   static void InitializeMap();
   // POST: noteMap has been initalized with entries for all valid values in the 
   //         Note enum

   static double Frequency(Note note, int octave);
   // PRE:  Assigned(note); octave > 0
   // POST: FCTVAL == frequency in Hz of note "note" played at octave "octave" 
   //                 for C <= note <= B. Otherwise, FCTVAL == 0 when note is 
   //                 REST and -1 when note is ERROR
      
   static Note NoteFromString(string note);
   // PRE:  note is any of the following: C, Cs, Db, D, Ds, Eb, E, F, Fs, Gb, G
   //          Ab, A, As, Bb, B, R
   // POST: FCTVAL == Note type equivalent of note
};
