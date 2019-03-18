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

    // Top left: Mode
    gl_draw_string(10, 105, "Mode:", GL_GREEN);
    gl_draw_string(20, 125, "A/I", GL_GREEN);

    // gl_draw_rect(18, 140, 14, 3, GL_YELLOW); // Left (A)
    gl_draw_rect(46, 140, 14, 3, GL_YELLOW); // Right (I)


    // Top right: Remaining Darts
    gl_draw_rect(430, 95, 20, 20, GL_WHITE); // square
    gl_draw_string(435, 98, "8", GL_BLUE);
    gl_draw_string(410, 120, "Darts", GL_GREEN);
    gl_draw_string(415, 140, "Left", GL_GREEN);

    // Bottom Center: Display info 
    gl_draw_string(110, _HEIGHT/2, "Locked down target!", GL_CYAN);
    gl_draw_string(60, _HEIGHT/2 + 40, "Target Distance: 9 in away", GL_CYAN);
    gl_draw_rect(_WIDTH/2 - 26, _HEIGHT/2 + 60, 16, 3, GL_BLUE); // Underline the number
    gl_draw_string(145, _HEIGHT/2 + 80, "Firing Darts!", GL_CYAN);

    // Target symbol
    gl_draw_triangle(250, 100, 245, 110, 255, 110, GL_RED);
    gl_draw_triangle(250, 114, 245, 104, 255, 104, GL_RED);


    // Display screen
    gl_swap_buffer();
    printf("\nSwap buffer finished\n");

}
