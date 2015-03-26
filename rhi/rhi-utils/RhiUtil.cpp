#include "RhiUtil.h"

bool SaveBitmap(const char *fileName, BYTE *data, int width, int height)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    FILE *outputFile;
    bool bRet = false;

    if (data)
    {
        if(outputFile = fopen(fileName, "wb"))
        {
            width = (width + 3) & (~3);
            int size = width * height * 3; // 24 bits per pixel

            fileHeader.bfType = 0x4D42;
            fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
            fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

            infoHeader.biSize = sizeof(BITMAPINFOHEADER);
            infoHeader.biWidth = width;
            infoHeader.biHeight = height;
            infoHeader.biPlanes = 1;
            infoHeader.biBitCount = 24;
            infoHeader.biCompression = BI_RGB;
            infoHeader.biSizeImage = BITMAP_SIZE(width, height);
            infoHeader.biXPelsPerMeter = 0;
            infoHeader.biYPelsPerMeter = 0;
            infoHeader.biClrUsed = 0;
            infoHeader.biClrImportant = 0;

            fwrite((unsigned char *)&fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFile);
            fwrite((unsigned char *)&infoHeader, 1, sizeof(BITMAPINFOHEADER), outputFile);
            fwrite(data, 1, size, outputFile);

            bRet = true;
            fclose(outputFile);
        }
    }

    return bRet;
}

bool SaveRGB(const char *fileName, BYTE *data, int width, int height)
{
    bool result = false;

    RGBPixel *input = (RGBPixel *)data;
    BitmapPixel *output = new BitmapPixel[BITMAP_SIZE(width, height)];

    // Pad bytes need to be set to zero, it's easier to just set the entire chunk of memory
    memset(output, 0, BITMAP_SIZE(width, height) * sizeof(BitmapPixel));

    for(int row = 0; row < height; ++row)
    {
        for(int col = 0; col < width; ++col)
        {
            // In a bitmap (0,0) is at the bottom left, in the frame buffer it is the top left.
            int outputIdx = BITMAP_INDEX(col, row, width);
            int inputIdx = ((height - row - 1) * width) + col;

            output[outputIdx].red = input[inputIdx].red;
            output[outputIdx].green = input[inputIdx].green;
            output[outputIdx].blue = input[inputIdx].blue;
        }
    }

    result = SaveBitmap(fileName, (BYTE *)output, width, height);

    delete [] output;

    return result;
}

bool SaveBGR(const char *fileName, BYTE *data, int width, int height)
{
    bool result = false;

    if (!data)
        return false;
    RGBPixel *input = (RGBPixel *)data;
    BitmapPixel *output = new BitmapPixel[BITMAP_SIZE(width, height)];

    // Pad bytes need to be set to zero, it's easier to just set the entire chunk of memory
    memset(output, 0, BITMAP_SIZE(width, height) * sizeof(BitmapPixel));

    for(int row = 0; row < height; ++row)
    {
        for(int col = 0; col < width; ++col)
        {
            // In a bitmap (0,0) is at the bottom left, in the frame buffer it is the top left.
            int outputIdx = BITMAP_INDEX(col, row, width);
            int inputIdx = ((height - row - 1) * width) + col;

            output[outputIdx].red = input[inputIdx].blue;
            output[outputIdx].green = input[inputIdx].green;
            output[outputIdx].blue = input[inputIdx].red;
        }
    }

    result = SaveBitmap(fileName, (BYTE *)output, width, height);

    delete [] output;

    return result;
}

bool SaveRGBPlanar(const char *fileName, BYTE *data, int width, int height)
{
    if (!data)
        return false;

    const char *nameExt[] = {"red", "green", "blue"};
    BitmapPixel *output = new BitmapPixel[BITMAP_SIZE(width, height)];
    memset(output, 0, BITMAP_SIZE(width, height) * sizeof(BitmapPixel));

    for(int color = 0; color < 3; ++color)
    {
        for(int row = 0; row < height; ++row)
        {
            for(int col = 0; col < width; ++col)
            {   
                int outputIdx = BITMAP_INDEX(col, row, width);
                int inputIdx = ((height - row - 1) * width) + col;

                output[outputIdx].blue = 0;
                output[outputIdx].green = 0;
                output[outputIdx].red = 0;

                switch(color)
                {
                case 0:
                    output[outputIdx].red = data[inputIdx];
                    break;

                case 1:
                    output[outputIdx].green = data[inputIdx + (width * height)];
                    break;

                case 2:
                    output[outputIdx].blue = data[inputIdx + 2 * (width * height)];
                    break;

                default:
                    break;
                }
            }
        }

        std::string outputFile = fileName;
        size_t find = outputFile.find_last_of(".");

        outputFile.insert(find, "-");
        outputFile.insert(find+1, nameExt[color]);

        if(!SaveBitmap(outputFile.c_str(), (BYTE *)output, width, height))
        {
            delete [] output;
            return false;
        }
    }

    delete [] output;

    return true;
}

