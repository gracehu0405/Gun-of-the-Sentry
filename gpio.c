#include "gpio.h"

volatile static unsigned int *SET0  =  (volatile unsigned int *)0x2020001c;
volatile static unsigned int *SET1  =  (volatile unsigned int *)0x20200020;
volatile static unsigned int *CLR0  =  (volatile unsigned int *)0x20200028;
volatile static unsigned int *CLR1  =  (volatile unsigned int *)0x2020002C;
volatile static unsigned int *LEV0  =  (volatile unsigned int *)0x20200034;
volatile static unsigned int *LEV1  =  (volatile unsigned int *)0x20200038;
volatile static unsigned int *FSEL0 =  (volatile unsigned int *)0x20200000;

void gpio_init(void) {
}

/* Finds FSEL register associated with pin.  Masks the FSEL
register exposing the 3 bits corresponding with the pin to preserve
functions of other pins. Stores new function value.  */
void gpio_set_function(unsigned int pin, unsigned int function) {
  if (pin<= 53 && pin >=0) {
    if (function>=0 && function <=7) {
      volatile unsigned int *p = (volatile unsigned int *)(FSEL0 + pin/10);
      unsigned int shift = (pin%10)*3;
      unsigned int current = *p;
      int mask = (~(7<<shift) & current);
      mask = mask | (function << shift);
      *p = mask;
    }
  }
}

/* Finds FSEL register associated with pin.  Right shifts to find
value at that location.  Ands with 7 to get function value and
returns it.  */
unsigned int gpio_get_function(unsigned int pin) {
  if (pin<= 53 && pin >=0) {
      volatile unsigned int *p = (volatile unsigned int *)(FSEL0 + pin/10);
      unsigned int shift = (pin%10)*3;
      unsigned int getnum = *p >> shift;
      getnum = getnum & 7;
      return getnum;
    }
    return -1;
}

void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, 0);
}

void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, 1);
}

void gpio_write(unsigned int pin, unsigned int value) {
  if (value == 1){
    if (pin >= 0 && pin <= 31) {
      *SET0 = 1<<pin;
    }
    if (pin>31 && pin <= 53) {
      *SET1 = 1<<(pin-32);
    }
  }
  if (value == 0){
    if (pin >= 0 && pin <= 31) {
      *CLR0 = 1<<pin;
    }
    if (pin>31 && pin <= 53) {
      *CLR1 = 1<<(pin-32);
    }
  }
}

unsigned int gpio_read(unsigned int pin) {
    if (pin >= 0 && pin <= 31) {
        unsigned int lev0num = *LEV0>>pin;
        lev0num = lev0num & 1;
        if (lev0num == 0 || lev0num ==1) {
          return lev0num;
        }
      }
    if (pin > 31 && pin <= 53) {
      unsigned int lev1num = *LEV1>>(pin-32);
      lev1num = lev1num & 1;
      if (lev1num == 0 || lev1num ==1) {
        return lev1num;
      }
    }
  return -1;
}
