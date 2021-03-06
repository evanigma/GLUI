#include "Wave.h"
#include <stdlib.h>
#include "Utility.h"
#include "Turtle.h"
#include <string>
#include <iostream>
using namespace std;

const double RDAMPTIME = 0.0069;       //as derived from experiment, the time in seconds in which the note needs to reach full amplitude
const double FULLAMPTIME = 0.0369;     //as derived from experiment, the time in seconds at which the note starts to damp
const int HEADERSIZE = 44;             //size in bytes of wav header
const int MSFOOTERSIZE = 48;           //size in bytes of Microsoft wav footer
const double WHOLENOTETIME = 2;        //how long to play a whole note, in seconds
const int DEFSAMPLERATE = 22050;       //how many samples of sound are made per second, default
const double DEFAMPLITUDE = 0.7;       //maximum amplitude of sound waves

Wave::Wave()
// POST: default Wave object is constructed.
{
    wavData = NULL;
}

Wave::Wave(const char* theFile)
//PRE:  theFile is either a Microsoft wave file (.wav) or a Song text file (formatted
//         according to the specifications in Song.h)
//POST: fileName has been set to theFile. wavData has been initialized to contain audio data
//        from theFile, either raw PCM data from a .wav file or music generated according
//        to the notes of a Song file. fileSize, numChannels, sampleRate, qLevel, samplesPerChannel,
//        and songLength have been set according to information in theFile.
{
    unsigned char holder[4];                                        //holds bytes 8, 9, 10, 11 of the wave file

    fileName = theFile;                                             //save fileName
    ifstream inFile(fileName.c_str(), ios::binary);                 //pointer to the input file

    inFile.seekg(0x8);                                              //go to portion of file that should read "WAVE"
    inFile.read(reinterpret_cast<char*>(holder), 4);                //  in little endian ASCII if this were a wav file
    inFile.close();

    if (Utility::LittleEndianCharToInt(holder, 4) != 0x45564157)    //file is not a wav
        SongInit();                                                 //so assume it's a Song and process accordingly
    else                                                            //file is a wav
        WavInit();
}

Wave::Wave(Wave& toCopy)
//PRE: toCopy is initialized
//POST: FCTVAL == A new wave object is constructed with the same contents as toCopy
{
    wavData = NULL;  //Set pointer object to null
    *this = toCopy;  //Copy over contents of toCopy into this object
}

Wave::~Wave()
//POST: Object is completely removed from virtual memory
{
    delete[] wavData;   //delete contents of wavData
    wavData = NULL;     //set pointer to null
}

Wave& Wave::operator = (const Wave& toCopy)
//PRE: toCopy is initialized
//POST: FCTVAL == entire contents of this wave object is overwritten with that of toCopy
{
    fileName = toCopy.fileName;                    //Copy members of toCopy to object
    fileSize = toCopy.fileSize;
    sampleRate = toCopy.sampleRate;
    qLevel = toCopy.qLevel;
    samplesPerChannel = toCopy.samplesPerChannel;
    songLength = toCopy.songLength;
    numChannels = toCopy.numChannels;


    delete[] wavData;                               //delete entire contents of wavData...
    wavData = NULL;                                 // ...and remove pointer

    wavData = new vector<double>[numChannels];      //allocate the memory needed to copy toCopy.wavData

    for (int i=0; i < numChannels; i++)             //loop through the entire conents of toCopy.wavData...
        for (int j=0; j < samplesPerChannel; j++)
            wavData[i].push_back(toCopy.wavData[i][j]); //...and copy over to current wavData

    return *this;
}

vector<double>& Wave::operator [](int i)
//PRE: 0 <= i < the number of audio channels
//POST: FCTVAL == the ith channel of wave data as a vector object
{
    return wavData[i];
}

void Wave::ChangePitch(double fraction, bool keepSampleRate)
//PRE: fraction > 0, keepSampleRate initialized
//POST: if keepSampleRate is false, sampleRate is changed by fraction
//      if keepSampleRate is true, sampleRate is not changed, but wavData is squeezed in time by fraction.
//      Both will affect the playback time of the wave multiplicatively by the inverse of fraction.
{
    int oldSampleRate = sampleRate;         //Remember the old sample rate in samples per second
    sampleRate *= fraction;                 //change the new sample rate by fraction

    if (keepSampleRate)                     //if we'd like to keep the old sample rate,
        ChangeSampleRate(oldSampleRate);    // restore the old sample rate.
}

