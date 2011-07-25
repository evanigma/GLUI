
#include "Pixel.h"
#include <iostream>
#include <cstdio>

Pixel::Pixel()
{
    color = new int[3];
    byte = new unsigned char[3];
    IntSplit(0xFFFFFF, color);
}

//Init Constructor
Pixel::Pixel(int* string)
{
    color = new int[3];
    byte = new unsigned char[3];
    copy(string, color, 3);
}

Pixel::Pixel(int string)
{
    color = new int[3];
    byte = new unsigned char[3];
    IntSplit(string, color);
}

//Destructor
Pixel::~Pixel()
{
    delete[] color;
    color = NULL;

    delete[] byte;
    byte = NULL;
}

//Assignment Operator
Pixel& Pixel::operator = (int* string)
{
    //delete[] color;
    //color = new int[3];
    copy(string, color, 3);
    return *this;
}

Pixel& Pixel::operator = (int string)
{
    IntSplit(string, color);
    return *this;
}

Pixel& Pixel::operator = (const Pixel& pixel2)
{
    *this = pixel2.color;
    return *this;
}

bool Pixel::operator == (const Pixel& pixel2) const
{
    return color[0] == pixel2.color[0]
        && color[1] == pixel2.color[1]
        && color[2] == pixel2.color[2];
}
bool Pixel::operator == (int string)
{
    Pixel test(string);

    return(test[0] == color[0] && test[1] == color[1] && test[2] == color[2]);
}
bool Pixel::operator != (int string)
{
    return !(*this == string);
}
void Pixel::Invert()
{
    color[0] = 0xFF-color[0];
    color[1] = 0xFF-color[1];
    color[2] = 0xFF-color[2];
}

void Pixel::ToGreyScale()
{
    color[0] = color[1] = color[2] = GetGrey();
}
int Pixel::GetGrey()
{
    return floor((color[0]+color[1]+color[2])/3.0);
}
//Returns the ith portion of the color, where i = {0,1,2} for {Red, Green, Blue}.
int& Pixel::operator [] (int i)
{
    //throw error
    return color[i];
}

int Pixel::GetFullColor() const
{
    return (color[0] << 16)+(color[1] << 8) + color[2];
}
unsigned char* Pixel::GetBGR()
{
    byte[0] = color[2];
    byte[1] = color[1];
    byte[2] = color[0];

    return byte;
}

void Pixel::copy(const int* source, int* destination, int count)
{
    for (int i=0; i < count; i++)
    {
        destination[i] = source[i];
    }
}

void Pixel::IntSplit(int source, int* destination)
{
    destination[0] = source >> 16;
    destination[1] = (source & 0xFF00) >> 8;
    destination[2] = source & 0xFF;
}
