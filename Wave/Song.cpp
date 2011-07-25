// Song class: Handles parsing of Songs from files of the format described below.
// Evan Fox and Doug Hogan
// Last Modified July 14, 2009.

#include "Song.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
using namespace std;

Song::Song(const char* fileName)
// PRE:  fileName corresponds to a file in the working directory that
//         is in the song file format described in the header comments of
//         Song.h
// POST: song[0..numStreams-1] are initialized such that the ith entry
//         contains all song data from the file given by fileName in elements
//         0 to song[i].size()-1
{
    inFile.open(fileName);

    if (!IsValid())
    {
        cout << "Song File not valid.\n";
        exit(1);
    }

    inFile.close();
    inFile.open(fileName);

    ReadSongCSVFile();

    inFile.close();
}

Song::~Song()
// POST: Song.isDead. It lived a nice life. Okay, fine... real comment:
// POST: memory allocated for song is freed.
{
    delete[] song;
}

vector<NoteType> Song::operator [](int i) const
// PRE:  0 < i < numStreams
// POST: FCTVAL == song[i]
{
    return song[i];
}

void Song::PrintSong()
// PRE:  Assigned(song[0..numStreams-1]) such that the ith entry
//         contains data in elements 0 to song[i].size()-1
// POST: note, octave, and duration of each note in song[0..numStreams-1]
//         have been displayed to screen with tabs between them, one note
//         per line, blank lines after rests, and labels for when new streams
//         start
{
    for(int j = 0; j < numStreams; j++)                 // go through all streams in song
    {
        cout << "\n\nStream " << j << ":\n";

        for(int i = 0; i < song[j].size(); i++)                   // go through all notes in song
        {
            cout << song[j][i].note << "\t"
                 << song[j][i].octave << "\t"
                 << song[j][i].duration
                 << ((song[j][i].note == REST) ? "\n" : "")
                 << "\n";
        }
    }
}

void Song::PrintSongFrequencies()
// PRE:  Assigned(song[0..numStreams-1]) such that the ith entry
//         contains data in elements 0 to song[i].size()-1
// POST: Frequencies in Hz of all notes in song[0...numStreams-1] have
//         been displayed to screen, one per line
{
    for(int j = 0; j < numStreams; j++)                      // go through all streams in song
    {
        cout << "\n\nStream " << j << ":\n";

        for(int i = 0; i < song[j].size(); i++)              // go through all notes in stream
            cout << song[j][i].frequency
                 << " for " << song[j][i].duration << endl;
    }
}

int Song::GetNumStreams()
// POST: FCTVAL == numStreams
{
    return numStreams;
}

bool Song::IsValid()
//POST: FCTVAL == Whether or not the song file is valid.
{
    int nStreams;           //The number of streams as designated by the first character in the file.
    int streamCount = 0;    //Counter for the number of "newstream"s in the file
    string temp;            //holder for each string in the file
    inFile >> nStreams;

    while (inFile >> temp)  //count the number of times the word " newstream " appears in the file.
    {
        if (temp == "newstream")
            streamCount++;
    }

    return streamCount == nStreams; //A valid file has the same number of "newstream"s as the first character states
}

void Song::ReadSongCSVFile()
// PRE:  inFile points to a file in the song file format described
//         in the header comments of Song.h
// POST: song[0..numStreams-1] are initialized such that the ith entry
//         contains all song data from the file given by inFile in elements
//         0 to song[i].size()-1
{
    int curStream = -1;           // holds index of stream currently being
                                  //  written to (started at -1 because we will
                                  //  increment it before the first stream)
    string note;                  // note from current line
    int octave;                   // octave from current line
    double duration;              // duration to play note from current line

    inFile >> numStreams;                       // first get how many streams from file...
    song = new vector<NoteType>[numStreams];    // ...and use it to allocate enough streams

    while(inFile >> note)                       // read string from file - it's either a
    {                                           //   note or "newstream"
        if(note == "newstream")                 // indicator to advance to next stream
            curStream++;
        else                                    // line is a note --> get rest and store
        {
            inFile >> octave >> duration;       // read rest of note data

            song[curStream].push_back(NoteType(NoteType::NoteFromString(note),
                                      octave, duration));                       // store it
        }
    }
}