void Wave::ChangeSampleRate(int rate)
//PRE: rate > 0
//POST: The sample rate is changed to rate. The wav data is altered such that each
//      sample is fitted with the closest existing sample that corresponds to the
//      new sample rate. Playback time is unchanged.
{
    vector<double>* temp = new vector<double>[numChannels];      //Create and allocate memory for wave data,
                                                               //one vector per channel.

    double rateRatio = double(rate)/sampleRate;                  //the unitless ratio of the new sample rate
                                                               //to the old sample rate.
    int finalSamplesPerChannel = samplesPerChannel*rateRatio;   //the new samples per channel, in samples

    cout << "Changing Sample Rate...\n";                        //Display friendly progress message

    for (int i=0; i < numChannels; i++)                         //loop through all the channels of the wav data
    {
        for (int j=0; j < finalSamplesPerChannel; j++)          //loop through all samples of each channel
        {
            Utility::Bar(cout, j, finalSamplesPerChannel-1);    //Display friendly progress bar
            temp[i].push_back(wavData[i][int(j/rateRatio)]);    //Push back the nearest sample of the old wav
                                                                //data to the corresponding sample in the new
                                                                //wavData, temp.
        }
        cout << endl;
    }


    samplesPerChannel = finalSamplesPerChannel;                 //reset class members
    sampleRate = rate;
    delete[] wavData;                                            //free the old wav data,
    wavData = temp;                                             //and reset the pointer.
}

void Wave::BackChannel(double start, double end)
//PRE: 0 <= start <= end <= song length in seconds
//POST: The samples in wavData corresponding to the time in seconds between the "start" and "end" times
//      are reversed. If end is equal to -1, the samples in wavData corresponding to the time in seconds
//      between the "start" time and the end of the wave are reversed.
{
    int holder;                                     //holder for swapping start and end if needed.

    int sampleStart = start*sampleRate;             //the sample number corresponding to the start time
    int sampleEnd = end*sampleRate;                 //the sample number corresponding to the end time

    if (start < 0)                                  //negative start times are not allowed, reset to zero.
        sampleStart = 0;

    if (end == -1 || end > songLength)              //an end time of "-1" corresponds to the last sample
        sampleEnd = samplesPerChannel;              //end times greater than the song length are not allowed.

    if (sampleStart > sampleEnd)                    //if start and end are in reverse order, swap them.
    {
        holder = sampleStart;
        sampleStart = sampleEnd;
        sampleEnd = holder;
    }

    vector<double>* temp = new vector<double>[numChannels]; //holder for new wave data

    cout << "Backchannelling...\n";                 //Display friendly progress message

    for (int i=0; i < numChannels; i++)             //Loop through all channels of the wave,
    {
        for (int j=0; j < samplesPerChannel; j++)   //and all samples too.
        {
            Utility::Bar(cout, j, samplesPerChannel-1);             //Display a progress bar

            temp[i].push_back((j > sampleStart && j < sampleEnd)    //if we are in the range of samples to be reversed,
                                ? wavData[i][sampleEnd-(j-sampleStart)-1]   //push back the opposite sample.
                                : wavData[i][j]);                           //else, push back the forward sample.
        }
        cout << endl;
    }

    delete[] wavData;                               //delete old data
    wavData = temp;                                 //reset pointer to new data
}

void Wave::BackChannel(string start, string end)
//PRE:  start and end are both formatted in the form [hh:]mm:ss[.xxx]
//POST: The samples in wavData corresponding to the time in seconds between the "start" and "end" times
//      are reversed. If end is equal to -1, the samples in wavData corresponding to the time in seconds
//      between the "start" time and the end of the wave are reversed.
{
    double resultStart;                         //start time of backchanneling, in seconds
    double resultEnd;                           //end time of backchanneling, in seconds
    resultStart = Utility::TimeInSeconds(start);         //convert start time to seconds

    if(end == "-1")                             //an end time of "-1" corresponds to the last sample
        resultEnd = -1;
    else                                        //end time is a valid time...
        resultEnd = Utility::TimeInSeconds(end);         //...so convert it to seconds

    BackChannel(resultStart, resultEnd);        //call version of this method that takes in times in seconds
}


