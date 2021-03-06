#pragma once
#include <string>
#include <fstream>
#include <math.h>
#include "Song.h"
using namespace std;

class Wave
{
public:
    Wave();
    // POST: default Wave object is constructed.

    Wave(const char* fileName);
    //PRE:  theFile is either a Microsoft wave file (.wav) or a Song text file (formatted
    //         according to the specifications in Song.h)
    //POST: fileName has been set to theFile. wavData has been initialized to contain audio data
    //        from theFile, either raw PCM data from a .wav file or music generated according
    //        to the notes of a Song file. fileSize, numChannels, sampleRate, qLevel, samplesPerChannel,
    //        and songLength have been set according to information in theFile.

    Wave(Wave& toCopy);
    //PRE: toCopy is initialized
    //POST: FCTVAL == A new wave object is constructed with the same contents as toCopy

    ~Wave();
    //POST: Object is completely removed from virtual memory

    Wave& operator = (const Wave& toCopy);
    //PRE: toCopy is initialized
    //POST: FCTVAL == entire contents of this wave object is overwritten with that of toCopy

    vector<double>& operator [](int i);
    //PRE: 0 <= i < the number of audio channels
    //POST: FCTVAL == the ith channel of wave data as a vector object

    void ChangePitch(double fraction, bool keepSampleRate=true);
    //PRE: fraction > 0, keepSampleRate initialized
    //POST: if keepSampleRate is false, sampleRate is changed by fraction
    //      if keepSampleRate is true, sampleRate is not changed, but wavData is squeezed in time by fraction.
    //      Both will affect the playback time of the wave multiplicatively by the inverse of fraction.

    void ChangeSampleRate(int rate);
    //PRE: rate > 0
    //POST: The sample rate is changed to rate. The wav data is altered such that each
    //      sample is fitted with the closest existing sample that corresponds to the
    //      new sample rate. Playback time is unchanged.

    void BackChannel(double start=0.0, double end = -1.0);
    //PRE: 0 <= start <= end <= song length in seconds
    //POST: The samples in wavData corresponding to the time in seconds between the "start" and "end" times
    //      are reversed. If end is equal to -1, the samples in wavData corresponding to the time in seconds
    //      between the "start" time and the end of the wave are reversed.

    void BackChannel(string start, string end);
    //PRE:  start and end are both formatted in the form [hh:]mm:ss[.xxx]
    //POST: The samples in wavData corresponding to the time in seconds between the "start" and "end" times
    //      are reversed. If end is equal to -1, the samples in wavData corresponding to the time in seconds
    //      between the "start" time and the end of the wave are reversed.

    void WriteMovie(int width=512, int height=192, int fps = 12);
    //PRE:  width >= 256, 0 < height > 0, width and height measured in pixels, fps > 0
    //POST: Several frames of a visualization of the waveform of this wave have been exported to the
    //        working directory. There are fps frames exported for every second of audio in the wave.
    //        Each frame is width x height pixels. Each frame is in .bmp format. Frame filenames are
    //        constructed by taking the base of fileName and adding numbers.

    void WriteWaveForm(int width=22050, int height=200, bool drawTicks = false);
    //PRE:  width > 0, height > 0, drawTicks initialized
    //POST: Writes a new image of width width and height height, consisting of blue waveforms for each channel of the Wave Object,
    //      ordered from top to bottom according to channel number, least to greatest, on a black background. Image is saved to the
    //      file name of the wave object, but with .bmp extension. If drawTicks is enabled, the white tickmarks will be drawn to the
    //      image at a spacing such that the distance in pixels between two tickmarks corresponds to 1/8th of a second.

    Wave Split(int channel);
    //PRE:  channel > numChannels-1
    //POST: A wave object is returned with one channel, that designated by channel and as extracted by calling wave object

    void Save(const char* fileName);
    //PRE: fileName is initialized and not empty
    //POST: The file located at fileName is populated with the sound data in wavData according to the .wav standard
    //      with the appropriate header as required.
    
    string GetFileName() const;
    //POST: FCTVAL == the file name of the wave or song file

    long GetSamplesPerChannel() const;
    //POST: FCTVAL == the total number of sample data points in this sound per channel

    int GetNumChannels() const;
    //POST: FCTVAL == the number of audio channels in this wave object

    int GetSampleRate() const;
    //POST: FCTVAL == the sample rate of the wave object

    int GetQuantization() const;
    //POST: FCTVAL == the quantization level (as a power of 2) of this wave object
    
