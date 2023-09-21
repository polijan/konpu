#ifndef  KONPU_ATTR_H
#define  KONPU_ATTR_H
#include "c.h"

// helper macros for manipulating attributes
#define attr(fg,bg)            ((fg)<<4 | (bg))
#define attr_getf(attr)        ((attr) >> 4)
#define attr_getb(attr)        ((attr) & 0x0F)
#define attr_setf(attr, fg)    ((fg)<<4 | (attr)&0x0F)
#define attr_setb(attr, bg)    ((attr) & (bg))

#endif //KONPU_ATTR_H