void Wave::WriteMovie(int width, int height, int fps)
//PRE:  width >= 256, 0 < height > 0, width and height measured in pixels, fps > 0
//POST: Several frames of a visualization of the waveform of this wave have been exported to the
//        working directory. There are fps frames exported for every second of audio in the wave.
//        Each frame is width x height pixels. Each frame is in .bmp format. Frame filenames are
//        constructed by taking the base of fileName and adding numbers.
{
    cout << "Writing Movie...\n";
    for (int i=0; i < fps*songLength; i++)                                          //output fps frames per second of visualization
    {
        outputFrame(i/(fps*songLength)*samplesPerChannel, width, height, fps);      //first argument is the fraction of the
                                                                                    //  sound we've already passed by

        Utility::Bar(cout, i, fps*songLength);                                      //display progress bar
    }
    cout << endl;
}

void Wave::WriteWaveForm(int width, int height, bool drawTicks)
//PRE:  width > 0, height > 0, drawTicks initialized
//POST: Writes a new image of width width and height height, consisting of blue waveforms for each channel of the Wave Object,
//      ordered from top to bottom according to channel number, least to greatest, on a black background. Image is saved to the
//      file name of the wave object, but with .bmp extension. If drawTicks is enabled, the white tickmarks will be drawn to the
//      image at a spacing such that the distance in pixels between two tickmarks corresponds to 1/8th of a second.
{
    Image myImage(width, height, 0, true);      //Create an image of size width x height on a black background. Enable Status Messages.
    Turtle* turtles = new Turtle[numChannels];  //Allocate one turtle (for drawing on the image) for each channel.
    Turtle tickTurtle(&myImage, 0xFFFFFF);      //Create a new turtle for drawing tickmarks (on my image and of white color);
    long curSample;                             //used to determine what sample of the waveform corresponds to what pixel on the image.
    double pixelsPerSecond = width/songLength;  //The number of pixels corresponding to one second of the song

    for (int i=0; i < numChannels; i++)         //Loop through each channel...
    {
        turtles[i] = Turtle(&myImage, 0x0088FF);            //...in order initialize each turtle (for drawing on myImage and of color blue)
        turtles[i].MoveTo(0, height/numChannels*(i+1/2.0)); //...and move them to their starting positions (for each channel)
    }

    cout << "Writing Waveform...\n";            //Display friendly progress message...

    for(int i=0; i < width; i++)                                //Loop through each pixel across the width of the image
    {
        Utility::Bar(cout, i, width);           //...and bar
        curSample = (double(i)/(width-1))*samplesPerChannel;    //Find the nearest sample of the wave corresponsing to that pixel...

        for (int j=0; j<numChannels; j++)                       //...and loop through each channel of our waveform
            turtles[j].LineTo(i,height/numChannels*(j+1/2.0+wavData[j][curSample]/2)); //and draw a line to the next sample in this channel
    }
    Utility::Bar(cout, width, width);           //Progress Bar should be 100% complete
    cout << endl;

    if (drawTicks)                              //If the user would like to draw ticks on the image, draw ticks.
    {
        for (double i=0; i < width; i+=pixelsPerSecond/8)   //loop through each pixel of the image
        {
            if (i/pixelsPerSecond - floor(i/pixelsPerSecond) <= 0.000000001)  //If our pixel corresponds to a whole second (well, close
                                                                              //enough due to C++'s problem with floating-point precision),
            {
                tickTurtle.MoveTo(i, height/2+10);          //then make a big tick mark (of 20 pixels in height).
                tickTurtle.LineTo(i, height/2-10);
            }
            else                                            //else,
            {
                tickTurtle.MoveTo(i, height/2+5);           //make a small tick mark (of 10 pixels in height).
                tickTurtle.LineTo(i, height/2-5);
            }
        }
    }

    myImage.Save((fileName.substr(0,fileName.length()-3)+"bmp").c_str());   //Save the image to fileName[-.ext].bmp

    //Free the turtles!
    delete[] turtles;
    turtles = NULL;
}

