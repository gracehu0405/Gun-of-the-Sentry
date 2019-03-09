#include "strings.h"

#define A 0x41
#define F 0x46
#define X 0x78
#define CONVERSION 0x30

//difference between letters and numbers
#define UPPERLETTER 0x7


int exponent(int base, int exp);
int validlength(const char *str, int min, int max);
int findvalue(const char *start, const char **endptr, int number, int length);

void *memset(void *s, int c, size_t n)
{
    for (int i=0; i<n; i++) {
      *(char*)(s+i) = (char)c;
    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
  for(int i=0; i<n; i++) {
    *(char*)(dst+i) = *(char*)(src+i);
  }
  return dst;
}

size_t strlen(const char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

int strcmp(const char *s1, const char *s2)
{
    while(*s1 != '\0' && *s2 != '\0') {
      if (*s1>*s2) {
        return 1;
      }
      if(*s1<*s2) {
        return -1;
      }
      if(*s1 == *s2) {
        s1++;
        s2++;
      }
    }
    if(*s1 != '\0') {
      return 1;
    }
    if(*s2 != '\0') {
      return -1;
    } else {
      return 0;
    }
}

size_t strlcat(char *dst, const char *src, size_t maxsize) //fix
{
    int total = strlen(dst)+strlen(src);
    if((strlen(dst)+1)<maxsize) {
      char* start = dst + strlen(dst);
      int amount = maxsize-strlen(dst)-1;
      memcpy(start, src, amount);
      start = dst + maxsize-1;
      *start = '\0';
    }
    return total;
}


//make constants for 0 and x
unsigned int strtonum(const char *str, const char **endptr) //handle lower and upper
{
    int length;

    //if Hexadecimal
    if(*str==CONVERSION && *(str+1) ==X) {
      const char *start = (str +2);
      length=validlength(start, 0, 'f');
      return findvalue(start, endptr, 16, length);
    }

    //if Decimal
    length= validlength(str, 0 , 9);
    if(length>=0) {
      if(str[0]>='0' && str[0]<='9') {
        return findvalue(str, endptr, 10, length);
      } else {

        *endptr = str;
        return 0;
      }
    } else {
      *endptr = str;

      return 0;
    }
}

//converts the string into an int with the number in hex or decimal
int findvalue(const char *start, const char **endptr, int number, int length) {
  int end=0;
  for(int i=0; i<=length; i++) {

      int current = (int)(start[i]);
      if(current>='0' && current <= '9') {
        current = current - '0';
      } else if(current>='A' && current <= 'F') {
        current = current - 'A' + 10;
      } else if(current>='a'&& current <= 'f') {
        current = current - 'a' + 10;

      }
      int exp = exponent(number, length-i)*current;
      end += exp;
  }

  //points to last valid in string
  *endptr = (start + length +1);
  return end;
}

//finds how many characters are valid before hitting an invalid character
int validlength(const char *str, int min, int max) {
  int counter = 0;
  for(int i=0; i<strlen(str); i++) {

    int current = (int)(str[i]);
    if(current>='0' && current <= '9') {
      counter++;
    } else if(max>='A') {
      if(current>='A' && current <= 'F') {
        counter++;
      } else if(current>='a'&& current <= 'f') {

        counter++;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  return (counter-1); //returns length-1, the amount it needs to move pointer
}

//executes exponential function with base that is passed in
int exponent(int base, int exp) {
  int mult = base;
  if(exp==0) {
    return 1;
  } else {
    for (int i=0; i<(exp-1); i++) {
      base *= mult;
    }
  }
  return base;
}