    int GetSongLength() const;
    //POST: FCTVAL == the length of the sound information in seconds

private:
    vector<double>* wavData;         //each entry contains one sample of the instanteous "loundness" of the wave

    string fileName;                //the filename of the input wave

    int numChannels;                //number of channels in sound (1 for mono, 2 for stereo, 6 for 5.1 Surround, etc..)
    int fileSize;                   //number of bytes in file
    int sampleRate;                 //sampling rate in Hz
    int qLevel;                     //qLevel is the quantization level of the sound, i.e. # of significant bits per sample.
    long samplesPerChannel;         //the total number of sample data points in the sound per channel
    double songLength;              //length of sound in seconds

    void WavInit();
    //PRE:  fileName corresponds to a valid .wav file.
    //POST: wavData has been initialized to contain all samples of audio data from the file
    //        denoted by fileName. fileSize, numChannels, sampleRate, qLevel, samplesPerChannel,
    //        and songLength have been set according to information in the .wav file header.

    void SongInit();
    //PRE:  fileName denotes a text file suitable for parsing in the Song class. (See Song.h for the
    //        format requirements.)
    //POST: Wave object constructed as a sound that realizes the notes stored in the file denoted by fileName
    //        by storing samples in wavData. numChannels == 1 for mono sound and qLevel = 16 bits per sample.
    //        sampleRate is DEFSAMPLERATE. Sound data in wavData has been normalized.

    int SetNote(double frequency, double duration, int sampleStart, double & phaseShift);
    //PRE: frequency >= 0, duration >= 0, sampleStart >= 0
    //POST: Sets the number of samples corresponding to duration of wavData starting at sampleStart to the
    //      appropriate amplitude based on the frequency of the note and the amplitude as a function of the duration
    //      according to the experimental model of the envelope of a note. FCTVAL == the sample at which the next note
    //      should start to write to.

    double PlayedDuration(double duration);
    //PRE: duration >= 0
    //POST: FCTVAL == The total duration to play a note in seconds, taking into account damping time

    double AmplitudePercent(int sampleNumber, double actualDuration);
    //PRE: sampleNumber >= 0, actualDuration != FULLAMPTIME
    //POST: FCTVAL == The fraction of the full amplitude of the waveform at the time corresponding to sampleNumber as a piecewise function
    //      of the actualDuration, as modeled experimentally.

    void Normalize();
    //POST: The contents of wavData is scaled down linearly such that its maximum amplitude is DEFAMPLITUDE

    void outputFrame(long sampleNumber, int frameWidth, int frameHeight, int fps);
    //PRE:  0 <= sampleNumber < wavData[i].size() for any value of i s.t. 0 <= i < numChannels
    //         width >= 256, 0 < height > 0, width and height measured in pixels, fps > 0
    //POST: One frame of visualization of the waveform of this wave has been exported to the
    //        working directory. This frame corresponds to a 0.005-second snapshot of the sound
    //        beginning at sampleNumber. The frame is width x height pixels. The frame is in .bmp format.
    //        The frame's filename is constructed by taking the base of fileName and appending the next
    //        unused number among files in the working directory.

    void WriteHeader(const char* fileName);
    //PRE:  private data fileSize, sampleRate, qLevel, samplesPerChannel, and numChannels are initialized
    //POST: The header information is written to the first 44 bytes of the file referenced by filename, based
    //      upon size, numChannels, rate, qLevel, and samplesPerChannel. The compression mode is written as
    //      uncompressed PCM.
    //      (WAV Header Reference: http://ccrma.stanford.edu/courses/422/projects/WaveFormat/)

    double ChannelAmp(int amplitude);
    //PRE:  amplitude is in two's complement hex
    //POST: FCTVAL == signed decimal representation of amplitude between -1 and 1, i.e. the
    //        fraction of the maxiumum amplitude of a sound wave

    double ChannelAmp(unsigned char dword[]);
    //PRE:  dword[0..1] contains two bytes of data representing the "amplitude" of a sound
    //        in little endian hexidecimal
    //POST: FCTVAL == signed decimal representation of amplitude between -1 and 1, i.e. the
    //        fraction of the maxiumum amplitude of a sound wave

    int InverseChannelAmp(double amp);
    //PRE:  amp contains a signed decimal repsentation of an amplitude between -1 and 1, i.e. the
    //         fraction of the maxiumum amplitude of a sound wave
    //POST: FCTVAL == two's complement equivalent of amp
};
