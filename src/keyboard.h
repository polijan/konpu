#ifndef  KONPU_KEY_H_
#define  KONPU_KEY_H_
#include "arch.h"
#include "bits.h"


// In Konpu, the PHYSICAL LOCATION of a key is identified by an integer whose
// value is as per the USB HID Keyboard ScanCode. Here is a visual reference of
// where many keys are located on the three different types of computer
// keyboards available today (i.e. ISO, ANSI, and JIS):
//
// ```
// ┌──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┐ ┌───────────┐
// │41│ │58│59│60│61│ │62│63│64│65│ │66│67│68│69│ │70│71│72│ │ANSI LAYOUT│
// └──┘ └──┴──┴──┴──┘ └──┴──┴──┴──┘ └──┴──┴──┴──┘ └──┴──┴──┘ └───────────┘
// ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬─────┐ ┌──┬──┬──┐ ┌──┬──┬──┬──┐
// │53│30│31│32│33│34│35│36│37│38│39│45│46│  42 │ │73│74│75│ │83│84│85│86│
// ├──┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬────┤ ├──┼──┼──┤ ├──┼──┼──┼──┤
// │43 │20│26│ 8│21│23│28│24│12│18│19│47│48│ 49 │ │76│77│78│ │95│96│97│  │
// ├───┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴────┤ └──┴──┴──┘ ├──┼──┼──┤87│
// │ 57 │ 4│22│ 7│ 9│10│11│13│14│15│51│52│  40  │            │92│93│94│  │
// ├────┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴──────┤    ┌──┐    ├──┼──┼──┼──┤
// │ 225 │29│27│ 6│25│ 5│17│16│54│55│56│  229   │    │82│    │89│90│91│  │
// ├────┬┴──┼──┴┬─┴──┴──┴──┴──┼──┴┬─┴─┬┴──┬─────┤ ┌──┼──┼──┐ ├──┴──┼──┤88│
// │ 224│227│226│      44     │230│231│101│ 228 │ │80│81│79│ │  98 │99│  │
// └────┴───┴───┴─────────────┴───┴───┴───┴─────┘ └──┴──┴──┘ └─────┴──┴──┘
//
//
// ┌──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┐ ┌───────────┐
// │41│ │58│59│60│61│ │62│63│64│65│ │66│67│68│69│ │70│71│72│ │ISO  LAYOUT│
// └──┘ └──┴──┴──┴──┘ └──┴──┴──┴──┘ └──┴──┴──┴──┘ └──┴──┴──┘ └───────────┘
// ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬─────┐ ┌──┬──┬──┐ ┌──┬──┬──┬──┐
// │53│30│31│32│33│34│35│36│37│38│39│45│46│  42 │ │73│74│75│ │83│84│85│86│
// ├──┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬────┤ ├──┼──┼──┤ ├──┼──┼──┼──┤
// │43 │20│26│ 8│21│23│28│24│12│18│19│47│48│  4 │ │76│77│78│ │95│96│97│  │
// ├───┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┐ 0 │ └──┴──┴──┘ ├──┼──┼──┤87│
// │ 57 │ 4│22│ 7│ 9│10│11│13│14│15│51│52│50│   │            │92│93│94│  │
// ├───┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴─┬┴──┴───┤    ┌──┐    ├──┼──┼──┼──┤
// │225│ *│29│27│ 6│25│ 5│17│16│54│55│56│  229  │    │82│    │89│90│91│  │
// ├───┴┬┼┴──┴──┼──┴──┴──┴──┴──┴┬─┴─┬┴──┼───┬───┤ ┌──┼──┼──┐ ├──┴──┼──┤88│
// │224 │227│226│      44       │230│231│101│228│ │80│81│79│ │  98 │99│  │
// └────┴┼──┴───┴───────────────┴───┴───┴───┴───┘ └──┴──┴──┘ └─────┴──┴──┘
//       └100
//
//                                          ┌─137
// ┌──┐ ┌──┬──┬──┬──┐ ┌──┬──┬──┬──┐ ┌──┬──┬─┼┬──┐ ┌──┬──┬──┐ ┌───────────┐
// │41│ │58│59│60│61│ │62│63│64│65│ │66│67│68│69│ │70│71│72│ │JIS  LAYOUT│
// └──┘ └──┴──┴──┴──┘ └──┴──┴──┴──┘ └──┴──┴─┼┴──┘ └──┴──┴──┘ └───────────┘
// ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬─┼┬──┐ ┌──┬──┬──┐ ┌──┬──┬──┬──┐
// │53│30│31│32│33│34│35│36│37│38│39│45│46│ *│42│ │73│74│75│ │83│84│85│86│
// ├──┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴──┤ ├──┼──┼──┤ ├──┼──┼──┼──┤
// │43 │20│26│ 8│21│23│28│24│12│18│19│47│48│  4 │ │76│77│78│ │95│96│97│  │
// ├───┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┐ 0 │ └──┴──┴──┘ ├──┼──┼──┤87│
// │ 57 │ 4│22│ 7│ 9│10│11│13│14│15│51│52│50│   │            │92│93│94│  │
// ├────┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴┬─┴───┤    ┌──┐    ├──┼──┼──┼──┤
// │ 225 │29│27│ 6│25│ 5│17│16│54│55│56│ *│ 229 │    │82│    │89│90│91│  │
// ├───┬─┴─┬┴──┼──┴┬─┴──┴─┬┴──┬──┴┬─┴┬─┴┬┼┴┬────┤ ┌──┼──┼──┐ ├──┴──┼──┤88│
// │224│227│226│139│  44  │138│136│* │* ││*│228 │ │80│81│79│ │  98 │99│  │
// └───┴───┴───┴───┴──────┴───┴───┴┼─┴┼─┴┼┼┴────┘ └──┴──┴──┘ └─────┴──┴──┘
//                              230┘ 231 │└101
//                                      135
// ```
//
// Konpu gives names to the scan codes:
// - Codes [4-49] + [51-99] + 101 + [224-231] receives a descriptive name AS IF
//   they were the keys of an ANSI-type US QWERTY keyboard from a PC.
//   Despite the name, SCANCODES REMAIN JUST A PHYSICAL KEY LOCATION.
// - A few other keys not found on ANSI-type keyboard receive a name.
// - Of course, many keys that might exist are not named here.
enum KeyScanCode {
   KEY_SCANCODE_UNKNOWN = 0,

