/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by tacking on the requested
 * space to the end of the heap thus far.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"
#include "backtrace.h"

extern int __bss_end__;

#define STACK_START 0x8000000
#define STACK_SIZE  0x1000000
#define STACK_END ((char *)STACK_START - STACK_SIZE)
#define REDZONE 0x7e7e7e7e



/*
 * The pool of memory used for the heap starts at the top end of the
 * data section and extends to the bottom end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates stack end assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * `heap_end` tracks location of next available address in heap
 * `heap_max` tracks location where heap is exhausted
 *
 */
static void *heap_end = &__bss_end__;
static void *heap_max = STACK_END;


struct header{
  unsigned int size;
  unsigned int status;
  frame_t f[3];
};

enum { IN_USE = 0, FREE = 1}; //swapped


// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

static void freeHeader(struct header* hdr, unsigned int size, unsigned int previous);
static void printMemory (unsigned int* address, unsigned int* redzone1, unsigned int* redzone2);

void *malloc (size_t nbytes) //do I need to use memcpy??
{
  if(nbytes==0) {
    return NULL;
  }
  nbytes = roundup(nbytes, 8);

    //Creates temp pointer to start of heap
    void *temp = &__bss_end__;

      //Iterates through heap to find first memory location with enough space
      while(temp!= heap_end) {
        //
        if (((char *)temp + nbytes + sizeof(struct header) + (2*sizeof(REDZONE))) < (char *)heap_max) {
          struct header *hdr = (struct header *)temp;
          void* redzonetemp = temp;

          //If current header in use, moves temp pointer to next header
          if(hdr->status == IN_USE) {
            temp = (char*)temp + hdr->size + sizeof(struct header) + (2*sizeof(REDZONE));

            //If header is free, checks if there is enough space and sets to in use
          } else if(hdr->status == FREE) {
            int total = hdr->size + (2*sizeof(REDZONE));
            if(nbytes<=total) {
              redzonetemp = (char*)redzonetemp+ sizeof(struct header);
              *(unsigned int*)temp = REDZONE;
              redzonetemp = (char*)redzonetemp + nbytes + (sizeof(REDZONE));
              *(unsigned int*)redzonetemp = REDZONE;

              hdr->size = nbytes;
              hdr->status = IN_USE;

              //If space left over, creates header with how much free space is left
              if(total!=nbytes) {
                freeHeader(temp, nbytes, total);
              }
              void* returnVal = (char*)hdr + sizeof(struct header) + sizeof(REDZONE);
              backtrace(hdr->f, 3);
              return returnVal;

              //If not enough space, moves on to next header
            } else {
              temp = (char*)temp + hdr->size + sizeof(struct header)+ (2*sizeof(REDZONE));
            }
          }
        } else {
            return NULL;
        }
      }

      //creates header where temp pointer is
      struct header *hdr = (struct header *)temp;
      backtrace(hdr->f, 3);
      temp = (char*)temp+ sizeof(struct header);
      char* returnVal = (char*)temp+sizeof(REDZONE);
      *(unsigned int*)temp = REDZONE;
      temp = (char*)temp + nbytes + sizeof(REDZONE);
      *(unsigned int*)temp = REDZONE;
      hdr->size= nbytes;
      hdr->status= IN_USE;
      heap_end = (char *)temp + sizeof(REDZONE);
      return returnVal;
}

