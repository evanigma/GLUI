//
// Written by Evan Fox


#include "Image.h"
#include "Utility.h"
#include <iostream>
#include <string.h>
using namespace std;

//Initializes data in image as a copy of another
Image::Image(const Image& copy)
{
    image = NULL;
    //^screams deep-copy assignment operator.
    *this = copy;
}

Image::Image(const string& fileIn, bool showProgress)
{
    showBar = showProgress;
    inFile.open(fileIn.data(), ios::binary); //open the bitmap in binary mode

    //initialize properties
    inFileName = fileIn;

    inFile.seekg(0x12);  //the width of the bitmap is located at offset 0x12 in the final
    width = ReadInt(4);  //... and it's a dword (4 bytes), so read 4 bytes.
    height = ReadInt(4); //same as above.

    AllocateMemory();    //Now that we know the width and height, we can allocate memory for our image

    inFile.seekg(0x36);  //the actual bitmap data starts at offset 0x36

    //Loop through entire image and read all the pixels
    for (int h = height-1; h >= 0; h--) //Bitmap pixel data is organized bottom left to top right
    {
        for (int w=0; w < width; w++)
            image[w][h] = ReadInt(3);

        inFile.seekg(int(inFile.tellg())+width%4); //accounts for padding (bitmap are organized such
                                                   //that padding 0's are added at the end of every
                                                   //row of pixels so as to make them 4-byte aligned)
    }
    inFile.close();
}

Image::Image(int w, int h, int bgColor, bool showProgress)
{
    width = w;
    height = h;
    showBar = showProgress;

    AllocateMemory();      //set space for image

    //assign every pixel to background color value.
    for (int j = 0; j < w; j++)
        for (int k=0; k < h; k++)
            image[j][k] = bgColor;
}

Image::~Image()
{
    //get rid of data

    if (showBar)
        cout << "Cleaning Up...\n";

    for (int i=0; i<width; i++)
    {
        if (showBar)
            Utility::Bar(cout, i, width);

        delete[] image[i];
        image[i] = NULL;
    }

    if (showBar)
    {
        Utility::Bar(cout, width, width);
        cout << endl;
    }
    delete[] image;
    image = NULL;
}

void Image::AllocateMemory()
{
    image = new Pixel*[width]; //allocate memory for arrays for all columns

    if (showBar)
        cout << "Allocating Memory...\n";

    //loop through and allocate memory for all rows
    for (int i=0; i<width; i++)
    {
        if (showBar)
            Utility::Bar(cout, i, width);
        image[i] = new Pixel[height];
    }

    if (showBar)
    {
        Utility::Bar(cout, width, width);
        cout << endl;
    }
}

void Image::Open(const string& fileIn)
{
    *this = Image(fileIn); //overwrite current data with that of a freshly made image from file.
}

Image& Image::operator = (const Image& image2)
{
    //Get rid of current image data
    if (image != NULL)
    {
        delete[] image;
        image = NULL;
    }

    //copy properties
    inFileName = image2.inFileName;
    outFileName = image2.outFileName;
    width = image2.width;
    height = image2.height;
    showBar = image2.showBar;

    AllocateMemory();

    //copy data
    for (int h = 0; h < height; h++)
        for (int w=0; w < width; w++)
            image[w][h] = image2.image[w][h];

    return *this;
}

//index operator
Pixel*& Image::operator [](int w)
{
    return image[w];
}

int Image::Width()
{
    return width;
}

int Image::Height()
{
    return height;
}

void Image::Flip(bool isHorizontalFlip)
{
    Image temp(*this); //copy current image data

    //loop through entir image
    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
        {
            if (isHorizontalFlip) //if we're flipping horizontal
                image[w][h] = temp.image[width-1-w][h]; //copy on opposite side horizontally
            else
                image[w][h] = temp.image[w][height-1-h]; //else, do so vertically.
        }
}

void Image::Resize(int newWidth, int newHeight)
{
    if (newWidth*newHeight == 0)
    {
        //throw error;
    }

    //Crate a new image of width newWidth and height newHeight
    Image newImage(newWidth, newHeight);

    int newX, newY;

    for (int i=0;i<newHeight;i++)
        for (int j=0;j<newWidth;j++)
        {
            newX = floor(j*width/double(newWidth));    //find the closest x-coordinate on the old image
                                                       //to that of the new image.
            newY = floor(i*height/double(newHeight));  //find the closest y-coordinate on the old image
                                                       //to that of the new image.
            newImage.image[j][i] = image[newX][newY];  //copy over data from old to new.
        }

    *this = newImage; //overwrite old with new.
}

void Image::Scale(double factor)
{
    //Why redefine something when you have pretty much the same exact method as above?
    Resize(floor(width*factor), floor(height*factor));
}

Image Image::Histogram()
{
    Image hist(256, 100, 0xFFFFFF); //create a new image to hold the histogram
                                                     //PS - I decided against creating a whole bunch
                                                     //of arguments for this method because that's what
                                                     //my Resize() and SetOutFile() methods are for...
    int histogram[256];             //array to hold the actual numerical data.
    int max = 0;
    int plotted;                    //how many pixels have been plotted so far?

    for (int i=0; i<256; i++)       //set all histogram data to 0 (because there may be some tonal
        histogram[i] = 0;           //that are underrepresented.

    //loop through entire image and increment values in the histogram for each pixel's tonal value.
    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
            histogram[image[w][h].GetGrey()]++;

    //find the max number pixels sharing the same tonal value.
    for (int i=0; i<256; i++)
        if (histogram[i] > max)
            max = histogram[i];

    //go through each column in the image
    for (int j=0; j<256; j++)
    {
        plotted = 0;

        //and plot the number of pixels as a percentage of the maximum
        //...like a "histogram"
        while (plotted < floor(histogram[j]*100.0/max))
        {
            hist[j][100-1-plotted] = 0;
            plotted++;
        }
    }

    return hist;
}