   KEY_SCANCODE_A =  4, KEY_SCANCODE_B =  5, KEY_SCANCODE_C =  6,
   KEY_SCANCODE_D =  7, KEY_SCANCODE_E =  8, KEY_SCANCODE_F =  9,
   KEY_SCANCODE_G = 10, KEY_SCANCODE_H = 11, KEY_SCANCODE_I = 12,
   KEY_SCANCODE_J = 13, KEY_SCANCODE_K = 14, KEY_SCANCODE_L = 15,
   KEY_SCANCODE_M = 16, KEY_SCANCODE_N = 17, KEY_SCANCODE_O = 18,
   KEY_SCANCODE_P = 19, KEY_SCANCODE_Q = 20, KEY_SCANCODE_R = 21,
   KEY_SCANCODE_S = 22, KEY_SCANCODE_T = 23, KEY_SCANCODE_U = 24,
   KEY_SCANCODE_V = 25, KEY_SCANCODE_W = 26, KEY_SCANCODE_X = 27,
   KEY_SCANCODE_Y = 28, KEY_SCANCODE_Z = 29, KEY_SCANCODE_1 = 30,
   KEY_SCANCODE_2 = 31, KEY_SCANCODE_3 = 32, KEY_SCANCODE_4 = 33,
   KEY_SCANCODE_5 = 34, KEY_SCANCODE_6 = 35, KEY_SCANCODE_7 = 36,
   KEY_SCANCODE_8 = 37, KEY_SCANCODE_9 = 38, KEY_SCANCODE_0 = 39,

   KEY_SCANCODE_RETURN         = 40,
   KEY_SCANCODE_ESCAPE         = 41,
   KEY_SCANCODE_BACKSPACE      = 42,
   KEY_SCANCODE_TAB            = 43,
   KEY_SCANCODE_SPACE          = 44,