Wave Wave::Split(int channel)
//PRE:  channel > numChannels-1
//POST: A wave object is returned with one channel, that designated by channel and as extracted by calling wave object
{
    if (channel > numChannels-1) //Test to see that channel is valid... if not, display warning message and set channel to zero
    {
        cout << "Invalid channel. " << channel << " > " << numChannels-1 << "\n...returning first channel.\n";
        channel = 0;
    }
    Wave toReturn;                                                   //Create new object
    toReturn.fileName = fileName.substr(0, fileName.length()-4)      //Append Channel to filename
                        + "_" + char(channel+1+int('0'))+".wav";
    toReturn.fileSize = samplesPerChannel*2+HEADERSIZE;              //2 bytes per sample + size in bytes of header

    toReturn.numChannels = 1;                                        //We have only one channel...
    toReturn.samplesPerChannel = samplesPerChannel;                  //...with the same number of samplesPerChannel...
    toReturn.qLevel = qLevel;                                        //...and the same qLevel...
    toReturn.sampleRate = sampleRate;                                //...and sampleRate...
    toReturn.songLength = songLength;                                //...and songLength.

    toReturn.wavData = new vector<double>[numChannels];              //Allocate enough memory for one channel for wavData

    cout << "Splitting...\n";                                        //Display friendly progress message...

    for (int i=0; i < samplesPerChannel; i++)                               //Loop through the number of samples in one channel...
    {
        Utility::Bar(cout, i, samplesPerChannel);                    //...and bar
        toReturn.wavData[0].push_back(wavData[channel][i]);                 //...and copy over the corresponding channel to our new wavData
    }

    Utility::Bar(cout, samplesPerChannel, samplesPerChannel);       //The progress bar should have finished.
    cout << endl;

    return toReturn;                                                //return the new wave object
}

void Wave::Save(const char* fileName)
//PRE: fileName is initialized and not empty
//POST: The file located at fileName is populated with the sound data in wavData according to the .wav standard
//      with the appropriate header as required.
{
    WriteHeader(fileName);                                  //Write the header of fileName with the data of our Wave Object
    ofstream outFile(fileName, ios::binary | ios::app);     //Open the file at fileName for appending binary data.
    unsigned char holder[2];                                //holder for two binary bytes to be written out to the ofstream

    outFile.seekp(0x2c);                                    //Set the "write-at" pointer to position 0x2c (Wav Data starts here)

    cout << "Saving as " << fileName << "...\n";            //Display friendly progress message

    for (int i=0; i < samplesPerChannel; i++)               //loop through all samples...
    {
        Utility::Bar(cout, i, samplesPerChannel);           //Display friendly progress bar
        for (int j=0; j<numChannels; j++)                                   //...of all channels in wavData...
        {
            holder[0] = InverseChannelAmp(wavData[j][i]) & 0xFF;            //Take the least significant byte of our sample and write it first
            holder[1] = (InverseChannelAmp(wavData[j][i]) & 0xFF00) >> 8;   //Take the next least significant byte and write is second (little-endianness)
            outFile.write(reinterpret_cast<char*>(holder), 2);              //...so that we can write them all to file in order
        }
    }

    Utility::Bar(cout, samplesPerChannel, samplesPerChannel); //Progress bar should be finished
    cout << endl;

    outFile.close();                                          //We are done writing. Close the file.
}

string Wave::GetFileName() const
//POST: FCTVAL == the file name of the wave or song file
{
	return fileName;
}

long Wave::GetSamplesPerChannel() const
//POST: FCTVAL == the total number of sample data points in this sound per channel
{
    return samplesPerChannel;
}

int Wave::GetNumChannels() const
//POST: FCTVAL == the number of audio channels in this wave object
{
    return numChannels;
}

int Wave::GetSampleRate() const
//POST: FCTVAL == the sample rate of the wave object
{
    return sampleRate;
}

int Wave::GetQuantization() const
//POST: FCTVAL == the quantization level (as a power of 2) of this wave object
{
    return qLevel;
}

int Wave::GetSongLength() const
//POST: FCTVAL == the length of the sound information in seconds
{
	return songLength;
}

