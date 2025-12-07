#include "pixel.h"

int PixelGet_(int x, int y)
{ return PixelGet_internal_(x,y); }

void PixelSet_(int x, int y, uint8_t color)
{ PixelSet_internal_(x,y, color); }

int PixelGetAtPlane_(int x, int y, int plane)
{ return PixelGetAtPlane_internal_(x,y,plane); }

void PixelSetAtPlane_(int x, int y, int plane, int bit)
{ PixelSetAtPlane_internal_(x,y,plane, bit); }
