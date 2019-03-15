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


static volatile unsigned start_times[NUM_SENSORS];
static volatile unsigned end_times[NUM_SENSORS];
static volatile unsigned int distances[NUM_SENSORS];

static unsigned int currSensorNum;

static void send_new_pulse(unsigned int pin);


bool risingEdge_0(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[0].echo_rising_pin)){
        start_times[0] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_0(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[0].echo_falling_pin)){
        end_times[0] = timer_get_ticks();
        distances[0] = (end_times[0] - start_times[0]) / 149;
        
        // disable sensor 0
        gpio_disable_event_detection(sensor_array[0].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_disable_event_detection(sensor_array[0].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // enable sensor 1 
        gpio_enable_event_detection(sensor_array[1].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_enable_event_detection(sensor_array[1].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // send out sensor 1 pulse 
        send_new_pulse(sensor_array[1].trigger);

        return true;
    }
    return false;
}


bool risingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[1].echo_rising_pin)){
        start_times[1] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[1].echo_falling_pin)){
        end_times[1] = timer_get_ticks();
        distances[1] = (end_times[1] - start_times[1]) / 149;
        
        // disable sensor 1
        gpio_disable_event_detection(sensor_array[1].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_disable_event_detection(sensor_array[1].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // enable sensor 2
        gpio_enable_event_detection(sensor_array[2].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_enable_event_detection(sensor_array[2].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // send out sensor 2 pulse 
        send_new_pulse(sensor_array[2].trigger);

        return true;
    }
    return false;
}

bool risingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[2].echo_rising_pin)){
        start_times[2] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(sensor_array[2].echo_falling_pin)){
        end_times[2] = timer_get_ticks();
        distances[2] = (end_times[2] - start_times[2]) / 149;

        // disable sensor 2
        gpio_disable_event_detection(sensor_array[2].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_disable_event_detection(sensor_array[2].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // enable sensor 0
        gpio_enable_event_detection(sensor_array[0].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
        gpio_enable_event_detection(sensor_array[0].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);

        // send out sensor 0 pulse 
        send_new_pulse(sensor_array[0].trigger);

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
   
   interrupts_attach_handler(risingEdge_0);
   interrupts_attach_handler(fallingEdge_0);


  // gpio_enable_event_detection(sensor_array[1].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
  // gpio_enable_event_detection(sensor_array[1].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_2);
   interrupts_attach_handler(fallingEdge_2);

//   gpio_enable_event_detection(sensor_array[2].echo_rising_pin, GPIO_DETECT_RISING_EDGE);
 //  gpio_enable_event_detection(sensor_array[2].echo_falling_pin, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_1);
   interrupts_attach_handler(fallingEdge_1);

   interrupts_enable_source(INTERRUPTS_GPIO3);

   interrupts_global_enable();
}

void ultraSound_init(void){

    interrupts_init();

    // change to a for loop over num sensors
    gpio_set_output(sensor_array[0].trigger);
  	gpio_set_input(sensor_array[0].echo_rising_pin);
  	gpio_set_pulldown(sensor_array[0].echo_rising_pin);
    gpio_set_input(sensor_array[0].echo_falling_pin);
    gpio_set_pulldown(sensor_array[0].echo_falling_pin);

  	gpio_set_output(sensor_array[1].trigger);
  	gpio_set_input(sensor_array[1].echo_rising_pin);
  	gpio_set_pulldown(sensor_array[1].echo_rising_pin);
    gpio_set_input(sensor_array[1].echo_falling_pin);
    gpio_set_pulldown(sensor_array[1].echo_falling_pin);

	gpio_set_output(sensor_array[2].trigger);
  	gpio_set_input(sensor_array[2].echo_rising_pin);
  	gpio_set_pulldown(sensor_array[2].echo_rising_pin);
    gpio_set_input(sensor_array[2].echo_falling_pin);
    gpio_set_pulldown(sensor_array[2].echo_falling_pin);

	timer_delay_ms(40);

      send_new_pulse(sensor_array[0].trigger);
   // send_new_pulse(sensor_array[1].trigger);
   // send_new_pulse(sensor_array[2].trigger);
}

unsigned int getDistance(int pos){
  // Out of bounds  
  if(pos > (NUM_SENSORS - 1)) return -1;

  return distances[pos];    
}


