#include "graphics.h"
#include "gl.h"
#include "fb.h"
#include "strings.h"
#include "printf.h"
#include "font.h"
#include "timer.h"
#include "console.h"
#include "gun.h"
#include "ultrasound.h"

/*
 * File: graphics.c
 * -------------------
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Thursday March 21st, 2019
 *
 * Functions for a simple bare metal Raspberry Pi graphics library
 * that draws pixels, text, lines, triangles, and rectangles. Builds
 * on the lower-level framebuffer library fb.h for framebuffer
 * access and configuration; trying to use both fb and gl
 * simultaneously is discouraged.
 *
 * For our nerf gun final project, this file will be used to draw out 
 * the target icon, display text when the target is found and when darts are fired, etc. 
 * Please see the header file for function comments.
 */


void graphics_init(void) {
    gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);
    gl_clear(GL_BLACK);
}

void write_text(void) {
    draw_mode();
    draw_remaining_darts(); 
    draw_frame_and_gun();
    draw_target_status(OFF_CENTERED);
    draw_target_distance(MAX_RANGE);
    draw_fire_status(RESTING);
}


void draw_mode(void) {

    gl_draw_string(30, 150, "Mode:", GL_GREEN);
    gl_draw_string(40, 170, "A/I", GL_GREEN);

    if (get_gun_mode() == AUTO) {
        gl_draw_rect(38, 186, 14, 3, GL_YELLOW); // Left (A)
    } else {
        gl_draw_rect(66, 186, 14, 3, GL_YELLOW); // Right (I)
    }
}


void draw_remaining_darts(void) {
    gl_draw_rect(550, 140, 40, 20, GL_WHITE); // clear the current number 
    char buf[30];
    snprintf(buf, sizeof(buf), "%d", get_ammo_remaining());
    gl_draw_string(555, 143, buf, GL_BLUE);
    gl_draw_string(540, 165, "Darts", GL_GREEN);
    gl_draw_string(545, 185, "Left", GL_GREEN);
}


void draw_target_status(int status){
    gl_draw_rect(200, _HEIGHT/2 + 100, 300, 25, GL_BLACK); // clear the current target status 
    if(status == CENTERED){  
        gl_draw_string(200, _HEIGHT/2 + 100, "Locked down target!", GL_CYAN); 
    } else if(status == OFF_CENTERED){
        gl_draw_string(200, _HEIGHT/2 + 100, "Target not centered...", GL_CYAN); 
    } 
}


void draw_fire_status(int status){
    gl_draw_rect(235, _HEIGHT/2 + 200, 300, 25, GL_BLACK); // clear the current firing status
    if(status == FIRING){
        gl_draw_string(235, _HEIGHT/2 + 200, "Firing Darts!", GL_CYAN); 
    }else if(status == RESTING){
        gl_draw_string(235, _HEIGHT/2 + 200, "Not firing...", GL_CYAN);
    }
}


void draw_target_distance(int distance){
    char buf[100];
    snprintf(buf, sizeof(buf), "Target Distance: %d inches away", distance);
    gl_draw_rect(100, _HEIGHT/2 + 150, 500, 25, GL_BLACK); // clear the current distance
    gl_draw_string(100, _HEIGHT/2 + 150, buf, GL_CYAN); 
}

void draw_frame_and_gun(void) {

    // Draw Gun Symbol
    gl_draw_rect(300, 250, 40, 20, GL_BLUE);
    gl_draw_string(302, 280, "Gun", GL_GREEN);

    // Draw Frame box
    gl_draw_line(120, 0, 120, 325, GL_WHITE);
    gl_draw_line(_WIDTH - 120, 0, _WIDTH - 120, 325, GL_WHITE);
    gl_draw_line(120, 325, _WIDTH - 120, 325, GL_WHITE);
}


void draw_or_clear_target(int pos, int action) {

    static int location_1_x = 315, location_1_y1 = 100, location_1_y2 = 104;
    static int location_0_x = 430, side_location_y1 = 130, side_location_y2 = 134;
    static int location_2_x = 200;

    color_t color1, color2;
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
        gl_draw_triangle(location_1_x + 5, location_1_y1, location_1_x, location_1_y1 + 10, location_1_x + 10, location_1_y1 + 10, color1); 
        gl_draw_triangle(location_1_x + 5, location_1_y2 + 10, location_1_x, location_1_y2, location_1_x + 10, location_1_y2, color2); 
    } else if(pos == 0){
        gl_draw_triangle(location_0_x + 5, side_location_y1, location_0_x, side_location_y1 + 10, location_0_x + 10, side_location_y1 + 10, color1); 
        gl_draw_triangle(location_0_x + 5, side_location_y2 + 10, location_0_x, side_location_y2, location_0_x + 10, side_location_y2, color2); 
    } else if(pos == 2){
        gl_draw_triangle(location_2_x + 5, side_location_y1, location_2_x, side_location_y1 + 10, location_2_x + 10, side_location_y1 + 10, color1); 
        gl_draw_triangle(location_2_x + 5, side_location_y2 + 10, location_2_x, side_location_y2, location_2_x + 10, side_location_y2, color2); 
    }
}

