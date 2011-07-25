// Song class: Handles parsing of Songs from files of the format described below.
// Evan Fox and Doug Hogan
// Last Modified July 14, 2009.

// SONG FILE FORMAT
// ----------------
// The file is organized into streams of notes. Each stream contains
//   notes that are played from the start of the song to the end of the
//   song. Streams are played consecutively. Streams must be "padded"
//   with rests when notes are playing in other streams but not the
//   current stream.
//
// The first value in the file is an integer telling how many streams are
//   in the file.
//
// The remainder of the file is data for each stream. The beginning of a
//   stream is indicated by the string "newstream" on its own line. Then
//   data within streams is given, where each line contains three values,
//   separated by whitespace:
//    0. The note, a single character A-G; a character immediately followed by
//       'b' for a flat or 's' for a sharp, e.g. "Db" or "Cs"; or the character
//       'R' to indicate a rest.
//    1. The octave, an integer. (An integer must be supplied for a rest, but it
//       is ignored.)
//    2. The duration of the note, expressed as a decimal, e.g. 0.25 for a quarter
//       note, 1 for a whole note, etc. (Dotted notes should be converted to
//       durations similar, e.g. 0.375 = 0.25 + 0.125 for a dotted quarter note.)

#pragma once
#include <fstream>
#include <vector>
#include "NoteType.h"
using namespace std;

class Song
{
public:
    Song(const char* fileName);
    // PRE:  fileName corresponds to a file in the working directory that
    //         is in the song file format described in the header comments of
    //         Song.h
    // POST: song[0..numStreams-1] are initialized such that the ith entry
    //         contains all song data from the file given by fileName in elements
    //         0 to song[i].size()-1

    ~Song();
    // POST: Song.isDead. It lived a nice life. Okay, fine... real comment:
    // POST: memory allocated for song is freed.

    vector<NoteType> operator [](int i) const;
    // PRE:  0 < i < numStreams
    // POST: FCTVAL == song[i]

    void PrintSong();
    // PRE:  Assigned(song[0..numStreams-1]) such that the ith entry
    //         contains data in elements 0 to song[i].size()-1
    // POST: note, octave, and duration of each note in song[0..numStreams-1]
    //         have been displayed to screen with tabs between them, one note
    //         per line, blank lines after rests, and labels for when new streams
    //         start

    void PrintSongFrequencies();
    // PRE:  Assigned(song[0..numStreams-1]) such that the ith entry
    //         contains data in elements 0 to song[i].size()-1
    // POST: Frequencies in Hz of all notes in song[0...numStreams-1] have
    //         been displayed to screen, one per line

    int GetNumStreams();
    // POST: FCTVAL == numStreams

    bool IsValid();
    //POST: FCTVAL == Whether or not the song file is valid.

private:
    ifstream inFile;            // pointer to file containing song data
    vector<NoteType>* song;     // each vector holds data for each note of the song:
                                //   note, octave, duration, frequency
    int numStreams;             // how many streams of notes are in the song

    void ReadSongCSVFile();
    // PRE:  inFile points to a file in the song file format described
    //         in the header comments of Song.h
    // POST: song[0..numStreams-1] are initialized such that the ith entry
    //         contains all song data from the file given by inFile in elements
    //         0 to song[i].size()-1

};
