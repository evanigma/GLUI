// Turtle.h
// Written by Evan Fox

#pragma once
#include "Image.h"
#include <math.h>
using namespace std;

class Turtle
{
public:
    Turtle();
    Turtle(Image* toAssign, int toColor = 0, int toWidth = 1);

    Turtle& operator = (const Turtle& toCopy);

    //Moves "turtle's" position, without drawing.
    void MoveTo(int x, int y);
    //Moves "turtle's" position, with drawing.
    void LineTo(int x, int y);

    void SetColor(int toColor);

    //Fill all pixels bordering (x,y) of targetColor with class member color.
    void Fill(int x, int y, int targetColor);
    void Fill(int x, int y, const Pixel& targetColor);

private:
    Image* pic;
    Pixel color;
    int width;
    int posX, posY;

    //utility function
    static void Swap(int& a, int&b);
};
