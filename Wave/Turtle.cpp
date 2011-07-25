#include "Turtle.h"
#include <math.h>
using namespace std;

Turtle::Turtle()
{
}

Turtle::Turtle(Image* toAssign, int toColor, int toWidth)
{
    pic = toAssign;
    color = toColor;
    width = toWidth;
    posX = posY = 0;
}

Turtle& Turtle::operator = (const Turtle& toCopy)
{
    pic = toCopy.pic;
    color = toCopy.color;
    width = toCopy.width;
    posX = toCopy.posX;
    posY = toCopy.posY;

    return *this;
}

void Turtle::MoveTo(int x, int y)
{
    posX = x;
    posY = y;
}
void Turtle::LineTo(int x, int y)
//An implementation of "Bresenham's line algorithm."
{
    int slope = 1;
    int dx, dy;
    int eInc, neInc;
    int d, y2;

    int tempX = x;
    int tempY = y;

    bool isSteep = fabs(posY-double(y)) >= fabs(posX-double(x));


    //Bresenham's method works only for line's whose slope's are less than or equal to one
    //(to see why, consider the for loop below. it moves one x-coordinate at a time.
    //if the line is "steep," it would require two vertical pixels for one x-coordinate.)
    //Two resolve this, we simply reflect the line across the origin (swapping x and y
    //coordinates), and thusly  the new slope = 1/old slope <= 1.
    if (isSteep)
    {
        Swap(posX, posY);
        Swap(x, y);
    }

    //Again, Bresenham's method works going left to right. If the new coordinate is to the left,
    //we reverse the coordinates and go not right to left but left to right.
    if (posX > x)
    {
        Swap(posX, x);
        Swap(posY, y);
    }

    //Calculate the difference between coordinates...
    dx = x - posX;
    dy = y - posY;

    // if the slope is negative, adjust the y-increment value...
    if (dy < 0)
    {
        slope = -1;
        dy = -dy;
    }

    // Bresenham's constants
    eInc = 2 * dy;
    neInc = 2 * dy - 2 * dx;
    d = 2 * dy - dx;
    y2 = posY;


    for (int i = posX; i <= x; i++){
        if (isSteep) //As above, we swapped x and y coordinates if the line is steep,
                     //so we must swap them back.
        {
            if (y2 < pic->Width() && i < pic->Height()) //Don't write to pixel if
                (*pic)[y2][i] = color;                  //we're outside the border
        }
        else         //...else, everything's fine.
        {
            if (i < pic->Width() && y2 < pic->Height()) //Don't write to pixel if
                (*pic)[i][y2] = color;                  //we're outside the border
        }

        if (d <= 0)                                     //adjust increments
            d += eInc;
        else
        {
            d += neInc;
            y2 += slope;
        }
    }
    posX = tempX;
    posY = tempY;
}

void Turtle::SetColor(int toColor)
{
    color = toColor;
}

void Turtle::Fill(int x, int y, int targetColor)
{
    //If we're past the images borders, quit.
    if (x < 0 || x>=pic->Width() || y<0 || y>=pic->Height())
        return;

    //If we already colored this spot, or we're on the wrong color, quit.
    if ((*pic)[x][y] !=  targetColor || (*pic)[x][y] == color)
        return;

    (*pic)[x][y] = color;

    //Fill the the pixel left
    Fill(x-1, y, targetColor);
    //Fill the pixel right
    Fill(x+1, y, targetColor);
    //Fill the pixel up
    Fill(x, y+1, targetColor);
    //Fill the pixel down
    Fill(x, y-1, targetColor);

    return;
}
void Turtle::Fill(int x, int y, const Pixel& targetColor)
{
    Fill(x,y, targetColor.GetFullColor());
}


void Turtle::Swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}
