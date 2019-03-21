#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "printf.h"
#include "font.h"
#include "timer.h"
#include "console.h"
#include "gun.h"

// size of the image coming in
#define _WIDTH   640 // 320
#define _HEIGHT  512 // 240

//int num_darts = 18;

/* graphics.h
 * -----
 * Functions for a simple bare metal Raspberry Pi graphics library
 * that draws pixels, text, lines, triangles, and rectangles. Builds
 * on the lower-level framebuffer library fb.[ch] for framebuffer
 * access and configuration; trying to use both fb and gl
 * simultaneously is discouraged.
 *
 * For our nerf gun final project, this file will be used to draw out
 * (overlaid on the arducam scene) the target icon, display text when the
 * target is found and when darts are fired, etc.
 */

 void graphics_init(void);

 void write_text(void);
