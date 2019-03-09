#include "backtrace.h"
#include "printf.h"
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))



int backtrace (frame_t f[], int max_frames)
{
    void *cur_fp;
    __asm__("mov %0, fp" : "=r" (cur_fp));

    //initializes counters
    int index = 0;
    int first = 0;

    while(*((unsigned int *)cur_fp-3)!=0 && index<max_frames) { //how to check if reached cstart

        unsigned int *savedfp = (unsigned int *)cur_fp-3;
        unsigned int *pc = (unsigned int *)*savedfp;
        pc=(unsigned int *)*pc;

        //Find resume address, points to lr
        unsigned int* resume = (unsigned int *)cur_fp -1;

        cur_fp = (unsigned int *)*savedfp;
        unsigned int* start = pc-3;
        char* name;

        //Finds offset->fix!!!
        unsigned int offset = (*resume-(unsigned int)start);

        //Finds name
        unsigned int fnName = *(start-1);

        //Left shifts to see if name starts with 0xff, then points to beginning of fn name
        if((fnName>>24)== 0xff) {
          unsigned int length = fnName & ~0xff000000;
          length = roundup(length, 4);
          name = (char*) start-length-4;
        } else {
          name = "???";
        }

        //if (first>0) { //if not the first frame, adds to frame_t array
          f[index].name = name;
          f[index].resume_addr = *resume;
          f[index].resume_offset = offset;
          index++;
      //}
      first ++;
    }
    return index;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
