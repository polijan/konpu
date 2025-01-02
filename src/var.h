// defines vars and related types or operations

#ifndef  KONPU_VAR_H
#define  KONPU_VAR_H
#include "c.h"

enum TypeBuiltin {
   // stack-only builtin types
   TYPE_NULL,         // fields are usually unused
   TYPE_INTEGER,      // use: .integer
   TYPE_REAL,         // use: .real
   TYPE_BITS,         // use: .uint64 (or .bytes)
   TYPE_CHAR,         // use: .chr
   TYPE_BOOLEAN,      // use: .boolean
   TYPE_ADDRESS,      // use: .address / .int32
   TYPE_HEAP_ADDRESS, // use: .uint16
   TYPE_POINTER,      // use: .pointer
   TYPE_CSTRING,      // use: .pointer
   TYPE_ERROR,        // use: .error
   TYPE_TYPE,         // use: .type
   TYPE_GLYPH8,       // use: .glyph8
   TYPE_GLYPH16,      // use: .glyph16
   TYPE_GLYPH32,      // use: .glyph32
   TYPE_GLYPH64,      // use: .glyph64

   // builtin types that use the heap
   TYPE_GLYPH128,
   TYPE_GLYPH256,
   TYPE_PAIR,         // .uint16_t->block of var1 .uint16->type1
   TYPE_OBJECT,       // 16bit:heap_block, 16bit:type, 32bit: refcount
};

// A Var value should be associated a type
// should it be int8, uint8, int16, or uint16?
typedef int16_t Type;

// return true iff the type indicates a a type can a
// simple value that fits in 64 bits, in which case, it can fits
// into the WasoStack
#define TypeIsSimple(type)      (type < TYPE_GLYPH128)
#define TypeIsNotSimple(type)   (type >= TYPE_GLYPH128)


// A Var (or var) is 64-bit union which can take      .-----64-----.
// many shapes. It is used extensively.            .--32--.    .--32--.
// The Waso stack contains vars.                  .16.  .16.  .16.  .16.
//                                                8  8  8  8  8  8  8  8
typedef union Var {
   // (64 bits [or less] single field)
   int64_t     int64, integer;
   uint64_t    uint64, glyph64;
   double      float64, real;
   uint8_t     bytes[8];
   char        string8[8];
   // we ASSUME (and later static_assert it) that bool, int, double, pointers,
   // and function pointers are at max. 64 bits
   int         error;
   bool        boolean;
   const char *cstring;
   void       *pointer;           // generic pointer
   int       (*function)(void*);  // generic function
   int       (*command)(void);    // waso command
#ifdef KONPU_TOKIPONA
   int64_t   nanpa64, nanpa;
   uint64_t  suli64, ijo64, sitelen64;
   double    kipisi64, kipisi;
   uint8_t   ijo[8];
   char      nimi8[8];
   int       pakala;
   bool      lon;
   int     (*pali)(void*);
   int     (*pali_waso)(void);
#endif
   struct { // (32,32)
      union {
         int32_t   int32, address;
         uint32_t  uint32, glyph32;
         float     float32;
#ifdef KONPU_TOKIPONA
         int32_t   nanpa32;
         uint32_t  suli32, sitelen32;
         float     kipisi32;
#endif
         struct { // (16,16)
            union {
               int16_t   int16;
               uint16_t  uint16, glyph16;
               Type      type;
#ifdef KONPU_TOKIPONA
               int16_t   nanpa16;
               uint16_t  suli16, sitelen16;
               Type      poki;
#endif
               struct { // (8,8)
                  union {
                     int8_t   int8;
                     uint8_t  uint8, glyph8;
                     char     chr;
                  };
                  union {
                     int8_t   int8_2;
                     uint8_t  uint8_2;
                     char     chr_2;
                  };
#ifdef KONPU_TOKIPONA
                  union {
                     int8_t   nanpa8;
                     uint8_t  suli8, sitelen8;
                     char     nimi;
                  };
                  union {
                     int8_t   nanpa8_2;
                     uint8_t  suli8_2;
                     char     chr_2;
                  };
#endif

               };
            };
            union {
               int16_t   int16_2;
               uint16_t  uint16_2;
               struct { // (8,8)
                  union {
                     int8_t   int8_3;
                     uint8_t  uint8_3;
                     char     chr_3;
                  };
                  union {
                     int8_t   int8_4;
                     uint8_t  uint8_4;
                     char     chr_4;
                  };
               };
            };
         };
      };
      union {
         int32_t   int32_2;
         uint32_t  uint32_2, refcount;
         float     float32_2;
         struct { // (16,16)
            union {
               int16_t   int16_3;
               uint16_t  uint16_3;
               struct { // (8,8)
                  union {
                     int8_t   int8_5;
                     uint8_t  uint8_5;
                     char     chr_5;
                  };
                  union {
                     int8_t   int8_6;
                     uint8_t  uint8_6;
                     char     chr_6;
                  };
               };
            };
            union {
               int16_t   int16_4;
               uint16_t  uint16_4;
               struct { // (8,8)
                  union {
                     int8_t   int8_7;
                     uint8_t  uint8_7;
                     char     chr_7;
                  };
                  union {
                     int8_t   int8_8;
                     uint8_t  uint8_8;
                     char     chr_8;
                  };
               };
            };
         };
      };
   };
} Var;
static_assert(sizeof(Var)  == 8);
static_assert(alignof(Var) <= 8);

// We give the synonym 'var' to the Var union. It's a very common type within
// ilo Konpu and when declaring C variables as var, it starts to make the name
// feel like like a keyword that could come from a dynamic language.
typedef Var var;
#ifdef KONPU_TOKIPONA
   typedef Var Ijo;
   typedef var ijo;
#endif

// A var value whose bits are set to zero, including the padding bits.
#define VAR_ZERO  ((var){ .uint64 = 0 })

// Return a 64-bits hash value from a var.
// To work properly as a hash function, the value of the var should obey the
// following rules:
// - The value must only depend on the bits stored in the var union
//   (note: this implies that the var must not be a heap object)
// - If the value uses less then 64 bits, the unused padding bits of every vars
//   you hash must be the same (so in practice, initialize vars you may want to
//   hash from VAR_ZERO)
static inline uint64_t VarHash(var value64)
{
   return value64.uint64 * 0xc6a4a7935bd1e99d;
   // This is a fast hash as it uses a single operation. That multiplication is
   // meant to "scatter" the bits around. I don't know how this constant was
   // derived, it might represent some big prime number, I simply took it from
   // the great (MIT-licensed) STC library: https://github.com/stclib/STC
   // For other good options for a simple multiplicative factor, see:
   // https://softwareengineering.stackexchange.com/questions/402542/where-do-magic-hashing-constants-like-0x9e3779b9-and-0x9e3779b1-come-from
}


// Print a Var
void VarPrintTTY(var v, Type t);


//void VarDrop(var v, Type type);
#define VarDrop(var, type)


#endif //KONPU_VAR_H
