#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#include "gpio.h"

/* Functions for controlling and reading input
 * from an array of ultrasound sensors
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Wednesday March 13th, 2019
 *
 */


// Pin mappings
enum pins {
  TRIGGER_0 = GPIO_PIN3,
  ECHO_RISING_PIN_0 = GPIO_PIN2,
  ECHO_FALLING_PIN_0 = GPIO_PIN4,

  TRIGGER_1 = GPIO_PIN17,
  ECHO_RISING_PIN_1 = GPIO_PIN27,
  ECHO_FALLING_PIN_1 = GPIO_PIN22,

  TRIGGER_2 = GPIO_PIN10,
  ECHO_RISING_PIN_2 = GPIO_PIN9,
  ECHO_FALLING_PIN_2 = GPIO_PIN11,

  // Add more triggers, rising, and falling edges 
};

// Number of sensors being utilized
#define NUM_SENSORS 7

// Initialize the ultrasound modules
void ultraSound_init(void);


// Add function to get smallest distance


// (Temporary) function to return distance - do bounds error checking
unsigned int getDistance(int pos);

#endif
