#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "pi.h"
#include "gpio.h"


void main(void) 
{
    uart_init();
    
    // set the pin to output
    gpio_set_output(GPIO_PIN26);

    gpio_write(GPIO_PIN26, 1);

    printf("Starting timer delay\n");

    timer_delay(5);
    printf("Done with timer delay\n");

    gpio_write(GPIO_PIN26, 0);
    while(1){
      printf("running\n");
    }

}
