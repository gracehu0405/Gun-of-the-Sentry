/* 
 * File: sensormap.c
 * -------------------
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Thursday March 14th, 2019
 *
 * Look up table to readily access key control pins for each sensor
 */


#include "sensormap.h"


ultrasonic_sensor_t const sensor_array[NUM_SENSORS] = {
    {TRIGGER_0, ECHO_RISING_PIN_0, ECHO_FALLING_PIN_0},
    {TRIGGER_1, ECHO_RISING_PIN_1, ECHO_FALLING_PIN_1},
    {TRIGGER_2, ECHO_RISING_PIN_2, ECHO_FALLING_PIN_2},
};