   KEY_SCANCODE_MINUS          = 45,
   KEY_SCANCODE_EQUALS         = 46,
   KEY_SCANCODE_LEFTBRACKET    = 47,
   KEY_SCANCODE_RIGHTBRACKET   = 48,

   // ScanCode 49/50 is either the "backslash" key over the Return key (ANSI) or
   // the key at the lower left of the Return key (ISO, JIS).
   // ISO USB keyboards use ScanCode 50 for that latest one, but OSes treat 49
   // and 50 as the same code or may relay only the ScanCode 49 in any case.
   KEY_SCANCODE_BACKSLASH      = 49,
   KEY_SCANCODE_NONUSHASH      = 50,

   KEY_SCANCODE_SEMICOLON      = 51,
   KEY_SCANCODE_APOSTROPHE     = 52,
   KEY_SCANCODE_GRAVE          = 53,
   KEY_SCANCODE_COMMA          = 54,
   KEY_SCANCODE_PERIOD         = 55,
   KEY_SCANCODE_SLASH          = 56,

   KEY_SCANCODE_CAPSLOCK       = 57,

   KEY_SCANCODE_F1  = 58, KEY_SCANCODE_F2  = 59, KEY_SCANCODE_F3  = 60,
   KEY_SCANCODE_F4  = 61, KEY_SCANCODE_F5  = 62, KEY_SCANCODE_F6  = 63,
   KEY_SCANCODE_F7  = 64, KEY_SCANCODE_F8  = 65, KEY_SCANCODE_F9  = 66,
   KEY_SCANCODE_F10 = 67, KEY_SCANCODE_F11 = 68, KEY_SCANCODE_F12 = 69,

   KEY_SCANCODE_PRINTSCREEN    = 70,
   KEY_SCANCODE_SCROLLLOCK     = 71,
   KEY_SCANCODE_PAUSE          = 72,
   KEY_SCANCODE_INSERT         = 73,

   KEY_SCANCODE_HOME           = 74,
   KEY_SCANCODE_PAGEUP         = 75,
   KEY_SCANCODE_DELETE         = 76,
   KEY_SCANCODE_END            = 77,
   KEY_SCANCODE_PAGEDOWN       = 78,
   KEY_SCANCODE_RIGHT          = 79,
   KEY_SCANCODE_LEFT           = 80,
   KEY_SCANCODE_DOWN           = 81,
   KEY_SCANCODE_UP             = 82,

   KEY_SCANCODE_NUMLOCKCLEAR   = 83,
   KEY_SCANCODE_KP_DIVIDE      = 84,
   KEY_SCANCODE_KP_MULTIPLY    = 85,
   KEY_SCANCODE_KP_MINUS       = 86,
   KEY_SCANCODE_KP_PLUS        = 87,
   KEY_SCANCODE_KP_ENTER       = 88,
   KEY_SCANCODE_KP_1           = 89,
   KEY_SCANCODE_KP_2           = 90,
   KEY_SCANCODE_KP_3           = 91,
   KEY_SCANCODE_KP_4           = 92,
   KEY_SCANCODE_KP_5           = 93,
   KEY_SCANCODE_KP_6           = 94,
   KEY_SCANCODE_KP_7           = 95,
   KEY_SCANCODE_KP_8           = 96,
   KEY_SCANCODE_KP_9           = 97,
   KEY_SCANCODE_KP_0           = 98,
   KEY_SCANCODE_KP_PERIOD      = 99,

   KEY_SCANCODE_NONUSBACKSLASH = 100, // ISO-only: between Left Shift and 'Z'

   KEY_SCANCODE_APPLICATION    = 101, // Windows contextual menu, Compose key

   // remove?
   KEY_SCANCODE_VOLUMEUP       = 128,
   KEY_SCANCODE_VOLUMEDOWN     = 129,

   // only keep name for 135 and 137 ???
   KEY_SCANCODE_INTERNATIONAL1 = 135, // JIS and some Asian keyboards
   KEY_SCANCODE_INTERNATIONAL2 = 136,
   KEY_SCANCODE_INTERNATIONAL3 = 137, // JIS: Yen
   KEY_SCANCODE_INTERNATIONAL4 = 138,
   KEY_SCANCODE_INTERNATIONAL5 = 139,

