#ifndef GRAPHICS_H
#define GRAPHICS_H

#define _WIDTH   640 
#define _HEIGHT  512
#define NROWS 20
#define NCOLS 40
#define DRAW 1
#define CLEAR 0
#define CENTERED 1
#define OFF_CENTERED 0

/* graphics.h
 * ----------------
 * Functions for a simple bare metal Raspberry Pi graphics library
 * that draws pixels, text, lines, triangles, and rectangles. Builds
 * on the lower-level framebuffer library fb.[ch] for framebuffer
 * access and configuration; trying to use both fb and gl
 * simultaneously is discouraged.
 *
 * For our nerf gun final project, this file will be used to draw out
 * (overlaid on the arducam scene) the target icon, display text when the
 * target is found and when darts are fired, etc. 
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Thursday March 21st, 2019
 *
 */


/* Initialize the graphics routine */
void graphics_init(void);


/* Writes the initial gun info to the screen (bullets, target location, etc.) */
void write_text(void);


/* Draws or removes a drawn target symbol on the screen 
 *
 * @param pos - the number of the currently active sensor
 * @param action - draw or remove the symbol 
 */
void draw_or_clear_target(int pos, int action);


/* Draws the number of darts left to the screen */
void draw_remaining_darts(void);


/* Draw the gun mode */
void draw_mode(void);


/* Initially draws a white border and a symbol for the gun to the screen */
void draw_frame_and_gun(void);


/* Displays info regarding if target is currently centered or not 
 * 
 * @param status - target either locked down or not locked down
 */
void draw_target_status(int status);


/* Draws the distance of the closest target
 *
 * @param distance - the distance of the closest target
 */
void draw_target_distance(int distance);


/* Displays whether the gun is currently firing or not 
 *
 * @param status - firing or not firing
 */
void draw_fire_status(int status);

#endif
