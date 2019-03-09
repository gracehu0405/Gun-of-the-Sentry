#include "printf.h"
#include <stdarg.h>
#include "strings.h"
#include "uart.h"

#define MAX_OUTPUT_LEN 1024


static int max(int a, int b);
static int min(int a, int b);
static void reverse(char *buf, char *tempBuf, size_t bufsize, int counter, int val);
int exponent(int base, int exp);



int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width)
{
    if(bufsize!=0) {
      int buffersize = max(0x42, bufsize);
      char tempBuf[buffersize];

      //store in buffer
      unsigned int tempNum = val;
      unsigned int current;
      int counter=0;


      if(tempNum==0) {
        tempBuf[0]='0';
        counter++;
      } else {

        //Counts number before decimal or hex
        while(tempNum>0) {
          if(counter==buffersize) {
            break;
          } else {
          current = tempNum%base; //isolates rightmost digit
            if(current<=9) {
              (tempBuf[counter])=(current+ '0'); //converts digits
            } if (current>9 && current<=16){
              (tempBuf[counter])=(current - 10 + 'a'); //converts a-f
            }
            tempNum = tempNum/base; //erases rightmost digit
            counter++;
          }

        }
      }

      //pad with zeros
      while(counter<min_width){
        (tempBuf[counter])='0';
        counter++;
      }

      //reverse
      reverse(buf, tempBuf, bufsize, counter, val);
      return counter;
    }

    //returns min width or size of final product if bigger than minwidth
    return 0;
}

void reverse(char *buf, char *tempBuf, size_t bufsize, int counter, int val) {
  int length = min(bufsize-1, counter);
  for(int i=0; i<length; i++) {
    buf[i]=tempBuf[counter-i-1];
  }
  (buf[length])='\0'; //null terminates
}

int min(int a, int b) {
  if(a<=b) {
    return a;
  } else {
    return b;
  }
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width)
{

  unsigned int currentReturn;


  //handles negative numbers
  if(val<0) {
    currentReturn = (unsigned_to_base((buf+1), (bufsize-1), (-val), base, (min_width-1)));
    currentReturn++;
    buf[0] = '-';
  } else {

    //handles positive numbers
    currentReturn = (unsigned_to_base(buf, bufsize, val, base, min_width));
  }
  return currentReturn;
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
  //initalize temp buffer
  int buffersize = 0x42;
  char tempBuf[buffersize]; //creates buffer to store current string

  //initalize indices and character pointer
  int bindex = 0; //tempBuf index
  int findex = 0; //format index
  char* ch;
  *tempBuf='\0';

  while(findex < strlen(format)) {
    char current = format[findex];
    if(current == '%') {
      int width = 0;
      findex++;
      char next = format[findex];

    if(next=='0') {
      findex++;
      int indexCounter = 0;
      char currentNum =format[findex];
        while(currentNum>='0' && currentNum<='9') {
          indexCounter++;
          findex++;
          currentNum=format[findex];
        }
        //indexCounter++;
        while(indexCounter>0) {
          currentNum = format[findex-indexCounter];

          width += (int)(currentNum - '0') * exponent(10, (indexCounter-1));
          indexCounter--;
        }
        next = format[findex];
    }

      //Char character code
      if (next=='c') {
        char current = (char)va_arg(args, int);
        tempBuf[bindex]=current;
        bindex++;
      }

      //% character code
      if (next=='%') {
        char current = '%';
        tempBuf[bindex]=current;
        bindex++;
      }

      //String character code
      if (next=='s') {
        ch = (char*)va_arg(args, int);
        int length = strlen(ch);
        memcpy(tempBuf+bindex, ch, buffersize);
        bindex = bindex + length;
      }

      //Pointer character code
      if(next=='p') {
        unsigned int *argument= (unsigned int *)va_arg(args, int);
        tempBuf[bindex]='0';
        bindex++;
        tempBuf[bindex]='x';
        bindex++;
        unsigned_to_base(tempBuf+bindex, buffersize, (unsigned int)argument, 16, 8);
        bindex = bindex+ 8;
      }

      //Decimal character code
      if(next =='d') {
        int decimal = va_arg(args, int);
        int length = signed_to_base(tempBuf+bindex, buffersize, decimal, 10, width);
        if(decimal<0) {
          bindex++;
        }
        bindex = bindex +length;
      }

      //Hexadecimal character code
      if(next =='x') {
        unsigned int number = va_arg(args, int);
        int length = signed_to_base(tempBuf+bindex, buffersize, number, 16, width);
        bindex = bindex +length;
      }
    } else {
        tempBuf[bindex]= format[findex];
        bindex++;
    }
    findex++;
  }

  //null terminate
  tempBuf[bindex]='\0';

  //loop through and store tempBuf into buf
  for(int i=0; i<bufsize-1; i++){
    buf[i] = tempBuf[i];
  }
  buf[bufsize-1]='\0'; //stores null pointer in last index of array
  return bindex;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  int size = vsnprintf(buf, bufsize, format, ap);
  va_end(ap);
  return size;
}

int printf(const char *format, ...)

{
    va_list ap;
    va_start(ap, format);
    char buf[MAX_OUTPUT_LEN];
    int size = vsnprintf(buf, MAX_OUTPUT_LEN, format, ap);
    va_end(ap);
    for(int i=0; i<size; i++) {
      uart_putchar(buf[i]);
    }
    return size;
}

int max(int a, int b) {
  if(a>=b) {
    return a;
  } else {
    return b;
  }
}