void free (void *ptr) //work on coaless
{
  if(ptr!=NULL) {
    //Sets the current header to free
    char* val = (char*)ptr - sizeof(struct header) - sizeof(REDZONE);
    struct header *start = (struct header*) val;
    val = (char*)ptr-sizeof(REDZONE);
    unsigned int* redzonePtr1 = (unsigned int *)val;
    val = (char*)ptr+start->size;
    unsigned int* redzonePtr2 = (unsigned int *)val;

    //checks if redzones are intact
    if(*redzonePtr1==REDZONE&&*redzonePtr2==REDZONE) {
      start->status = FREE;
    } else {
      printMemory(ptr, redzonePtr1, redzonePtr2);
    }

      //If there is another free header after, coalesces
      while(((char*)start+sizeof(struct header) + start->size) + (2*sizeof(REDZONE)) <= (char*)heap_end) { //check
        void *value = (char *)start + start->size + sizeof(struct header)+ (2*sizeof(REDZONE));
        struct header *next = (struct header *)value;
        if(next->status==FREE) {
          int sizeAdded = next->size + sizeof(struct header);
          start->size = start->size + sizeAdded;
          value = (char*)start + start->size + sizeof(REDZONE);
          *(unsigned int*)value = REDZONE;
        } else {
          break;
        }
      }
    }
}

void *realloc (void *orig_ptr, size_t new_size) //point to header or payload
{
    new_size = roundup(new_size, 8);
    void* new_ptr = orig_ptr;
    struct header *hdr = (struct header*)new_ptr - 1 - sizeof(REDZONE);

//Edge case for null ptr
    if(new_ptr==NULL) {
      new_ptr = malloc(new_size);
      return new_ptr;
    }

    //Edge case for size of zero
    if(new_size==0) {
      hdr->size = new_size;
      hdr->status = FREE;
    }

    //if new size is smaller than current size, changes size in place
    if(new_size<=(hdr->size)) {
      int previous = hdr->size;
      hdr->size = new_size;
      hdr->status = IN_USE;
      new_ptr= (char*) new_ptr + sizeof(struct header) + sizeof(REDZONE);
      *(unsigned int*)new_ptr = REDZONE;

      //add free header
      if(new_size!=previous) {
        freeHeader(hdr, (int)new_size, previous);
      }

      //if new size is larger than current size and next header is free, checks if there is space
    } else if (new_size>(hdr->size)){ //check
      if((char*)hdr + new_size + sizeof(struct header) + (2*sizeof(REDZONE))<= (char*)heap_end) {
          void *value = (char *)new_ptr + hdr->size + (2*sizeof(REDZONE));
          struct header *next = (struct header *)value;
          unsigned int rest = new_size - hdr->size;

          //If free header has space for the newsize, changes size of original header
          if(next->status==FREE && next->size>=rest) { //Fix
            int total = next->size + sizeof(struct header)+ hdr->size + + (2*sizeof(REDZONE));
            hdr->size = new_size;
            //
            new_ptr = (char*)new_ptr + sizeof(struct header)+ new_size + sizeof(REDZONE);
            *(unsigned int*)new_ptr = REDZONE;

            //If space left over, creates header with how much free space is left
            freeHeader(hdr, (int)new_size, total);
            return new_ptr;
        }
      }

      //If none of the above cases apply, creates a new header at heap_end
      new_ptr = malloc(new_size);
      if (!new_ptr) return NULL; //what does this mean?
      memcpy(new_ptr, orig_ptr, new_size);
      free(orig_ptr);
      }
      return new_ptr;
}

//hdr is start of first header, size is new size of header, previous is total space
static void freeHeader(struct header* hdr, unsigned int size, unsigned int previous) {
  void* temp = (char*)hdr + size + sizeof(struct header) + (2*sizeof(REDZONE));
  struct header *new = (struct header *)temp;
  new->size = previous - size - sizeof(struct header) - (2*sizeof(REDZONE));
  new->status = FREE;
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("----------  END DUMP (%s) ----------\n", label);
}

void memory_report (void)
{
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
}

static void printMemory (unsigned int* address, unsigned int* redzone1, unsigned int* redzone2) {
  memory_report();
  printf("Attempt to free address %x that has damaged red zone(s): [%x] [%x] \n", (unsigned int)address, *redzone1, *redzone2);
  char* returnVal = (char*)address - sizeof(struct header) - sizeof(REDZONE);
  struct header *hdr = (struct header *) returnVal;
  print_frames(hdr->f, 3);
}
