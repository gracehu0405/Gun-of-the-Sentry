#ifndef ULTRASOUND_H
#define ULTRASOUND_H


/* Functions for controlling and reading input
 * from an array of ultrasound sensors
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Wednesday March 13th, 2019
 *
 */


#define INCH_CONVERSION 149
#define MAX_RANGE 79
#define TIMEOUT (MAX_RANGE * INCH_CONVERSION)

// Initialize the ultrasound modules
void ultraSound_init(void);


// Returns index of sensor with smallest distance, -1 if no signals below threshold
int closestSensor(void);

// (Temporary) function to return distance - do bounds error checking
unsigned int getDistance(int pos);

#endif
