#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "pi.h"
#include "gpio.h"
#include "gpioextra.h"


void main(void) 
{
    uart_init();
    
    // set the pin to input
    gpio_set_input(GPIO_PIN26);

   // gpio_set_pullup(GPIO_PIN26);

    while(1){
      printf("%d\n", gpio_read(GPIO_PIN26));
    }

}
