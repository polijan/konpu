// Various small Functions and utility MACROs

#ifndef KONPU_UTIL_H_
#define KONPU_UTIL_H_
#include "c.h" // c.h also defines many UTIL_* macros

// Similar to the `container_of` macro as popularized by the Linux kernel. I.e:
// Given the type T of a "container" struct and the name of the member the
// pointer refers to, returns a pointer to the struct that contains that member.
// For more info, see: https://radek.io/posts/magical-container_of-macro/
#define UTIL_CONTAINER_OF(pointer, T, member) \
   ((T*)((char*)(1 ? (pointer) : &((T*)0)->member) - offsetof(T, member)))

// Clamp the value of `n` into [0..dimension[
static inline void UtilClampCoordinate(int *n, int dimension) {
   if (*n < 0) {
      *n = 0;
   } else if (*n >= dimension) {
      *n = dimension - 1;
   }
}

// A comma. (Occasionally useful to pass a comma as a macro parameter)
#define UTIL_COMMA ,

// Return the (signed) length of a C string literal.
#define UTIL_STRING_LITERAL_LENGTH(literal_string) \
   (C_SIZEOF(literal_string) - 1)

// Return non-zero iff the given integer is a power of two.
// Note: macro evaluates its argument several times.
#define UTIL_IS_POWER_OF_TWO(x)          (((x) > 0) && !((x) & ((x) - 1)))

// Return non-zero iff the given integer is a power of two or zero.
// Note: macro evaluates its argument several times.
#define UTIL_IS_POWER_OF_TWO_OR_ZERO(x)  (((x) >= 0) && !((x) & ((x) - 1)))

// Return the maximum of a and b
// Warning: No macro safety (a and b will be evaluated twice)
#define UTIL_MAX(a,b)         (((a) >= (b))? (a) : (b))

// Return the minimum of a and b
// Warning: No macro safety (a and b will be evaluated twice)
#define UTIL_MIN(a,b)         (((a) <= (b))? (a) : (b))

// Mathematical constant Pi (as a double)
#define UTIL_PI \
   3.1415926535897932384626433832795028841971693993751058209749445923078164062

// Mathematical constant Pi (as a float)
#define UTIL_PIf \
   3.1415926535897932384626433832795028841971693993751058209749445923078164062f

//------------------------------------------------------------------------------
// Variadic preprocessor utilities (in complement to what's already in c.h)
//------------------------------------------------------------------------------

// UTIL_ARG_AT(INDEX, arguments...) macro expands to the INDEX-1 th argument
#define UTIL_ARG_AT(INDEX, ...) \
   UTIL_CAT(UTIL_CAT(PRIVATE_UTIL_ARG_AT_, INDEX), _)(__VA_ARGS__)
#  define PRIVATE_UTIL_ARG_AT_0_(n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_1_(n01, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_2_(n01, n02, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_3_(n01, n02, n03, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_4_(n01, n02, n03, n04, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_5_(n01, n02, n03, n04, n05, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_6_(n01, n02, n03, n04, n05, n06, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_7_(n01, n02, n03, n04, n05, n06, n07, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_8_(n01, n02, n03, n04, n05, n06, n07, n08, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_9_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_10_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_11_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_12_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_13_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_14_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_15_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_16_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_17_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_18_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_19_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_20_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_21_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_22_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_23_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_24_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_25_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_26_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_27_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_28_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_29_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_30_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_31_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_32_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_33_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_34_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_35_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_36_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_37_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_38_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_39_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_40_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_41_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_42_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_43_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_44_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_45_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_46_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_47_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_48_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n, ...)  n
#  define PRIVATE_UTIL_ARG_AT_49_(n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n49, n, ...)  n
// UTIL_ARG_AT_50_ is available in "c.h"

#endif //include guard
