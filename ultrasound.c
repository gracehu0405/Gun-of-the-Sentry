/*
 * HC-SR04
   http://www.modmypi.com/blog/hc-sr04-ultrasonic-range-sensor-on-the-raspberry-pi
   IMPORTANT. The sensor output signal (ECHO) on the HC-SR04 is rated
   at 5V. However, the input pin on the Raspberry Pi GPIO is rated at
   3.3V. Sending a 5V signal into that unprotected 3.3V input port could
   damage your GPIO pins, which is something we want to avoid! Well
   need to use a small voltage divider circuit, consisting of two
   resistors, to lower the sensor output voltage to something our
   Raspberry Pi can handle.
   http://www.bytecreation.com/blog/2013/10/13/raspberry-pi-ultrasonic-sensor-hc-sr04
 */
#include "timer.h"
#include "gpio.h"
#include "gpioextra.h"
#include "uart.h"
#include "printf.h"
#include "interrupts.h"
#include "ultrasound.h"


static volatile unsigned start_times[NUM_SENSORS];
static volatile unsigned end_times[NUM_SENSORS];
static volatile unsigned int distances[NUM_SENSORS];

/*unsigned start_time_0 = 0;
unsigned end_time_0 = 0;
unsigned int distance_0 = 0;

unsigned start_time_1 = 0;
unsigned end_time_1 = 0;
unsigned int distance_1 = 0;

unsigned start_time_2 = 0;
unsigned end_time_2 = 0;
unsigned int distance_2 = 0;*/

static void send_new_pulse(unsigned int pin);

bool risingEdge_0(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_0)){
        start_times[0] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_0(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_0)){
        end_times[0] = timer_get_ticks();
        distances[0] = (end_times[0] - start_times[0]) / 149;
        
        send_new_pulse(TRIGGER_0);

        return true;
    }
    return false;
}


bool risingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_1)){
        start_times[1] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_1)){
        end_times[1] = timer_get_ticks();
        distances[1] = (end_times[1] - start_times[1]) / 149;
        
        send_new_pulse(TRIGGER_1);

        return true;
    }
    return false;
}

bool risingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_2)){
        start_times[2] = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_2)){
        end_times[2] = timer_get_ticks();
        distances[2] = (end_times[2] - start_times[2]) / 149;
        
        send_new_pulse(TRIGGER_2);

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
   gpio_enable_event_detection(ECHO_RISING_PIN_0, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_0, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_0);
   interrupts_attach_handler(fallingEdge_0);


   gpio_enable_event_detection(ECHO_RISING_PIN_1, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_1, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_2);
   interrupts_attach_handler(fallingEdge_2);

   gpio_enable_event_detection(ECHO_RISING_PIN_2, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_2, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_1);
   interrupts_attach_handler(fallingEdge_1);

   interrupts_enable_source(INTERRUPTS_GPIO3);

   interrupts_global_enable();
}

void ultraSound_init(void){

    interrupts_init();

    gpio_set_output(TRIGGER_0);
  	gpio_set_input(ECHO_RISING_PIN_0);
  	gpio_set_pulldown(ECHO_RISING_PIN_0);
    gpio_set_input(ECHO_FALLING_PIN_0);
    gpio_set_pulldown(ECHO_FALLING_PIN_0);

  	gpio_set_output(TRIGGER_1);
  	gpio_set_input(ECHO_RISING_PIN_1);
  	gpio_set_pulldown(ECHO_RISING_PIN_1);
    gpio_set_input(ECHO_FALLING_PIN_1);
    gpio_set_pulldown(ECHO_FALLING_PIN_1);

	gpio_set_output(TRIGGER_2);
  	gpio_set_input(ECHO_RISING_PIN_2);
  	gpio_set_pulldown(ECHO_RISING_PIN_2);
    gpio_set_input(ECHO_FALLING_PIN_2);
    gpio_set_pulldown(ECHO_FALLING_PIN_2);

	timer_delay_ms(40);

    send_new_pulse(TRIGGER_0);
    send_new_pulse(TRIGGER_1);
    send_new_pulse(TRIGGER_2);
}

unsigned int getDistance(int pos){
  // Out of bounds  
  if(pos > (NUM_SENSORS - 1)) return -1;

  return distances[pos];    
}


