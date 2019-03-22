#ifndef GRAPHICS_H
#define GRAPHICS_H

#define _WIDTH   640 // 320
#define _HEIGHT  512 // 240
#define DRAW 1
#define CLEAR 0

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

 void update_screen(void);

 void draw_or_clear_target(int pos, int action);


#endif