   // Those are the modifier keys:
   KEY_SCANCODE_LCTRL          = 224,
   KEY_SCANCODE_LSHIFT         = 225,
   KEY_SCANCODE_LALT           = 226, // (Left) Alt, Option
   KEY_SCANCODE_LGUI           = 227, // (Left) Windows, Command (Apple), Meta
   KEY_SCANCODE_RCTRL          = 228,
   KEY_SCANCODE_RSHIFT         = 229,
   KEY_SCANCODE_RALT           = 230, // (Right) AltGr, Option
   KEY_SCANCODE_RGUI           = 231, // (Right) Windows, Command (Apple), Meta
};


// Non-zero iff any key was pressed during the last Update()
#define KEY_IS_ANY_DOWN()                            ( \
   Keyboard.current_u64[0] | Keyboard.current_u64[1] | \
   Keyboard.current_u64[2] | Keyboard.current_u64[3] )

// Non-zero iff any key was pressed during the previous Update()
#define KEY_WAS_ANY_DOWN()                             ( \
   Keyboard.previous_u64[0] | Keyboard.previous_u64[1] | \
   Keyboard.previous_u64[2] | Keyboard.previous_u64[3] )

// Non-zero iff the given key scancode was pressed during the last Update()
#define KEY_IS_DOWN(key_scancode) \
   BITS_GET_BIT(Keyboard.current[(key_scancode) >> 3], (key_scancode) & 7)

// Non-zero iff the given key scancode was pressed during the previous Update()
#define KEY_WAS_DOWN(key_scancode) \
   BITS_GET_BIT(Keyboard.previous[(key_scancode) >> 3], (key_scancode) & 7)

#define KEY_IS_UP(key_scancode)    (!KEY_IS_DOWN(key_scancode))

#define KEY_WAS_UP(key_scancode)   (!KEY_WAS_DOWN(key_scancode))

// Non-zero iff the given key was pushed just during the previous Update()
#define KEY_IS_TRIGGERED(key_scancode) \
   (KEY_IS_DOWN(key_scancode) && KEY_WAS_UP(key_scancode))

// Non-zero iff the given key was released during the previous Update()
#define KEY_IS_RELEASED(key_scancode) \
   (KEY_WAS_DOWN(key_scancode) && KEY_IS_UP(key_scancode))


//------------------------------------------------------------------------------
// Modifier keys (SHIFTs, CTRLs, ALTs, GUI)
//------------------------------------------------------------------------------

// Non-zero iff the given key a modifier key.
#define KEY_IS_MOD(key_scancode) \
   ((key_scancode) >= KEY_SCANCODE_LCTRL && (key_scancode) <= KEY_SCANCODE_RGUI)

// Bit number for repoting key press of modifiers keys in KEY_MOD
// (KEY_MOD | ...) tell that the given modifer key is pressed
enum KeyModBit {
   KEY_MOD_LCTRL   =   1u,
   KEY_MOD_LSHIFT  =   2u,
   KEY_MOD_LALT    =   4u,
   KEY_MOD_LGUI    =   8u,
   KEY_MOD_RCTRL   =  16u,
   KEY_MOD_RSHIFT  =  32u,
   KEY_MOD_RALT    =  64u,
   KEY_MOD_RGUI    = 128u,
};
#define KEY_MOD_NONE  0
#define KEY_MOD_SHIFT (KEY_MOD_LSHIFT | KEY_MOD_RSHIFT)
#define KEY_MOD_CTRL  (KEY_MOD_LCTRL  | KEY_MOD_RCTRL)
#define KEY_MOD_ALT   (KEY_MOD_LALT   | KEY_MOD_RALT)
#define KEY_MOD_GUI   (KEY_MOD_LGUI   | KEY_MOD_RGUI)

// An uint8_t lvalue for the modifier keys' current status
#define KEY_MOD  (Keyboard.current[KEY_SCANCODE_LCTRL / CHAR_BIT])
static_assert(KEY_SCANCODE_LCTRL % CHAR_BIT == 0);


//------------------------------------------------------------------------------

// Update the Key state
void KeyboardUpdate(void);



#endif //include guard
