#include "graphics.h"
#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "printf.h"
#include "font.h"
#include "timer.h"
#include "console.h"
//#include "gun.h"

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
    gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);
    gl_clear(GL_BLACK);
}

void write_text(void) {

    // Top left: Mode
    gl_draw_string(30, 150, "Mode:", GL_GREEN);
    gl_draw_string(40, 170, "A/I", GL_GREEN);

    /* if (get_gun_mode() == AUTO) {
       gl_draw_rect(38, 186, 14, 3, GL_YELLOW); // Left (A)
       } else {
       gl_draw_rect(66, 186, 14, 3, GL_YELLOW); // Right (I)
       }*/

    gl_draw_rect(38, 186, 14, 3, GL_YELLOW); // Left (A)


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

    // Draw Gun Symbol
    gl_draw_rect(300, 150, 40, 20, GL_BLUE);
    gl_draw_string(303, 190, "Gun", GL_GREEN);

    // Draw Frame box
    gl_draw_line(120, 0, 120, 325, GL_WHITE);
    gl_draw_line(_WIDTH - 120, 0, _WIDTH - 120, 325, GL_WHITE);
    gl_draw_line(120, 325, _WIDTH - 120, 325, GL_WHITE);

}

void update_screen(void){



}

void draw_or_clear_target(int pos, int action){

    static int location_1_x = 315;
    static int location_1_y1 = 100;
    static int location_1_y2 = 104;

    static int location_0_x = 200;
    static int side_location_y1 = 130;
    static int side_location_y2 = 134;

    static int location_2_x = 430;

    color_t color1;
    color_t color2;
    if(action == DRAW) {
      color1 = GL_RED;
      color2 = GL_MAGENTA;
    } else if(action == CLEAR){
      color1 = GL_BLACK;
      color2 = GL_BLACK;
    } else {
        return; // invalid action
    }

    if(pos == 1){
        gl_draw_triangle(location_1_x + 5, location_1_y1, location_1_x, location_1_y1 + 10, location_1_x + 10, location_1_y1 + 10, color1); // TODO: not constant
        gl_draw_triangle(location_1_x + 5, location_1_y2 + 10, location_1_x, location_1_y2, location_1_x + 10, location_1_y2, color2); // TODO: not constant
    } else if(pos == 0){
        gl_draw_triangle(location_0_x + 5, side_location_y1, location_0_x, side_location_y1 + 10, location_0_x + 10, side_location_y1 + 10, color1); 
        gl_draw_triangle(location_0_x + 5, side_location_y2 + 10, location_0_x, side_location_y2, location_0_x + 10, side_location_y2, color2); 
    } else if(pos == 2){
        gl_draw_triangle(location_2_x + 5, side_location_y1, location_2_x, side_location_y1 + 10, location_2_x + 10, side_location_y1 + 10, color1); 
        gl_draw_triangle(location_2_x + 5, side_location_y2 + 10, location_2_x, side_location_y2, location_2_x + 10, side_location_y2, color2); 
    }

}