bool SaveARGB(const char *fileName, BYTE *data, int width, int height)
{
    bool result = false;
    if (!data)
        return result;

    ARGBPixel *input = (ARGBPixel *)data;
    BitmapPixel *output = new BitmapPixel[BITMAP_SIZE(width, height)];
    memset(output, 0, BITMAP_SIZE(width, height) * sizeof(BitmapPixel));

    for(int row = 0; row < height; ++row)
    {
        for(int col = 0; col < width; ++col)
        {
            int outputIdx = BITMAP_INDEX(col, row, width);
            int inputIdx = ((height - row - 1) * width) + col;

            output[outputIdx].red = input[inputIdx].red;
            output[outputIdx].green = input[inputIdx].green;
            output[outputIdx].blue = input[inputIdx].blue;
        }
    }

    result = SaveBitmap(fileName, (BYTE *)output, width, height);

    delete [] output;

    return result;
}

bool SaveYUV(const char *fileName, BYTE *data, int width, int height)
{
    if (!data)
        return false;

    int hWidth = width >> 1;
    int hHeight = height >> 1;
    size_t find = -1;
    std::string outputFile;

    BitmapPixel *luma = new BitmapPixel[BITMAP_SIZE(width, height)];
    BitmapPixel *chrom = new BitmapPixel[BITMAP_SIZE(width, height)];

    memset(luma, 0, BITMAP_SIZE(width, height) * sizeof(BitmapPixel));
    memset(chrom, 0, BITMAP_SIZE(hWidth, hHeight) * sizeof(BitmapPixel));

    for(int row = 0; row < height; ++row)
    {
        for(int col = 0; col < width; ++col)
        {
            int outputIdx = BITMAP_INDEX(col, row, width);
            int inputIdx = ((height - row - 1) * width) + col;

            luma[outputIdx].red = data[inputIdx];
            luma[outputIdx].green = data[inputIdx];
            luma[outputIdx].blue = data[inputIdx];
        }
    }

    data += width * height;

    outputFile = fileName;
    find = outputFile.find_last_of(".");

    outputFile.insert(find, "-");
    outputFile.insert(find+1, "y");

    if(!SaveBitmap(outputFile.c_str(), (BYTE *)luma, width, height))
    {
        delete [] luma;
        delete [] chrom;
        return false;
    }

    for(int row = 0; row < hHeight; ++row)
    {
        for(int col = 0; col < hWidth; ++col)
        {
            int outputIdx = BITMAP_INDEX(col, row, hWidth);
            int inputIdx = ((hHeight - row - 1) * hWidth) + col;

            chrom[outputIdx].red = data[inputIdx];
            chrom[outputIdx].green = 255 - data[inputIdx];
            chrom[outputIdx].blue = 0;
        }
    }

    data += hWidth * hHeight;

    outputFile = fileName;
    find = outputFile.find_last_of(".");

    outputFile.insert(find, "-");
    outputFile.insert(find+1, "u");

    if(!SaveBitmap(outputFile.c_str(), (BYTE *)chrom, hWidth, hHeight))
    {
        delete [] luma;
        delete [] chrom;
        return false;
    }

    for(int row = 0; row < hHeight; ++row)
    {
        for(int col = 0; col < hWidth; ++col)
        {
            int outputIdx = BITMAP_INDEX(col, row, hWidth);
            int inputIdx = ((hHeight - row - 1) * hWidth) + col;

            chrom[outputIdx].red = 0;
            chrom[outputIdx].green = 255 - data[inputIdx];
            chrom[outputIdx].blue = data[inputIdx];
        }
    }

    data += hWidth * hHeight;

    outputFile = fileName;
    find = outputFile.find_last_of(".");

    outputFile.insert(find, "-");
    outputFile.insert(find+1, "v");

    if(!SaveBitmap(outputFile.c_str(), (BYTE *)chrom, hWidth, hHeight))
    {
        delete [] luma;
        delete [] chrom;
        return false;
    }

    delete [] luma;
    delete [] chrom;
    return true;
}
