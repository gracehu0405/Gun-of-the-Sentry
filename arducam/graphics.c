#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "printf.h"
#include "font.h"
#include "timer.h"
#include "console.h"

/* graphics.c
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


// size of the image coming in
#define _WIDTH   640 // 320
#define _HEIGHT  512 // 240

// size of the console, values too large get trimmed later
#define _NROWS	10 // 20
#define _NCOLS	20 // 50

static void write_text(void) {
    
    // Black background, white text
    // gl_clear(GL_BLACK);

    // Top left: Mode
    gl_draw_string(60, 100, "Mode:", GL_GREEN);
    gl_draw_string(80, 60, "Auto/Interactive", GL_GREEN);

    // Top right: Remaining Darts
    gl_draw_rect(_WIDTH - 100, 100, 50, 50, GL_WHITE); // square
    gl_draw_string(_WIDTH - 150, 100, "darts remaining", GL_GREEN);

    // Bottom Center: 
    gl_draw_string(_WIDTH/2 - 80, _HEIGHT/2, "Locked down target!", GL_BLUE);
    gl_draw_string(_WIDTH/2 - 160, _HEIGHT/2 + 40, "Target Distance: N in away", GL_BLUE);
    gl_draw_string(_WIDTH - 80, _HEIGHT - 40, "Firing Darts!", GL_BLUE);

    gl_swap_buffer();
    printf("\nSwap buffer finished");

    timer_delay(3);
    gl_clear(GL_BLACK);    
}