void Wave::WavInit()
//PRE:  fileName corresponds to a valid .wav file.
//POST: wavData has been initialized to contain all samples of audio data from the file
//        denoted by fileName. fileSize, numChannels, sampleRate, qLevel, samplesPerChannel,
//        and songLength have been set according to information in the .wav file header.
{
    unsigned char holder[4];                                        //holds up to 4 bytes of raw data from file
    ifstream inFile(fileName.c_str(), ios::binary);                 //pointer to input wave file

    // READ WAV HEADER
    // (WAV Header Reference: http://ccrma.stanford.edu/courses/422/projects/WaveFormat/)
    // We ignore parts of the header that do not vary based upon the sound. (Compare to WriteHeader.)
    // All data is stored in Little Endian format, hence bytes are reversed from "natural" order. We
    //   reverse this using LittleEndianCharToInt.

    inFile.seekg(0x4);                                              //bytes 4, 5, 6, 7 contain the file size
    inFile.read(reinterpret_cast<char*>(holder), 4);                //  in B - 8 (for part of the header)
    fileSize = Utility::LittleEndianCharToInt(holder, 4)+8;

    inFile.seekg(0x16);                                             //bytes 16 and 17 contain the number of channels
    inFile.read(reinterpret_cast<char*>(holder), 2);
    numChannels = Utility::LittleEndianCharToInt(holder, 2);

    inFile.seekg(0x18);                                             //bytes 18, 19, 20, and 21 contain the sample
    inFile.read(reinterpret_cast<char*>(holder), 4);                //  rate in Hz
    sampleRate = Utility::LittleEndianCharToInt(holder, 4);

    inFile.seekg(0x22);                                             //bytes 22 and 23 contain the quantization level
    inFile.read(reinterpret_cast<char*>(holder), 2);                //  level, i.e. significant bits/sample
    qLevel = Utility::LittleEndianCharToInt(holder, 2);

    samplesPerChannel = fileSize-HEADERSIZE;                        //begin calculation of how many samples of data
                                                                    //  we have per channel by assuming file is only
                                                                    //  header bytes and sample bytes

    inFile.seekg(fileSize-MSFOOTERSIZE);                            //check if there's a Microsoft-generated footer
    inFile.read(reinterpret_cast<char*>(holder), 4);                //  at the end of the file...

    if (Utility::LittleEndianCharToInt(holder, 4) == 0x5453494C ||  //footer would begin with bytes reading "list" or
        Utility::LittleEndianCharToInt(holder, 4) == 0x7473696C)    //  "LIST" in little endian (hence reversed ASCII here)
        samplesPerChannel -= MSFOOTERSIZE;                          //subtract header size from # samples if so

    samplesPerChannel /= numChannels*2;                             //each sample takes 2 bytes and we want samples per
                                                                    //  channel, so divide accordingly

    songLength = double(samplesPerChannel)/sampleRate;              //sample rate in samples/second. dimensional analysis
                                                                    //  gives us the song length in seconds

    // ALLOCATE MEMORY FOR SAMPLES

    wavData = new vector<double>[numChannels];                      //create a vector for each channel's sound data

    for(int i=0; i < numChannels; i++)                              //...and size it
        wavData[i].resize(samplesPerChannel);

    // READ ACTUAL SAMPLE DATA

    inFile.seekg(0x2C);                                             //actual sound data starts right after the header

    cout << "Loading " << fileName << "...\n";

    for (int i=0; i < samplesPerChannel; i++)                       //read each sample, samples contain all channels
    {                                                               //  grouped together
        Utility::Bar(cout, i, samplesPerChannel);                   //display progress bar

        for (int j=0; j < numChannels; j++)                         //read data for all channels of this sample
        {
            inFile.read(reinterpret_cast<char*>(holder), 2);
            wavData[j][i] = ChannelAmp(holder);                     //store it as a value between -1 and 1
        }
    }

    Utility::Bar(cout, samplesPerChannel, samplesPerChannel);       //display progress bar at the end of reading samples
    cout << endl;

    inFile.close();
}

