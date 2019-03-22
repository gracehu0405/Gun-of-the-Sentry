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

const unsigned int TRIGGER_1 = GPIO_PIN3;
const unsigned int ECHO_RISING_PIN_1 = GPIO_PIN2;
const unsigned int ECHO_FALLING_PIN_1 = GPIO_PIN4;

unsigned start_time_1 = 0;
unsigned end_time_1 = 0;
unsigned int distance_1 = 0;

const unsigned int TRIGGER_2 = GPIO_PIN17;
const unsigned int ECHO_RISING_PIN_2 = GPIO_PIN27;
const unsigned int ECHO_FALLING_PIN_2 = GPIO_PIN22;

unsigned start_time_2 = 0;
unsigned end_time_2 = 0;
unsigned int distance_2 = 0;

const unsigned int TRIGGER_3 = GPIO_PIN10;
const unsigned int ECHO_RISING_PIN_3 = GPIO_PIN9;
const unsigned int ECHO_FALLING_PIN_3 = GPIO_PIN11;

unsigned start_time_3 = 0;
unsigned end_time_3 = 0;
unsigned int distance_3 = 0;



void send_new_pulse(unsigned int pin);

bool risingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_1)){
        start_time_1 = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_1(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_1)){
        end_time_1 = timer_get_ticks();
        distance_1 = (end_time_1 - start_time_1) / 149;
        
        send_new_pulse(TRIGGER_1);

        return true;
    }
    return false;
}



bool risingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_2)){
        start_time_2 = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_2(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_2)){
        end_time_2 = timer_get_ticks();
        distance_2 = (end_time_2 - start_time_2) / 149;
        
        send_new_pulse(TRIGGER_2);

        return true;
    }
    return false;
}

bool risingEdge_3(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN_3)){
        start_time_3 = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge_3(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN_3)){
        end_time_3 = timer_get_ticks();
        distance_3 = (end_time_3 - start_time_3) / 149;
        
        send_new_pulse(TRIGGER_3);

        return true;
    }
    return false;
}

void send_new_pulse(unsigned int pin){
    gpio_write(pin, 1);
    timer_delay_us(10);
    gpio_write(pin, 0);
}

void interrupts_init(void){
   gpio_enable_event_detection(ECHO_RISING_PIN_1, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_1, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_1);
   interrupts_attach_handler(fallingEdge_1);


   gpio_enable_event_detection(ECHO_RISING_PIN_2, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_2, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_3);
   interrupts_attach_handler(fallingEdge_3);

   gpio_enable_event_detection(ECHO_RISING_PIN_3, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN_3, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge_2);
   interrupts_attach_handler(fallingEdge_2);

   interrupts_enable_source(INTERRUPTS_GPIO3);

   interrupts_global_enable();
}



void main(void) {
    timer_init();
    gpio_init();
    uart_init();
    interrupts_init();

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

	gpio_set_output(TRIGGER_3);
  	gpio_set_input(ECHO_RISING_PIN_3);
  	gpio_set_pulldown(ECHO_RISING_PIN_3);
    gpio_set_input(ECHO_FALLING_PIN_3);
    gpio_set_pulldown(ECHO_FALLING_PIN_3);

	timer_delay_ms(40);

    send_new_pulse(TRIGGER_1);
    send_new_pulse(TRIGGER_2);
    send_new_pulse(TRIGGER_3);

  	while(1) {
            printf("distance_1 = %d inches, distance_2 = %d inches, distance_3 = %d inches\n", distance_1, distance_2, distance_3);
           // printf("distance_1 = %d inches\n", distance_1);
           // printf("distance_2 = %d inches\n", distance_2);
            timer_delay_ms(250);
       
	}
}
