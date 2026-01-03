#include "pixel.h"

int PixelGet_(int x, int y)
{ return PrivatePixelGet_(x,y); }

void PixelSet_(int x, int y, uint8_t color)
{ PrivatePixelSet_(x,y, color); }

int PixelGetAtPlane_(int x, int y, int plane)
{ return PrivatePixelGetAtPlane_(x,y,plane); }

void PixelSetAtPlane_(int x, int y, int plane, int bit)
{ PrivatePixelSetAtPlane_(x,y,plane, bit); }
