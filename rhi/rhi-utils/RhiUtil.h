#ifndef __RHIUTIL_H__
#define __RHIUTIL_H__

#include <windows.h>
#include <stdio.h>
#include <string>

// Macros to help with bitmap padding
#define BITMAP_SIZE(width, height) ((((width) + 3) & ~3) * (height))
#define BITMAP_INDEX(x, y, width) (((y) * (((width) + 3) & ~3)) + (x))

// Describes the structure of a 24-bpp Bitmap pixel
struct BitmapPixel
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

// Describes the structure of a RGB pixel
struct RGBPixel
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

// Describes the structure of a ARGB pixel
struct ARGBPixel
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
};

bool SaveBitmap(const char *fileName, BYTE *data, int width, int height);

bool SaveRGB(const char *fileName, BYTE *data, int width, int height);

bool SaveBGR(const char *fileName, BYTE *data, int width, int height);

bool SaveRGBPlanar(const char *fileName, BYTE *data, int width, int height);

bool SaveARGB(const char *fileName, BYTE *data, int width, int height);

bool SaveYUV(const char *fileName, BYTE *data, int width, int height);

#endif