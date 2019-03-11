#include "printf.h"
#include "uart.h"

//need 3 files
    //motion detection (have function that returns the array with the motion values: 0 or 1 for each sensor)
    //fire bullet
    //rotate gun
static int currentPosition;
static int numSensors = 5;


void main(void)
{
  uart_init();
  printf("Hello, world!\n");
  /*int array[numSensors];
  while(1) {

    //getMotionArray returns the array with the values from the motion sensors
    array = getMotionArray;
    for(int i = 0; i<numSensors; i++) {
      if(array[i]==1) {
        //findMoveNum is a calculation that determines where to shift from current Position
        int moveNum = findMoveNum(i, currentPosition);
        moveGun(moveNum); //moves gun 60 degrees * moveNum
        fireGun()

        currentPosition = i;
      }
    }
    //timer_delay(5); //whatever delay between shots so it can reset?
  }*/

}
