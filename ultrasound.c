/*
 * File: ultrasound.c
 * -------------------
 *
 * Authors: Michael Oduoza, Steffi Andersen, Grace Hu
 * Date: Thursday March 14th, 2019
 *
 * Implementation of standard functions defined in ultrasound.h for
 * initializing and reading the output of HC-SRO4 sensor modules.
 * Contains an interrupts mechanism that allows the sensors to periodically
 * keep track of the range of objects in their vicinity.
 */
#include "timer.h"
#include "gpio.h"
#include "gpioextra.h"
#include "uart.h"
#include "printf.h"
#include "interrupts.h"
#include "ultrasound.h"
#include "sensormap.h"
#include "armtimer.h"


static volatile unsigned start_times[NUM_SENSORS];
static volatile unsigned end_times[NUM_SENSORS];
static volatile unsigned int distances[NUM_SENSORS];

static unsigned int currSensorNum = 0;

static void send_new_pulse(unsigned int pin);


bool risingEdge(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[currSensorNum].echo_rising_pin)){
        start_times[currSensorNum] = timer_get_ticks();

        // start a new countdown
        armtimer_init(TIMEOUT);
        armtimer_enable_interrupts();
        armtimer_enable();

        return true;
    }
    return false;
}


bool fallingEdge(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[currSensorNum].echo_falling_pin)){

        // Stop the timer
        armtimer_disable_interrupts();
        armtimer_disable();
        armtimer_clear_event();

        end_times[currSensorNum] = timer_get_ticks();
        distances[currSensorNum] = (end_times[currSensorNum] - start_times[currSensorNum]) / INCH_CONVERSION;

        // disable the current sensor
        gpio_disable_event_detection(sensor_array[currSensorNum].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_disable_event_detection(sensor_array[currSensorNum].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // Advance to the next sensor
        currSensorNum = ((currSensorNum + 1) % NUM_SENSORS);

        // enable the next sensor
        gpio_enable_event_detection(sensor_array[currSensorNum].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_enable_event_detection(sensor_array[currSensorNum].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // send out the next sensor pulse
        send_new_pulse(sensor_array[currSensorNum].trigger);

        return true;
    }
    return false;
}


bool timeout_handler(unsigned int pc){
    if(armtimer_check_and_clear_interrupt()){

        // Stop the timer
        armtimer_disable_interrupts();
        armtimer_disable();

        // Set the distance to be the maximum
        distances[currSensorNum] = MAX_RANGE;

        // disable the current sensor
        gpio_disable_event_detection(sensor_array[currSensorNum].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_disable_event_detection(sensor_array[currSensorNum].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // clear any events
        gpio_clear_event(sensor_array[currSensorNum].echo_rising_pin);
        gpio_clear_event(sensor_array[currSensorNum].echo_falling_pin);

        // Advance to the next sensor
        currSensorNum = ((currSensorNum + 1) % NUM_SENSORS);

        // enable the next sensor
        gpio_enable_event_detection(sensor_array[currSensorNum].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_enable_event_detection(sensor_array[currSensorNum].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // send out the next sensor pulse
        send_new_pulse(sensor_array[currSensorNum].trigger);

        return true;
    }
    return false;
}

static void send_new_pulse(unsigned int pin){
    gpio_write(pin, 1);
    timer_delay_us(10);
    gpio_write(pin, 0);
}

static void interrupts_init(void){
    gpio_enable_event_detection(sensor_array[0].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
    gpio_enable_event_detection(sensor_array[0].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

    interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ);

    interrupts_attach_handler(risingEdge);
    interrupts_attach_handler(fallingEdge);
    interrupts_attach_handler(timeout_handler);

    interrupts_enable_source(INTERRUPTS_GPIO3);

    interrupts_global_enable();
}

void ultraSound_init(void){

    interrupts_init();

    for(int i = 0; i < NUM_SENSORS; i++){
        gpio_set_output(sensor_array[i].trigger);
        gpio_set_input(sensor_array[i].echo_rising_pin);
        gpio_set_pulldown(sensor_array[i].echo_rising_pin);
        gpio_set_input(sensor_array[i].echo_falling_pin);
        gpio_set_pulldown(sensor_array[i].echo_falling_pin);
    }

    timer_delay_ms(40);

    // send the very first pulse
    send_new_pulse(sensor_array[0].trigger);

}

unsigned int getDistance(int pos){
    // Out of bounds
    if(pos > (NUM_SENSORS - 1)) return -1;

    return distances[pos];
}

int smallestDistance(void) {
  int curDistance = MAX_RANGE;
  int index = -1;
  for(int i = 0; i < NUM_SENSORS; i++){
    if(getDistance(i)<= curDistance) {
      curDistance = getDistance(i);
      index = i;
    }
  }
  return index;
}
