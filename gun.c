/*
 * File: gun.c
 * -------------------
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Friday March 15th, 2019
 *
 * Implementation of standard functions defined in gun.h for controlling
 * the movement and firing mechanism of the gun. Please see the gun.h header
 * file for function comments.
 */


#include "gun.h"
#include "gpio.h"
#include "timer.h"

static int bullets_left = NUM_DARTS;
static int gun_mode;

void gun_init(int mode){
   gpio_set_output(GUN_TRIGGER);
   gpio_set_output(ROTATOR_POS);
   gpio_set_output(ROTATOR_NEG);

   trigger_off();
   rotator_off();

   gun_mode = mode;
}

int get_gun_mode(void){
  return gun_mode;
}


void trigger_on(void){
   gpio_write(GUN_TRIGGER, 0); 
}


void trigger_off(void){
  gpio_write(GUN_TRIGGER, 1);  
}


void rotate_clockwise(void){
  gpio_write(ROTATOR_POS, 1);
  gpio_write(ROTATOR_NEG, 0);

}


void rotate_counter_clockwise(void){
  gpio_write(ROTATOR_POS, 0);
  gpio_write(ROTATOR_NEG, 1);

}

void rotator_off(void){
  gpio_write(ROTATOR_POS, 1);
  gpio_write(ROTATOR_NEG, 1);
}

void fire_once(void){
   trigger_on();
   timer_delay_ms(ONE_BULLET_DELAY);
   trigger_off();
   bullets_left = (bullets_left == 0) ? 0 : bullets_left - 1;
}

int get_ammo_remaining(void){
    return bullets_left;
}
