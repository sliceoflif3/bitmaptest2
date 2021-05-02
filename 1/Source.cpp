/*
#ifndef _MYFUNCTION_H
#define _MYFUNCTION_H
#endif
*/
#include<iostream>
#include<Windows.h>
#include<bits/stdc++.h>
using namespace std;
#pragma pack(1)
struct BmpSignature
{
    unsigned char data[2];
};
#pragma pack(1)


struct BmpHeader
{
    BmpSignature signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
};
struct BmpDib
{
    uint32_t dibSize;
    int32_t imageWidth;
    int32_t imageHeight;
    uint16_t colorPlaneCount;
    uint16_t pixelSize;
    uint32_t compressMethod;
    uint32_t bitmapByteCount;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colorCount;
    uint32_t importantColorCount;
};
struct Color
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};
struct ColorTable
{
    Color* colors;
    uint32_t length;
};
struct PixelArray
{
    Color** pixels;
    int columnCount, rowCount;
    int padding = 0;
};
bool isBmpFile(FILE* f)
{
    if (f == NULL)
        return false;
    BmpSignature signature;
    fseek(f, 0, 0L);
    fread(&signature, sizeof(BmpSignature), 1, f);
    return signature.data[0] == 'B' && signature.data[1] == 'M';
}
void readBmpHeader(FILE* f, BmpHeader& header)
{
    if (f == NULL)
        return;
    fseek(f, 0, 0L);
    fread(&header, sizeof(BmpHeader), 1, f);
}
void printBmpHeader(BmpHeader header)
{
    printf("*** BMP Header ***");
    printf("-File Size  : %d byte(s)\n", header.fileSize);
    printf("-Reserved 1 :%d\n", header.reserved1);
    printf("-Reserved 2 :%d\n", header.reserved2);
    printf("-Data Offset:%d byte(s)\n", header.dataOffset);
}
void readBmpDib(FILE* f, BmpDib& dib)
{
    if (f == NULL)
        return;
    fseek(f, sizeof(BmpHeader), 0L);
    fread(&dib, sizeof(BmpDib), 1, f);
}
void printBmpLib(BmpDib dib)
{
    printf("*** BMP Dib ***\n");
    printf("-DIB Size:%d byte(s)\n", dib.dibSize);
    printf("-Image Width:%d\n", dib.imageWidth);
    printf("-Image Height:%d\n", dib.imageHeight);
    printf("-Number of Color Planes:%d\n", dib.colorPlaneCount);
    printf("-Pixel Size:%d bit(s)\n", dib.pixelSize);
    printf("-Compress Method:%d\n", dib.compressMethod);
    printf("-Bitmap Size:%d bytes(s)\n", dib.bitmapByteCount);
    printf("Horizontal Resolution:%d\n", dib.horizontalResolution);
    printf("-Vertical resolution:%d\n", dib.verticalResolution);
    printf("Number of Colors:%d\n", dib.colorCount);
    printf("Number of Impt Colors", dib.importantColorCount);
}
void scanBmpPixelLine(FILE* f, Color*& line, uint32_t length)
{
    if (f == NULL)
        return;
    line = new Color[length];
    fread(line, sizeof(Color), length, f);
}
void skipBmpPadding(FILE* f, char count)
{
    if (f == NULL)
        return;
    if (count == 0)
        return;
    char padding[3];
    fread(padding, count, 1, f);
}
void readBmpPixelArray(FILE* f, BmpHeader header, BmpDib dib, PixelArray& data)
{
    if (f == NULL)
        return;
    data.rowCount = dib.imageHeight;
    data.columnCount = dib.imageWidth;
    data.pixels = new Color * [data.rowCount];
    char paddingCount = (4 - (dib.imageWidth * (dib.pixelSize / 8) % 4)) % 4;
    fseek(f, header.dataOffset, 0L);
    for (int i = 0; i < data.rowCount; i++)
    {
        scanBmpPixelLine(f, data.pixels[data.rowCount - 1 - i], dib.imageWidth);
        skipBmpPadding(f, paddingCount);
    }
}
void drawBmp(BmpDib dib, PixelArray data)
{
    HWND console = GetConsoleWindow();
    HDC hdc = GetDC(console);
    for (int i = 0; i < dib.imageHeight; i++)
        for (int j = 0; j < dib.imageWidth; j++)
        {
            Color pixel = data.pixels[i][j];
            SetPixel(hdc, j, i, RGB(pixel.red, pixel.green, pixel.blue));
        }
    ReleaseDC(console, hdc);
}
void releaseBmpPixelArray(PixelArray data)
{
    for (int i = 0; i < data.rowCount; i++)
        delete[] data.pixels[i];
    delete[] data.pixels;
}

int main() {
    ofstream fout("output.bmp", ios::binary);
    BmpHeader header;
    header.signature = { 'B','M' };
    fout.write((char*)&header.signature, 2);
    header.fileSize = 70;
    fout.write((char*)&header.fileSize, 4);
    header.reserved1 = 0;
    fout.write((char*)&header.reserved1, 2);
    header.reserved2 = 0;
    fout.write((char*)&header.reserved2, 2);
    header.dataOffset = 54;
    fout.write((char*)&header.dataOffset, 4);

    int h;
    int w;
    cout << "Input height: ";
    cin >> h;
    cout << "Input width: ";
    cin >> w;

    BmpDib dib;
    dib.dibSize = 40;
    fout.write((char*)&dib.dibSize, 4);
    dib.imageWidth = w;
    fout.write((char*)&dib.imageWidth, 4);
    dib.imageHeight = h;
    fout.write((char*)&dib.imageHeight, 4);
    dib.colorPlaneCount = 1;
    fout.write((char*)&dib.colorPlaneCount, 2);
    dib.pixelSize = 24;
    fout.write((char*)&dib.pixelSize, 2);
    dib.compressMethod = 0;
    fout.write((char*)&dib.compressMethod, 4);
    dib.bitmapByteCount = w*h;
    fout.write((char*)&dib.bitmapByteCount, 4);
    dib.horizontalResolution = 2835;
    fout.write((char*)&dib.horizontalResolution, 4);
    dib.verticalResolution = 2835;
    fout.write((char*)&dib.verticalResolution, 4);
    dib.colorCount = 0;
    fout.write((char*)&dib.colorCount, 4);
    dib.importantColorCount = 0;
    fout.write((char*)&dib.importantColorCount, 4);
    PixelArray data;
    data.pixels = new Color * [dib.imageHeight];
    for (int i = 0; i <= dib.imageHeight; i++) {
        data.pixels[i] = new Color[dib.imageWidth];
    }
   
    int c = w / 2;
    int r = h / 2;
 
    for (int i = 0; i <= dib.imageHeight; i++) {
        for (int j = 0; j <= dib.imageWidth; j++) {
            data.pixels[i, j]->red = 255;
            data.pixels[i, j]->green = 0;
            data.pixels[i, j]->blue = 0;
            fout.write((char*)data.pixels[i, j], 3);
        }
    }
    fout.close();
}