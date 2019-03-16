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


void gun_init(void){
// write them all to HIGH initially (re-wire trigger relay switch so it's active low)
//
   gpio_set_output(GUN_TRIGGER);
   gpio_set_output(ROTATOR_POS);
   gpio_set_output(ROTATOR_NEG);

   trigger_off();
   rotator_off();
}


void trigger_on(void){
   gpio_write(GUN_TRIGGER, 0); // check this, active low
}


void trigger_off(void){
  gpio_write(GUN_TRIGGER, 1);  // check this, passive high
}


// check directions
void rotate_clockwise(void){
  gpio_write(ROTATOR_POS, 1);
  gpio_write(ROTATOR_NEG, 0);

}


// check directions
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
   timer_delay(3);
   trigger_off();
}



