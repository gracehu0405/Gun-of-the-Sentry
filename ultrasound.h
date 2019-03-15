#ifndef ULTRASOUND_H
#define ULTRASOUND_H


/* Functions for controlling and reading input
 * from an array of ultrasound sensors
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Wednesday March 13th, 2019
 *
 */



// Initialize the ultrasound modules
void ultraSound_init(void);


// Add function to get smallest distance (index)


// (Temporary) function to return distance - do bounds error checking
unsigned int getDistance(int pos);

#endif
