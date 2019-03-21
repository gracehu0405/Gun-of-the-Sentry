#ifndef SENSORMAP_H
#define SENSORMAP_H

#include "gpio.h"

/* Definitions of pin mappings to physical hardware for the
 * ultrasound sensor array
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Thursday March 14th, 2019
 *
 */

// Pin mappings
enum pins {
  TRIGGER_0 = GPIO_PIN6,
  ECHO_RISING_PIN_0 = GPIO_PIN5,
  ECHO_FALLING_PIN_0 = GPIO_PIN13,

  TRIGGER_1 = GPIO_PIN17,
  ECHO_RISING_PIN_1 = GPIO_PIN27,
  ECHO_FALLING_PIN_1 = GPIO_PIN22,

  TRIGGER_2 = GPIO_PIN25, //moved from 10
  ECHO_RISING_PIN_2 = GPIO_PIN26, //moved from 9
  ECHO_FALLING_PIN_2 = GPIO_PIN19, //moved from 11 

  // Add more triggers, rising, and falling edges
};

// Number of sensors being utilized
#define NUM_SENSORS 3

/** STRUCT: ultrasonic_sensor_t
 *
 * Each standard HC-SR04 ultrasonic sensor has one trigger pin
 * and one echo pin. We create two enrties for the echo pin,
 * to be able to independently detect both rising and falling edges
 * on the pin.
 */
typedef struct {
    unsigned int trigger;
    unsigned int echo_rising_pin;
    unsigned int echo_falling_pin;
 } ultrasonic_sensor_t;

extern ultrasonic_sensor_t const sensor_array[NUM_SENSORS];

#endif
