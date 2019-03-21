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
#define MAX_RANGE 150
#define TIMEOUT (MAX_RANGE * INCH_CONVERSION)

// Initialize the ultrasound modules
void ultraSound_init(void);

// Returns array index of middle sensor
int middleSensor(void);

// Returns index of sensor with the closest object 
int closestSensor(void);

// Returs the distance of the object in front of the sensor at pos location
int getDistance(int pos);

#endif
