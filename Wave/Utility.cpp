#include "Utility.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <math.h>

#ifdef _WIN32
#define isWindows true
#else
#define isWindows false
#endif

using namespace std;

int Utility::Power(int a, int b)
{
    int toReturn = 1;
    for (int i=0; i < b; i++)
        toReturn *= a;

    return toReturn;
}

int Utility::LittleEndianCharToInt(unsigned char dword[], int length)
{
    int number = 0;
    for (int i=0; i<length; i++)
    {
        number += (dword[i] << i*8); //Little endian byte orders the least significant bytes first,
                                     // "<< i*8" bit-shifts the more significant bytes over i bytes
                                     // so as to retain their significance.
                                           //i.e. if data[0] = 0xFE, data[1] = 0x03, data[2] = 0xA4,
                                           // we want 0xFE + 0x0300 + 0xA40000 = 0xA403FE
    }
    return number;
}

int Utility::HexToD(const string & hex)
//PRE: Each character in string is a hex character 0...F
//POST: Returns the decimal representation of the composite string of hex characters
{
    int sum = 0;        //Running sum of each character
    for (unsigned int i=0; i < hex.length(); i++)   //loop through each character
    {
        //I need to left-bitshift each character according to its position in the string
        //(i.e. 0x123abc = 0x100000 + 0x20000 + 0x3000 + 0xa00 + 0xb0 + 0xc)
        //1 hex character = 4 bits
        sum += (CharToD(hex[i]) << 4*(hex.length()-i-1));
    }
    return sum;
}

int Utility::CharToD(const char & hexChar)
//PRE: hexChar is a hex character 0..F
//POST: Returns the integral representation of hexChar.
//      If precondition is not satisfied, returns -1.
{
    if (hexChar >= '0' && hexChar <= '9')
        return hexChar - '0';

    if (hexChar >= 'A' && hexChar <= 'F')
        return hexChar - 'A' + 10;

    if (hexChar >= 'a' && hexChar <= 'f')
        return hexChar - 'a' + 10;

    return -1;
}

void Utility::DwordToLittleEndianCharacterArray(char holder[], int start, long dword)
//PRE: Assigned(start, dword)
//POST: Writes data of size 4 bytes in little endian format to holder starting at position start.
//      In little endian format, groups of two bytes are written in order of least significant
//      to most significant. i.e. a hex number 0xABCDEFGH will be written GH, EF, CD, AB.
{
    holder[start] = (dword & 0xFF);                  //Get 16^1 and 16^0 parts of size
    holder[start+1] = ((dword & 0xFF00) >> 8);       //Get 16^3 and 16^2 parts of size, shift down to two bytes
    holder[start+2] = ((dword & 0xFF0000) >> 16);    //Get 16^5 and 16^4 parts of size, shift down to two bytes
    holder[start+3] = ((dword & 0xFF000000) >> 24);  //Get 16^7 and 16^6 parts of size, shift down to two bytes
}

string Utility::FindNextUntitledDocument(string base, string ext, int spaces)
//PRE:  base, ext, initialized. spaces >= 0
//POST: Returns the next available, non-existent document in the current directory of the format
//      [base]([digit]*spaces)[ext]. Example: With the files "Untitled001.bmp" and "Untitled003.bmp"
//      in the current directory, a call to FNUD("Untitled", ".bmp", 3) will return "Untitled002.bmp"
{
    char fileTest[25];
    string topFile = "";
    int systemHolder;

    string temp("");                  //holder for string manipulations

    if (isWindows)
    {
        temp = "dir /A-D /B /O-N | findstr " + base + " | findstr " + ext + " > Files.txt";
    }
    else
    {
        temp = "ls -r | grep " + base + " | grep " + ext +/* " | head -n 10*/" > Files.txt";
    }

    systemHolder = system(temp.c_str());                       //Make list of files in current directory. !Requires Unix.

    ifstream inFile("Files.txt");                           //object to read in files of the current directory

    while (inFile >> temp)
    {
        if (temp.length() == base.length()+spaces+ext.length())
        {
            topFile = temp;
            break;
        }
    }

    temp = "";                                             //Delete contents of string holder
    temp = temp + "%0" + char(spaces+int('0')) + "d";     //Form second argument of sprintf call to handle spaces

    strcpy(fileTest, base.c_str());
    if (topFile != "")
        sprintf(strchr(fileTest,0), temp.c_str(), atoi(topFile.substr(base.length(), spaces).c_str())+1);
    else
        sprintf(strchr(fileTest,0), temp.c_str(), 0);

    strcat(fileTest, ext.c_str());

    inFile.close();

    if (isWindows)
        systemHolder = system("del Files.txt");
    else
        systemHolder = system("rm Files.txt");

    return fileTest;
}

void Utility::Bar(ostream &os, double numer, double denom, int length)
{
    int decimalPlaces = NumDigits(denom);
    int maxDec = 4;
    //int power = decimalPlaces-maxDec;

    //cout << "decimal places: " << decimalPlaces << ", " << "10^" << power << " = " << Power(3,4) << endl;

    if (decimalPlaces <= maxDec || int(numer) % Power(10, decimalPlaces-maxDec) == 0 || numer == denom)
    {
        os.setf(ios::fixed);
        os << "\r[";
        for (int i=0; i<length; i++)
            os << (double(i)/length <= numer/denom ? "=" : ".");

        os << "] " << setw(decimalPlaces) << setprecision(0) << floor(numer) << "/" << setprecision(0) << floor(denom);
        os.flush();
        os.unsetf(ios::fixed);
    }
}