void Wave::SongInit()
//PRE:  fileName denotes a text file suitable for parsing in the Song class. (See Song.h for the
//        format requirements.)
//POST: Wave object constructed as a sound that realizes the notes stored in the file denoted by fileName
//        by storing samples in wavData. numChannels == 1 for mono sound and qLevel = 16 bits per sample.
//        sampleRate is DEFSAMPLERATE. Sound data in wavData has been normalized.
{
    int sStart;                     //the sample number to start writing sound information at

    Song mySong(fileName.c_str());  //song object

    numChannels = 1;                //song objects are (for now) mono-channel.
    sampleRate = DEFSAMPLERATE;     //store default sample rate in class variable.
    qLevel = 16;                    //the only quantization level currently handled is 16 significant bits per sample

    double phaseShift = 0;          //phase shift for sound wave to eliminate "crackling" during note changes
                                    // (in seconds); value is updated during conversion of a Song to a wave

    wavData = new vector<double>[numChannels];       //allocate memory for wavData

    cout << "Loading " << fileName << "...\n";       //Display loading message

    for (int i=0; i < mySong.GetNumStreams(); i++)   //loop through each stream of the song and initialize wavData
    {
        sStart = 0;                                   //each stream writes first at sample number zero
        for (int j=0; j < mySong[i].size(); j++)      //loop through each note of the stream and set wavData correspondingly
        {
            Utility::Bar(cout, j+1, mySong[i].size()); //display loading bar

            sStart = SetNote(mySong[i][j].frequency, mySong[i][j].duration, sStart, phaseShift); //translate note mathematically to wavData
        }
        Utility::Bar(cout, mySong[i].size(), mySong[i].size()); //display completely loaded bar
        cout << endl;
    }

    Normalize();                                         //scale the wave data down to the zero to one scale for .wav

    samplesPerChannel = wavData->size();                 //the size of wavData is the number of samples it contains
    songLength = double(samplesPerChannel)/sampleRate;
    fileSize = samplesPerChannel*2+HEADERSIZE;          //Each sample consists of two bytes. The .wav header chunk for
                                                        //file size is the actual file size minus eight
}

int Wave::SetNote(double frequency, double duration, int sampleStart, double & phaseShift)
//PRE: frequency >= 0, duration >= 0, sampleStart >= 0
//POST: Sets the number of samples corresponding to duration of wavData starting at sampleStart to the
//      appropriate amplitude based on the frequency of the note and the amplitude as a function of the duration
//      according to the experimental model of the envelope of a note. FCTVAL == the sample at which the next note
//      should start to write to.
{
    double actualDuration = PlayedDuration(duration);     //the extended duration of the note (to account for damping)
    double seconds = WHOLENOTETIME*actualDuration;        //the number of seconds of data to actually write
    int numSamples = DEFSAMPLERATE*seconds;               //the number of samples of data to actually write

    if (frequency != 0)                                   //don't divide by zero!
        phaseShift /= frequency;                          //the phaseShift at this point is the unitless fraction of a period.
                                                          //    to find the number of actual seconds to shift the wave,
                                                          //    we multiply by the actual period (or divide by the frequency)

    for (int i=0; i < numSamples; i++)                    //loop through the number of samples to write to wavData
    {
        if (sampleStart+i >= wavData[0].size())              //if the sample we are to write to hasn't been initialized yet,
            wavData[0].push_back(0);                         //initialize it to zero

        wavData[0][sampleStart+i] += AmplitudePercent(i, actualDuration)* //The time snapshot of a propagating sound wave at x=0
                                  sin(-2*M_PI*frequency*                  //(for convenience) is modeled by A*sin(-w(t+O)), with
                                  (double(i)/DEFSAMPLERATE+phaseShift));  //A: the amplitude of our wave, set according to
                                                                          //   our experimental model of the envelope of a keyboard
                                                                          //   as a function of time.
                                                                          //w: the angular frequency of our wave. w=2*Pi*frequency
                                                                          //t: time. With i as the sample number we're writing,
                                                                          //   sample/(samples per second) = seconds
                                                                          //O: the phaseshift, or number of seconds to shift over
                                                                          //   the waveform over
    }

   phaseShift = (seconds+phaseShift)*frequency;             //The new phaseshift is the old phaseshift (as a fraction of the period)
                                                            //plus the current fraction of a period we are into the current waveform
   phaseShift -= floor(phaseShift);                         //We care about only the fractional part

   return sampleStart+duration*WHOLENOTETIME*DEFSAMPLERATE; //Return the next sample at which to write, the sample at which we've
                                                            //started to write plus the number of samples in duration
}

double Wave::PlayedDuration(double duration)
//PRE: duration >= 0
//POST: FCTVAL == The total duration to play a note in seconds, taking into account damping time
{
    return -0.1925*pow(duration, 2)+1.3754*duration; //Experimental model of duration to play note vs. duration of note in sheet form
}

