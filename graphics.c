#include "graphics.h"

/* graphics.c
 * -----
 * Functions for a simple bare metal Raspberry Pi graphics library
 * that draws pixels, text, lines, triangles, and rectangles. Builds
 * on the lower-level framebuffer library fb.h for framebuffer
 * access and configuration; trying to use both fb and gl
 * simultaneously is discouraged.
 *
 * For our nerf gun final project, this file will be used to draw out
 * (overlaid on the arducam scene) the target icon, display text when the
 * target is found and when darts are fired, etc.
 */


void graphics_init(void) {
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);
    gl_clear(GL_BLACK);
}

void write_text(void) {

    // Top left: Mode
    gl_draw_string(30, 150, "Mode:", GL_GREEN);
    gl_draw_string(40, 170, "A/I", GL_GREEN);

    if (mode == AUTO) {
        gl_draw_rect(38, 186, 14, 3, GL_YELLOW); // Left (A)
    } else {
        gl_draw_rect(66, 186, 14, 3, GL_YELLOW); // Right (I)
    }


    // Top right: Remaining Darts
    gl_draw_rect(560, 140, 20, 20, GL_WHITE); // square

    // char buf[30];
    //memset(buf, 0x77, sizeof(buf));
    //snprintf(buf, 5, "%d", num_darts);

    gl_draw_string(565, 143, "8", GL_BLUE); // TODO: not constant 
    gl_draw_string(540, 165, "Darts", GL_GREEN);
    gl_draw_string(545, 185, "Left", GL_GREEN);

    // Bottom Center: Display info
    gl_draw_string(200, _HEIGHT/2 + 100, "Locked down target!", GL_CYAN); // TODO: not constant
    gl_draw_string(100, _HEIGHT/2 + 150, "Target Distance: 9 inches away", GL_CYAN); // TODO: not constant
    gl_draw_rect(_WIDTH/2 + 15, _HEIGHT/2 + 170, 16, 3, GL_BLUE); // Underline the number
    gl_draw_string(235, _HEIGHT/2 + 200, "Firing Darts!", GL_CYAN); // TODO: not constant

    // Target symbol
    gl_draw_triangle(320, 100, 315, 110, 325, 110, GL_RED);
    gl_draw_triangle(320, 114, 315, 104, 325, 104, GL_MAGENTA);

    // Draw Gun Symbol
    gl_draw_rect(300, 250, 40, 20, GL_BLUE);

    // Draw Frame box
    gl_draw_line(120, 0, 120, 325, GL_WHITE);
    gl_draw_line(_WIDTH - 120, 0, _WIDTH - 120, 325, GL_WHITE);
    gl_draw_line(120, 325, _WIDTH - 120, 325, GL_WHITE);


    // Display screen
    gl_swap_buffer();
}
