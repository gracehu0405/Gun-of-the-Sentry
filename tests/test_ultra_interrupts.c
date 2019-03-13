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

const unsigned int TRIGGER = GPIO_PIN3;
const unsigned int ECHO_RISING_PIN = GPIO_PIN2;
const unsigned int ECHO_FALLING_PIN = GPIO_PIN4;
const unsigned int echo = GPIO_PIN2;
unsigned start_time = 0;
unsigned end_time = 0;
unsigned int distance = 0;

unsigned int get_distance(void) {
	// write hi for 10usec
	gpio_write(TRIGGER, 1);
	timer_delay_us(10);
	gpio_write(TRIGGER, 0);

	//unsigned start = timer_get_ticks();
	//timer_delay_us(149); // wait til device settles: 148 = time to go one inch
	while(!gpio_read(echo))
		;

    unsigned start = timer_get_ticks();
	unsigned end;
	while(gpio_read(echo) == 1)
		;
	end = timer_get_ticks();

	// ((340M/S / 2) * 39.37inch / meter) / 10^6 = inch/usec
	return (end - start) / 149;
}


bool risingEdge(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_RISING_PIN)){
        start_time = timer_get_ticks();
        return true;
    }
    return false;
}


bool fallingEdge(unsigned int pc){
    if(gpio_check_and_clear_event(ECHO_FALLING_PIN)){
        end_time = timer_get_ticks();
        distance = (end_time - start_time) / 149;
        
        gpio_write(TRIGGER, 1);
        timer_delay_us(10);
        gpio_write(TRIGGER, 0);

        return true;
    }
    return false;
}

void interrupts_init(void){
   gpio_enable_event_detection(ECHO_RISING_PIN, GPIO_DETECT_RISING_EDGE);
   gpio_enable_event_detection(ECHO_FALLING_PIN, GPIO_DETECT_FALLING_EDGE);
   
   interrupts_attach_handler(risingEdge);
   interrupts_attach_handler(fallingEdge);

   interrupts_enable_source(INTERRUPTS_GPIO3);

   interrupts_global_enable();
}



void main(void) {
    timer_init();
    gpio_init();
    uart_init();
    interrupts_init();

  	gpio_set_output(TRIGGER);
  	gpio_set_input(ECHO_RISING_PIN);
  	gpio_set_pulldown(ECHO_RISING_PIN);

    gpio_set_input(ECHO_FALLING_PIN);
    gpio_set_pulldown(ECHO_FALLING_PIN);
	timer_delay_ms(40);

    gpio_write(TRIGGER, 1);
    timer_delay_us(10);
    gpio_write(TRIGGER, 0);

  	while(1) {
		//unsigned int distance = get_distance();
       // if( distance <= 50 ) {
            printf("distance = %d inches\n", distance);
            timer_delay_ms(250);
       // }
	}
}