void Image::Blur(int r/*adius*/)
{
    //Note: Still have to handle outside border!!

    Image temp(*this);   //used to retain original data when blurring

    int sum;

    //Loop through all pixels which actually have a radius large enough
    //(i.e. - everything but the outside border r pixels wide)
    for (int h=r; h<height-1-r; h++)
        for (int w=r; w<width-1-r; w++)

            //loop once for each r, g, b component
            for(int i=0; i<3; i++)
            {
                //this is a simple "averaging" blur, which just set each pixel to the average value
                //of that of it's neighbors of radius r.
                sum = 0;

                //loop through surrounding pixels.
                for (int j=-r; j<=r; j++)
                    for (int k=-r; k<=r; k++)
                        sum += temp[w+j][h+k][i];  //and keep a running sum.

                sum /= pow(1.0+2*r, 2);            //divide by the number of pixels
                                                   //(it is an average, after all...)
                image[w][h][i] = sum;
            }
    //Yikes! 5 nested for-loops! :).
}

void Image::Invert()
{
    //Loop through all pixels and invert them.
    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
            image[w][h].Invert();
}

void Image::ToGreyScale()
{
    //Loop through all pixels and set them to greyscale.
    for (int h = 0; h<height; h++)
        for (int w=0; w<width; w++)
            image[w][h].ToGreyScale();
}

Image Image::GetRedChannel()
{
    Image channel(*this);

    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
            channel[w][h][1] = channel[w][h][2] = 0;

    return channel;
}

Image Image::GetGreenChannel()
{
    Image channel(*this);

    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
            channel[w][h][0] = channel[w][h][2] = 0;

    return channel;
}

Image Image::GetBlueChannel()
{
    Image channel(*this);

    for (int h=0; h<height; h++)
        for (int w=0; w<width; w++)
            channel[w][h][0] = channel[w][h][1] = 0;

    return channel;
}

void Image::Save(string fileName)
{
    if (fileName.empty())
    {
        if (outFileName.empty())
            fileName = outFileName = Utility::FindNextUntitledDocument("Untitled", ".bmp");
        else
            fileName = outFileName;
    }

    outFile.open(fileName.data(), ios::binary); //open the file for writing
    outFile.seekp(0x36);                           //bitmap data starts at offset 0x36.

    if (showBar)
        cout << "Saving Image...\n";

    for (int h = height-1; h >= 0; h--)            //again, data is written bottom left
                                                   //to top right.
    {
        if (showBar)
            Utility::Bar(cout, height-h, height);
        for (int w=0; w < width; w++)
            outFile.write(reinterpret_cast<char*>(image[w][h].GetBGR()), 3); //Write B, G, R
                                                                             //(little-endian structure)
        outFile.seekp(int(outFile.tellp())+width%4); //for padding, as explained above in Image::Image
    }

    if (showBar)
    {
        Utility::Bar(cout, height, height);
        cout << endl;
    }

    WriteHeader();
    outFile.close();
}

void Image::WriteHeader()
{
    char header[55];                            //Holder for all header information
    long dataSize = (width*3+(width%4))*height; //Bitmap Data Size = (width of each row)*(height)
                                                    //Again, (width of row)=(pixels in row+padding)
    dataSize += 16-(54+dataSize)%16;            //Add padding to EOF to make final size 16-byte aligned
                                                    //(i.e. size%16 = 0)
    long size = 54+dataSize;                    //Overall File Size (header is 54 bytes long)

    for (int k=0; k < 55; k++)                  //Initialize all header bytes to 0;
    {
        header[k] = 0;
    }
    header[0] = Utility::HexToD("42");                 //These two are the identifier for all bitmaps, "B", "M."
    header[1] = Utility::HexToD("4D");

    Utility::DwordToLittleEndianCharacterArray(header,2,size);

    header[10] = Utility::HexToD("36");                //Bitmap data offset value (54)
    header[14] = Utility::HexToD("28");                //Bitmap header size value (40) (notice 40 < 53)

    Utility::DwordToLittleEndianCharacterArray(header,0x12,width);

    Utility::DwordToLittleEndianCharacterArray(header,0x16,height);

    header[26] = Utility::HexToD("1");                 //Number of planes (1)
    header[28] = Utility::HexToD("18");                //Bits per pixel (24)

    Utility::DwordToLittleEndianCharacterArray(header,34,dataSize);

    Utility::DwordToLittleEndianCharacterArray(header,38,2834); //Horizontal Resolution
    Utility::DwordToLittleEndianCharacterArray(header,42,2834); //Vertical Resolution
                                                              //(2834 pixels/meter=72 pixels/in)

    outFile.seekp(0);                           //Set writing offset to 0
    outFile.write(header,52);                   //Write header to file*/
}

void Image::SetOutFile(const string& fileOut)
{
    outFileName = fileOut;
}

int Image::ReadInt(int numBytes)
{
    unsigned char* bytes = new unsigned char[numBytes]; //holder for string of bytes from ifstream::read
    inFile.read(reinterpret_cast<char*>(bytes), numBytes); //read in numBytes of data from the file
    return Utility::LittleEndianCharToInt(bytes, numBytes);         //make it human-readable.
}
