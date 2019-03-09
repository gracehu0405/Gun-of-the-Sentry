
#include "../gpio.h"
#include "../timer.h"

//define numbers
#define DIGIT0 0b00111111
#define DIGIT1 0b00000110
#define DIGIT2 0b01011011
#define DIGIT3 0b01001111
#define DIGIT4 0b01100110
#define DIGIT5 0b01101101
#define DIGIT6 0b01111101
#define DIGIT7 0b00000111
#define DIGIT8 0b01111111
#define DIGIT9 0b01100111

//define letters
#define A 0b01110111
#define B 0b01111111
#define C 0b00111001
#define D 0b00111111
#define E 0b01111001
#define F 0b01110001

static void draw_digit(unsigned int transistornum, unsigned int digit, unsigned int delay);
static unsigned int debounce(unsigned int1, unsigned int2);

void main(void)
{
      unsigned int digits[16]={DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5,
      DIGIT6, DIGIT7, DIGIT8, DIGIT9, A, B, C, D, E, F};

      // Sets pins for output that turn on segments and digits
      for (unsigned int i = GPIO_PIN10; i<=GPIO_PIN13; i++) {
        gpio_set_output(i);
      }
      for (unsigned int i= GPIO_PIN20; i<=GPIO_PIN27; i++) {
        gpio_set_output(i);
      }

      // Sets pins for input that correspond with buttons.
      gpio_set_input(GPIO_PIN2);
      gpio_set_input(GPIO_PIN3);

      //initializes variables to set the time
      unsigned int added = 0;
      unsigned int setdelay = 25000;
      unsigned int currentdigit[4]= {0, 0, 0, 0};
      unsigned int currentgpio[4] = {GPIO_PIN10, GPIO_PIN11, GPIO_PIN12, GPIO_PIN13};
      unsigned int index=0;

      for(;;) {
        /* If blue button is pushed, selects which digit to change. Labels current
         digit with "E" for edit */
        if(debounce(1, 0)==1) {
          index++;
          if(index==4) {
            index = 0;
          }
          draw_digit(currentgpio[index], E, setdelay);
        }

        /* If red button is selected, increments that digit by 1. */
        if(debounce(0, 1)==1) {
              currentdigit[index] = (currentdigit[index])+1;
              if (index==0 || index == 2) {
                currentdigit[index]= (currentdigit[index])%6;
              }
              if (index==1 || index == 3) {
                currentdigit[index]= (currentdigit[index])%10;
              }
              unsigned int digitnum = currentdigit[index];
              draw_digit(currentgpio[index], digits[digitnum], setdelay);
      }

        // totals up the seconds added from the user setting the time
        added = (currentdigit[0]*600)+(currentdigit[1]*60)+(currentdigit[2]*10)+currentdigit[3];

        /* If both the red and blue buttons are pushed at the same time, exits
        the loop and begins the clock. */
        if(debounce(0, 0)==1) {
          break;
        }
    }

      // Loops through each digit and turns on and off one at a time.
      unsigned int seconds = added;

      unsigned int start = timer_get_ticks();
      for(;;) {
          unsigned int delay = 2500;
          //checkchanges(added);
          for(unsigned int i=GPIO_PIN13; i>=GPIO_PIN10; i--) {
            if(i==GPIO_PIN13) {
              draw_digit(i, digits[(seconds%10)], delay);
            }
            if(i==GPIO_PIN12) {
              draw_digit(i, digits[(seconds/10)%6], delay);
            }
            if(i==GPIO_PIN11) {
              draw_digit(i, digits[(seconds/60)%10], delay);
            }
            if(i==GPIO_PIN10) {
              draw_digit(i, digits[(seconds/600)%6], delay);
            }
            seconds = ((timer_get_ticks()-start)/1000000) + added;
          }
      }
}

//  Makes sure each time the button is pressed, only one action is recorded.
static unsigned int debounce(unsigned int1, unsigned int2) {
  unsigned int current = 0;
  for (int i=0; i<500; i++) {
    if(gpio_read(2)==int1 && gpio_read(3)==int2) {
      current = 1;
      timer_delay_us(25000);
    }
  }

  return current;
}

/* Turns on transistor associated with current digit.  Cycles through
each segment of the digit and turns on segment if indicated. Delays
for 2500 us and then turns off digit. */
static void draw_digit(unsigned int transistornum, unsigned int digit, unsigned int delay) {
  gpio_write(transistornum, 1);
  for(unsigned int i=GPIO_PIN20; i<GPIO_PIN28; i++) {
    unsigned int current = digit & 1;
    if (current ==1) {
      gpio_write(i, 1);
    } else {
      gpio_write(i, 0);
    }
    digit = digit >> 1;
  }
  timer_delay_us(delay);
  gpio_write(transistornum, 0);
}
