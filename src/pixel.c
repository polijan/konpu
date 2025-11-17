#include "pixel.h"

int PixelGet_(int x, int y)
{ return PixelGet_internal_(x,y); }

void PixelSet_(int x, int y, uint8_t color)
{ PixelSet_internal_(x,y, color); }
