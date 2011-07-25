//Image.h
//Written by Evan Fox

#pragma once
#include <fstream>
#include <string>
#include <math.h>
#include "Pixel.h"
using namespace std;

class Image
{
public:
    //init constructors
    Image(const Image& copy);
    Image(const string& fileIn, bool showProgress = false);
    Image(int w, int h, int bgColor = 0xFFFFFF, bool showProgress = false);

    //destructor
    ~Image();

    //Opens new image file
    void Open(const string& fileIn);

    //Deep copy assignment operator
    Image& operator = (const Image& image2);

    //index operator
    Pixel*& operator [](int w);

    //retrieves parameters
    int Width();
    int Height();

    //Image altering methods
    void Flip(bool isHorizontalFlip = true);
    void Resize(int newWidth, int newHeight);
    void Scale(double factor);
    void Blur(int r/*adius*/=1);
    void Invert();
    void ToGreyScale();

    //Get Channels
    Image GetRedChannel();
    Image GetGreenChannel();
    Image GetBlueChannel();

    //provides tonal analysis of image
    Image Histogram();

    // Methods used to write image out to bitmap.
    void Save(string fileName = "");
    void WriteHeader();
    void SetOutFile(const string& fileOut);

    //Ideas...
    // '+'?? overwrite second image pixels on first image pixels. (at int x=0, int y=0)
    // '+' (int)
    // difference operator?
    //'*' (*=?) takes average of first and second images.
    //Transparency ~= !def Pixels.
    //void Rotate(bool is90, bool isCW);

private:
    string inFileName;
    string outFileName;
    ifstream inFile;
    ofstream outFile;
    bool showBar;
    int width;
    int height;
    Pixel** image;       //Two-dimensional array containing actual image data.

    //private method for dynamically allocating memory for image
    void AllocateMemory();

    //Reads a little endian string of numBytes bytes from the ifstream at its current position
    // and reverses it into human-usable form.
    int ReadInt(int numBytes);
};