int Utility::NumDigits(int number)
{
    return (log10(number) == ceil(log10(number)) ? //log10 of number gives number of digits, except when number is a power of 10,
            log10(number)+1 :                      //in which case we must add one
            ceil(log10(number)));

}

double Utility::TimeInSecondsMMSSXXX(string time)
//PRE:  time is in the form mm:ss[.xxx]
//POST: FTCVAL == time converted to seconds
{
    double result;                              //time, converted to seconds
    int msColonLoc;                             //location of colon between hh and mm part of time
    int periodLoc;                              //location of period after seconds part of time
    int fractionalPartLength;                   //how many characters fraction of seconds takes up
    int fractionalPartInt;                      //what's after decimal
    double fractionalPart;                      //fraction of seconds, converted to seconds

    msColonLoc = time.find(":");                //find the colon dividing minutes and seconds
    periodLoc = time.find(".");                 //find the period dividing seconds and fraction of seconds
    if(periodLoc != -1)                         //time has fraction of seconds part
    {
        fractionalPartLength = time.length() - periodLoc - 1;
        fractionalPartInt = atoi(time.substr(periodLoc+1, fractionalPartLength).c_str());    //get fractional part as integer

        fractionalPart = fractionalPartInt
                       / pow(10, fractionalPartLength);                                      //convert to decimal
    }
    else                                        //time has no fraction of seconds part
        fractionalPart = 0;

    result = 60*atoi(time.substr(0, msColonLoc).c_str())            //minutes part, 60 seconds/minute
           + atoi(time.substr(msColonLoc+1, 2).c_str())             //seconds part
           + fractionalPart;                                        //fraction of seconds part

    return result;
}


double Utility::TimeInSeconds(string time)
//PRE:  time is in the form [hh:]mm:ss[.xxx]
//POST: FTCVAL == time converted to seconds
{
    int hmColonLoc = time.find(":");                                //location of colon dividing hh and mm
    int msColonLoc = time.substr(hmColonLoc+1).find(":");           //location of colon dividing mm and ss,
                                                                    //  provided there are two colons

    if(msColonLoc == -1)                                            //no hours part
        return TimeInSecondsMMSSXXX(time);
    else                                                            //time has hours part
        return 3600*atoi(time.substr(0,hmColonLoc).c_str())         //hours part, 3600 seconds/hour
              +TimeInSecondsMMSSXXX(time.substr(hmColonLoc+1));     //get seconds from rest
}

string Utility::StringFromInt(int x)
// PRE:  x is assigned
// POST: FCTVAL == x coverted to string form
{
    int length = log10(x) == ceil(log10(x))       // how many digits in x: log10(x) rounded up gives # digits
               ? log10(x)+1                       // except when x is a power of 10, in which case we must add 1
               : ceil(log10(x));
    string result("");                            // string version of x
    int xTemp = x;                                // copy of x from which digits will be removed
    int curDigit;                                 // holds each digit as it's calculated

    for(int i = 0; i < length; i++)               // build string digit-by-digit
    {
        curDigit = int(floor(xTemp / pow(10, length-i-1)));         // compute current digit by shifting
                                                                    //   the number and extracting whole part

        xTemp -= curDigit*pow(10, length-i-1);                      // remove leftmost digit by subtracting
                                                                    //   a multiple of it and correct power of 10

        result = result + char(curDigit+48);                        // shift by 48 to get to ASCII equivalent of digit
                                                                    //   and append digit to string

    }

    return result;
}

int Utility::NumDecimals(double x)
// PRE:  0 < x < 1
// POST: FCTVAL == number of digits in x
{
    // Would like something more mathematically elegant, analogous to ceil(log10(x)) that works for whole numbers
    // but this'll do...
    int result = 0;                         // number of decimals
    double xTemp = x*10;                    // copy of x in which to shift decimal place for counting

    while(xTemp - floor(xTemp) > 0.00001)                // count a digit each time there's a fraction remaining
    {
        result++;
        xTemp = (xTemp-floor(xTemp))*10;    // shift decimal again
    }

    return result;
}

string Utility::StringFromDouble(double x)
// PRE:  x contains a decimal
// POST: FCTVAL == x converted to string form
{
    int wholeNumberPart;                           // whole number part of x
    int fractionalPart;                            // fractional part of x shifted to whole number, e.g. "1" for "2.1"


    wholeNumberPart = int(floor(x));                                // extract whole number
    fractionalPart = (x - wholeNumberPart)                          // get fractional part
                   * pow(10, NumDecimals(x - wholeNumberPart));     // multiply fraction by a power of 10 to shift it

    //cout << x << " " << wholeNumberPart << "  " << fractionalPart << "     ";

    return StringFromInt(wholeNumberPart)                            // convert both parts to char arrays, then to strings
         + "."                                                       // and glue the number back together
         + StringFromInt(fractionalPart);
}

