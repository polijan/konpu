#include "pixel.h"

int PixelGet_(int x, int y)
{ return PixelGet_internal_(x,y); }

void PixelSet_(int x, int y, int color)
{ PixelSet_internal_(x,y, color); }