double Wave::AmplitudePercent(int sampleNumber, double actualDuration)
//PRE: sampleNumber >= 0, actualDuration != FULLAMPTIME
//POST: FCTVAL == The fraction of the full amplitude of the waveform at the time corresponding to sampleNumber as a piecewise function
//      of the actualDuration, as modeled experimentally.
{
    double curTime = double(sampleNumber)/(DEFSAMPLERATE*WHOLENOTETIME);   //The time in seconds to find the amplitude of

    if (curTime <= RDAMPTIME)       //The note increases linearly from amplitude 0 to 1 in time RDAMPTIME as collected experimentally
        return curTime/RDAMPTIME;
    if (curTime <= FULLAMPTIME)     //Following the "reverse damping" period, the waveform has a period of constant amplitude
        return 1.0;

    return (actualDuration-curTime)/(actualDuration-FULLAMPTIME); //then, our model damps the note linearly to zero at time actualDuration
}

void Wave::Normalize()
//POST: The contents of wavData is scaled down linearly such that its maximum amplitude is DEFAMPLITUDE
{
    double max;   //the current maximum value of wavData

    for (int i=0; i<numChannels; i++)
    {
        max = 0;
        for (int j=0; j < wavData[i].size(); j++)  //Loop through the entire contents of wavData and find the maximum value
            if (fabs(wavData[i][j]) > max)
                max = wavData[i][j];

        for (int j=0; j < wavData[i].size(); j++)  //Then, scale the entire contents of wavData linearly down to DEFAMPLITUDE
            wavData[i][j] = DEFAMPLITUDE*(wavData[i][j]/max);
    }
}

void Wave::outputFrame(long sampleNumber, int frameWidth, int frameHeight, int fps)
//PRE:  0 <= sampleNumber < wavData[i].size() for any value of i s.t. 0 <= i < numChannels
//         width >= 256, 0 < height > 0, width and height measured in pixels, fps > 0
//POST: One frame of visualization of the waveform of this wave has been exported to the
//        working directory. This frame corresponds to a 0.005-second snapshot of the sound
//        beginning at sampleNumber. The frame is width x height pixels. The frame is in .bmp format.
//        The frame's filename is constructed by taking the base of fileName and appending the next
//        unused number among files in the working directory.
{
    Image frame(frameWidth, frameHeight, 0);                                //canvas on which to draw this frame
    Turtle* turtles = new Turtle[numChannels];                              //turtles to draw waveform for each channel
    int numSamples;                                                         //how many samples we'll draw

    for (int i=0; i < numChannels; i++)                                     //set up turtles for each channel
    {
        turtles[i] = Turtle(&frame, 0x0088FF);                              //attach turtles to frame and make them blue
        turtles[i].MoveTo(0, frameHeight/numChannels*(i+1/2.0));            //start each turtle at horizontal axis for channel
    }                                                                       //  (screen is divided into as many parts as there
                                                                            //   are channels and axis is at center)
    turtles[0].SetColor(0x00FF00);                                          //make first turtle green

    numSamples = sampleRate*256/44100;                                      //use 256 samples of data corresponding to
                                                                            //  0.005 seconds of audio

    for (int j=0; j<numChannels; j++)                                                           //draw first data point in
       turtles[j].MoveTo(0, frameHeight/numChannels*(j+1/2.0+wavData[j][sampleNumber]/2));      //  each channel

    for (int i=1; i<numSamples; i++)                                                            //draw line for each sample
        for (int j=0; j<numChannels; j++)                                                       //...in each channel
           turtles[j].LineTo(i*frameWidth/numSamples,                                           //x coord. moves us across screen
                             frameHeight/numChannels*(j+1/2.0+wavData[j][sampleNumber+i]/2));   //y coord. is based upon the height
                                                                                                //  of the sound wave at that instant

    frame.Save(Utility::FindNextUntitledDocument(fileName.substr(fileName.find_last_of("/")+1,                   //save frame using base of filename
               fileName.length()-fileName.find_last_of("/")-5).c_str(), ".bmp", Utility::NumDigits(fps*songLength))); //  before extension (and . and \0,
                                                                                                                 //  hence -5). fps*songLength gives
                                                                                                                 //  us how many frames we'll have and
                                                                                                                 //  log10 tells us how many digits to
                                                                                                                 //  allow in numbering files.

    delete[] turtles;                                                       //free memory used by turtles
    turtles = NULL;
}


