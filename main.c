#include "ultrasound.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "gun.h"



void main(void){
    uart_init();
    timer_init();
    gpio_init();
    ultraSound_init();

	while(1) {
            printf("distance_0 = %d inches, distance_1 = %d inches, distance_2 = %d inches\n", getDistance(0), getDistance(1), getDistance(2));
           // printf("distance_0 = %d inches\n", distance_0);
           // printf("distance_1 = %d inches\n", distance_1);
            timer_delay_ms(250);
       
	}
}
