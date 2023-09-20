/*******************************************************************************
 * @file
 * The renderer paints the global `screen` canvas on the physical screen.
 * This file provides:
 * * the `render()` function to paint the screen,
 * * the `renderer_drop()` to uninitialize the renderer
 * And concrete implementations of renderers provide initialization methods.
 ******************************************************************************/
#ifndef  KONPU_RENDERER_H
#define  KONPU_RENDERER_H
#include "platform.h"
#include "c.h"


/*  maybe TODO elsewhere?
/// @brief initialize a renderer and set it as the active one.
/// @param id renderer id, or -1 (= "best rendered" as determined by Konpu)
/// @param ... extra initializing parameters as may be accepted by the renderer
/// @return 0 iff initialization is successful
int renderer_init(int id, ...);
*/

/// @brief   the active renderer paints the "screen" canvas
/// @details this function doesn't return an error code. Instead, if an error
///          occurs and the screen cannot be painted, the renderer *may* keep
///          a count of the number of frames which could not be rendered. This
///          can then be queried using the `renderer_getErrors()` function
static inline  void  render(void);

/// @brief   uninitialize Konpu's renderer, set it to the "Null" renderer.
/// @return  non-zero iff an error occurs while releasing the renderer.
///          (note: In a good design, *un*initialization should not fail.
///                 All the builtin renderers will always return 0)
/// @details In case of an error:
/// * the return value is the type of the renderer
/// * advanced: the renderer_getError() function can be also be queried to get
///             an extra error code set by the renderer.
int renderer_drop(void);

/// @brief this is the "Null" renderer, which doesn't display anything.
/// This is the default renderer at the start of the program
///                              or after `render_drop()` is called.
#define RENDERER_NULL  1

/// @brief  return what is the current renderer
static inline int renderer_getId(void);

/// @brief  return an error count
/// @return * If a renderer is properly initialized, then:
///           - if the initialized renderer supports it, the error count
///             indicates the number of frames which haven't been renderered
///             properly (this number may wrap around in case of overflow).
///           - if the renderer doesn't report errors, the value is 0.
///         * After a failed uninitialization from the drop function,
///           the value returned is an error code specific to the renderer.
static inline int renderer_getError(void);


//--- temporary: foreground/background color for the renderer-------------------

#ifndef RENDERER_FG_R
#   define RENDERER_FG_R 0xCC
#   define RENDERER_FG_G 0xCC
#   define RENDERER_FG_B 0x00
#   define RENDERER_BG_R 0x00
#   define RENDERER_BG_G 0x00
#   define RENDERER_BG_B 0x80
#endif


//--- advanced: interface to define your own custom renderer -------------------

/// @brief a renderer object
struct rendererObject {
   int (*render)(void);  ///< function which renders the "screen" canvas
   int (*drop)(void);    ///< function which releases possible resources that
                         ///<   were hold by the renderer.
   // internally kept as unsigned to prevent UB on overflow:
   unsigned error;       ///< error field:
                         ///<   * after renderer has been initialized:
                         ///<     this is a counter of the number of frames
                         ///<     where a rendering error occured.
                         ///<   * after renderer has been dropped:
                         ///<     this is zero or the error code which has
                         ///<     been returned by the drop function.
   int id;               ///< value which indicates what's the renderer
                         ///<   * value is from the `enum renderer` for the
                         ///<     builtin Konpu renderers,
                         ///<   * or could be different for a customer renderer
};

/// @brief this function takes no argument and always return the value 0.
/// This can be useful if you implement a renderer which need not unitialize
/// anything, as in this case, you can pass a pointer to `renderer_null` as
/// the `drop` function.
/// @param  none
/// @return 0
int renderer_null(void);


//--- inline implementation ----------------------------------------------------

/// @brief global variable for the active renderer object
/// At the start of the program ro when a renderer is `drop`ped, it is still
/// possible to call the `render()` function, but it will have no effect.
extern struct rendererObject rendererSingleton;

static inline void render(void)
{
   // (unlikely is used as it doesn't matter if error "costs" more)
   if (unlikely( (*rendererSingleton.render)() ))
      rendererSingleton.error++;
}

static inline int renderer_getError(void)
{
   return (int)(rendererSingleton.error);
}

static inline int renderer_getId(void)
{
   return rendererSingleton.id;
}

#endif //KONPU_RENDERER_H