void Wave::WriteHeader(const char* fileName)
//PRE:  private data fileSize, sampleRate, qLevel, samplesPerChannel, and numChannels are initialized
//POST: The header information is written to the first 44 bytes of the file referenced by filename, based
//      upon size, numChannels, rate, qLevel, and samplesPerChannel. The compression mode is written as
//      uncompressed PCM.
//      (WAV Header Reference: http://ccrma.stanford.edu/courses/422/projects/WaveFormat/)
{
    ofstream outFile(fileName, ios::binary);
    char header[44];                                                    //holds header of wav file

    // Note: All data is output in Little Endian format, hence bytes are reversed from "natural" order.
    //       Fields that occupy 2 bytes instead of 4 are hardcoded with extra zeroes in this fashion.

    Utility::DwordToLittleEndianCharacterArray(header, 0x0, 0x46464952);         //"RIFF"
    Utility::DwordToLittleEndianCharacterArray(header, 0x4, fileSize-8);         //file size (in B) - 8 (for part of header)
    Utility::DwordToLittleEndianCharacterArray(header, 0x8, 0x45564157);         //"WAVE"
    Utility::DwordToLittleEndianCharacterArray(header, 0xc, 0x20746D66);         //"fmt"
    Utility::DwordToLittleEndianCharacterArray(header, 0x10, 16);                //size of "fmt" chunk, i.e. 16 B
    header[0x14] = 1;                                                            //14 and 15 are compression code
    header[0x15] = 0;                                                            //   (1 == uncompressed PCM)
    header[0x16] = numChannels;                                                  //16 and 17 are number of channels
    header[0x17] = 0;
    Utility::DwordToLittleEndianCharacterArray(header, 0x18, sampleRate);              //sample rate, in Hz
    Utility::DwordToLittleEndianCharacterArray(header, 0x1c,                     //avg bytes/sec (sample rate * Block Align)
                                      qLevel/8*sampleRate*numChannels);
    header[0x20] = qLevel*numChannels/8;                                         //20 and 21 are Block Align
    header[0x21] = 0;
    header[0x22] = qLevel;                                                       //22 and 23 are quantization level,
    header[0x23] = 0;                                                            //  i.e. # significant bits/sample
    Utility::DwordToLittleEndianCharacterArray(header, 0x24, 0x61746164);        //"DATA"
    Utility::DwordToLittleEndianCharacterArray(header, 0x28, samplesPerChannel
                                      *numChannels*qLevel/8);                    //number of bytes in "data" subchunk

    outFile.seekp(0);                                                            //Add header to beginning of file
    outFile.write(header, 44);
    outFile.close();
}

double Wave::ChannelAmp(int amplitude)
//PRE:  amplitude is in two's complement hex
//POST: FCTVAL == signed decimal representation of amplitude between -1 and 1, i.e. the
//        fraction of the maxiumum amplitude of a sound wave
{
    double toReturn = amplitude;                // amplitude between -1 and 1

    if (toReturn > 0x7fff)                      // 2's complement stores negatives after
    {                                           //   positives and 8000 is thus the largest
        toReturn -= 0xffff;                     //   positive value in 4 hex digits. We
        toReturn /= 0x8000;                     //   subtract the largest hex value possible
    }                                           //   in 4 digits and normalize the result.
    else                                        // 2's complement value is a positive value...
        toReturn /= 0x7fff;                     //   ...so just normalize it.

    return toReturn;
}

double Wave::ChannelAmp(unsigned char dword[])
//PRE:  dword[0..1] contains two bytes of data representing the "amplitude" of a sound
//        in little endian hexidecimal
//POST: FCTVAL == signed decimal representation of amplitude between -1 and 1, i.e. the
//        fraction of the maxiumum amplitude of a sound wave
{
    return ChannelAmp(Utility::LittleEndianCharToInt(dword, 2));        // reverse endianness and
                                                                        //  use integer form of ChannelAmp
}

int Wave::InverseChannelAmp(double amp)
//PRE:  amp contains a signed decimal repsentation of an amplitude between -1 and 1, i.e. the
//         fraction of the maxiumum amplitude of a sound wave
//POST: FCTVAL == two's complement equivalent of amp
{
    int result;                                 // amplitude between 0x0000 and 0xffff

    if (amp < 0)                                // need to shift negative amp values forward so
        amp += 2.0;                             //   that they come after the greatest positive
                                                //   value in 2's complement

    result = amp/2.0*0xFFFF;                    // rescale amplitude from between 0.0 and 2.0 to
                                                //   between 0x0000 and 0xFFFF
    return result;
}

