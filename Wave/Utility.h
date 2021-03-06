#pragma once
#include <string>
#include <ostream>
#include <complex>
#include <vector>
using namespace std;

class Utility
{
public:
    //Utility methods (used primarily for handling bitmap data)
    static int Power(int a, int b);
    static int LittleEndianCharToInt(unsigned char dword[], int length);
    static int HexToD(const string & hex);
    static void DwordToLittleEndianCharacterArray(char holder[], int start, long dword);
    static int CharToD(const char & hexChar);
    static string FindNextUntitledDocument(string base, string ext, int spaces = 3);
    static void Bar(ostream &os, double numer, double denom, int length=50);
    static int NumDigits(int number);
    static double TimeInSecondsMMSSXXX(string time);
    static double TimeInSeconds(string time);
    static string StringFromInt(int x);
    static int NumDecimals(double x);
    static string StringFromDouble(double x);
    static vector<complex<double> > FFT(const vector<complex<double> >& input);
};
