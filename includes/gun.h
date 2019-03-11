#ifndef GUN_H
#define GUN_H

/* Functions for controlling and operating the gun 
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Sunday March 10th, 2019
 *
 */


enum gun_mode {INTERACTIVE = 0, AUTO = 1};

//static int mode; // check need for this?


/* Initialize the gun for operation in the given mode */
void gun_init(int mode_num);


/* Get the current mode of operation. Can be 
 * a) INTERACTIVE - user controlled
 * b) AUTO- autonomous operation and automatic aiming
 *
 * @ return - the current mode of operation
 */
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


#endif
