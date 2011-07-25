// Pixel.h
// Written by Evan Fox

#pragma once
#include <math.h>

class Pixel
{
public:
    //Default Constructor
    Pixel();

    //Init Constructors
    Pixel(int* string);
    Pixel(int string);

    //Destructor
    ~Pixel();

    //Assignment Operators
    Pixel& operator = (int* string);
    Pixel& operator = (int string);
    Pixel& operator = (const Pixel& pixel2);

    //Comparison Operators
    bool operator == (int string);
    bool operator == (const Pixel& pixel2) const;
    bool operator != (int string);

    //Manipulation Methods
    void Invert();
    void ToGreyScale();

    //Returns Grey Scale equivalent
    int GetGrey();

    //Returns the ith portion of the color, where i = {0,1,2} for {Blue, Green, Red}.
    int& operator [] (int i);

    //Constructs original hexidecimal color value from respective R G B parts
    int GetFullColor() const;

    unsigned char* GetBGR();

private:
    // Utility Methods
    static void copy(const int* source, int* destination, int count);
    static void IntSplit(int source, int* destination);

    int* color;
    unsigned char* byte;
};
