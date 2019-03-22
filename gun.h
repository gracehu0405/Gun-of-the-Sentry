#ifndef GUN_H
#define GUN_H

#define NUM_DARTS 18
#define ONE_BULLET_DELAY 333
#define CALIBRATION_DELAY 500
#define TRACKING_TIMEOUT 2000000
#define FIRING 1
#define RESTING 0

#include "gpio.h"

/* Functions for controlling and operating the gun
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Sunday March 10th, 2019
 *
 */

enum gun_mode{ AUTO = 0, INTERACTIVE = 1 };

enum gun_pins{
    ROTATOR_POS = GPIO_PIN16,
    ROTATOR_NEG = GPIO_PIN20,
    GUN_TRIGGER = GPIO_PIN21,
};

/* Initialize the gun for operation */
void gun_init(int mode);

/* Returns the current mode of the gun (auto or interactive) */
int get_gun_mode(void);

/* Turn the trigger on (fires the gun) */
void trigger_on(void);

/* Turn the trigger off (disables gun fire) */
void trigger_off(void);

/* Rotate the gun clockwise */
void rotate_clockwise(void);

/* Rotate the gun counter clockwise */
void rotate_counter_clockwise(void);

/* Hold the gun still */
void rotator_off(void);

/* Fire 1 bullet */
void fire_once(void);

/* Returns the number of unfired bullets left in the gun */
int get_ammo_remaining(void);


#endif
