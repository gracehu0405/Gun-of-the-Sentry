#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "../nameless.h"
#include "rand.h"
#include <stdint.h>
#include "strings.h"
#include "uart.h"
#define REDZONE 0x7e7e7e7e

void main(void);
void heap_dump(const char *label);  // Debugging output from malloc module

struct header{
  unsigned int size;
  unsigned int status;
  frame_t f[3];
};

static void test_backtrace_simple(void)
{
 frame_t f[2];
 int frames_filled = backtrace(f, 2);

 assert(frames_filled == 2);
 assert(strcmp(f[0].name, "test_backtrace_simple") == 0);
 assert(f[0].resume_addr == (uintptr_t)test_backtrace_simple + f[0].resume_offset);
 assert(strcmp(f[1].name, "main") == 0);
 assert(f[1].resume_addr == (uintptr_t)main + f[1].resume_offset);
 printf("Here is a simple backtrace:\n");
 print_frames(f, frames_filled);
 printf("\n");

}

static int recursion_fun(int n)
{
    if (n == 0)
        return mystery();   // look in nameless.c
    else
        return 1 + recursion_fun(n-1);
}

static int test_backtrace_complex(int n)
{
    return recursion_fun(n);
}

static void test_red_back(void)
{
  char *p = malloc(24);
  heap_dump("After p = malloc(24)");
  char* returnVal = p - sizeof(struct header) - sizeof(REDZONE);
  struct header *hdr = (struct header *) returnVal;
  print_frames(hdr->f, 3);
}

static void test_heap_dump(void)
{
    heap_dump("Empty heap");

    char *p = malloc(24);
    heap_dump("After p = malloc(24)");


    free(p);
    heap_dump("After free(p)");

    char *q = malloc(8);
    heap_dump("After q = malloc(24)");
    // struct header *hdr1 = (struct header *)q-1;
    // print_frames(hdr1->f, 3);
    //
    char *r = malloc(24);
    heap_dump("After r = malloc(24)");
    //
    free(q);
    heap_dump("After free(q)");
    //
    //
    p = realloc(p, 20);
    heap_dump("After p = realloc(p, 30)");

    // char *r = malloc(24);
    // heap_dump("After p = malloc(24)");

    // free(q);
    // heap_dump("After free(p)");
    //
    // char *s = malloc(8);
    // heap_dump("After p = malloc(24)");

    //p = realloc(p, 32);
    //heap_dump("After p = realloc(p, 32)");

    //free(p);
    //heap_dump("After free(p)");
}

static void test_heap_simple(void)
{
    char *s;

    s = malloc(6);
    memcpy(s, "hello", 6);
    assert(strcmp(s, "hello") == 0);

    s = realloc(s, 12);
    strlcat(s, " world", 12);
    assert(strcmp(s, "hello world") == 0);
    free(s);

}

// array of dynamically-allocated strings, each
// string filled with repeated char, e.g. "A" , "BB" , "CCC"
// Examine each string, verify expected contents intact.
static void test_heap_multiple(void)
{
    int n = 26;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        assert(ptr != NULL);
        assert((uintptr_t)ptr % 8 == 0); // verify 8-byte alignment
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        ptr[num_repeats] = '\0';
        arr[i] = ptr;
    }
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);
        assert(first == last);
        free(arr[i]);
    }
}

//tests if two things added are back to back, separated by a header
static void test_heap_malloc1(void)
{
  int n = 32;
  void *ptr1 = malloc(n);
  void *ptr2 = malloc(n);
  assert(((char*)ptr1+n + 8) == ptr2);
  free(ptr2);
  free(ptr1);
}

//tests if free changes status to free
static void test_heap_malloc_free(void)
{
  int n = 32;
  void *ptr1 = malloc(n);
  struct header *current = (struct header *) ptr1 -1;
  assert (current-> size == 32);
  free(ptr1);
  assert(current->status == 1);
}

//tests if two free blocks are coalesced
static void test_heap_malloc_coalesce(void)
{
  int n = 32;
  int m = 24;
  void *ptr1 = malloc(n);
  void *ptr2 = malloc(m);
  struct header *current = (struct header *) ptr1 -1;
  free(ptr2);
  free(ptr1);
  assert(current->status == 1);
  assert(current->size == 64);
}

//tests if memory that is smaller than previous just changes header size
static void test_heap_realloc_smaller(void)
{
  int n = 32;
  int m = 24;
  int s = 8;
  void *ptr1 = malloc(n);
  struct header *current = (struct header *) ptr1 -1;
  void *ptr2 = malloc(m);
  void *real = realloc(ptr1, s);

  assert(current->status == 0);
  assert(current->size == s);
  assert(real == ptr1);
  free(ptr2);
  free(ptr1);

}

//tests if memory that is bigger and that fits in free space stays in same place
static void test_heap_realloc_bigger_sameplace(void)
{
  int n = 32;
  int m = 24;
  int s = 38;
  void *ptr1 = malloc(n);
  struct header *current = (struct header *) ptr1 -1;
  void *ptr2 = malloc(m);
  struct header *current2 = (struct header *) ptr2 -1;
  free(ptr2);
  void *real = realloc(ptr1, s);
  printf("%d", current->size);
  assert(current->status == 0);
  assert(current->size == 40);
  assert(real == ptr1);
  free(ptr1);
}

//tests if memory that does not fit is moved to end
static void test_heap_realloc_bigger_end(void)
{
  int n = 32;
  int m = 24;
  int s = 38;
  void *ptr1 = malloc(n);
  void *ptr2 = malloc(m);
  struct header *current2 = (struct header *) ptr2 -1;
  void *real = realloc(ptr1, s);
  struct header *current = (struct header *) real -1;
  void * currentEnd = (char*)ptr2 + sizeof(struct header) + 24;
  assert(current->status == 0);
  assert(current->size == 40);
  assert(real == currentEnd);
  free(ptr2);
  free(ptr1);
}

#define max(x, y) ((x) > (y) ? (x) : (y))

static void test_heap_recycle(int niter)
{
    extern int __bss_end__;
    void *heap_low = &__bss_end__, *heap_high = NULL;

    size_t max_size = 1024, total = 0;
    void *p = malloc(1);

    for (int i = 0; i < niter; i++) {
        size_t size = rand() % max_size;
        void *q = malloc(size);
        p = realloc(p, size);
        total += 2*size;
        void *higher = (char *)max(p, q) + size;
        heap_high = max(heap_high, higher);
        free(q);
    }
    free(p);
    size_t extent = (char *)heap_high - (char *)heap_low;
    size_t percent = total > extent ? (100*total)/extent : 0;
    printf("\nRecycling report:\n");
    printf("Completed %d iterations. Allocated %d bytes. Heap extent %d bytes. Recycling = %d%%\n", niter, total, extent, percent);
}

void test_heap_redzones(void)
{
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9); // write into payload
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      // ptr is NOT ok
}


void main(void)
{
    uart_init();

<<<<<<< HEAD
     //test_backtrace_simple();
    // test_red_back();
    // test_backtrace_simple(); // Again so you can see the main offset change!
    // test_backtrace_complex(7);  // Slightly tricky backtrace
    //
    //test_heap_dump();
=======
    test_backtrace_simple();
    test_backtrace_simple(); // Again so you can see the main offset change!
    test_backtrace_complex(7);  // Slightly tricky backtrace

    // test_heap_dump();
>>>>>>> assign4-basic
    //
    // test_heap_simple();
    // //test_heap_malloc1();
    // test_heap_malloc_free();
    // //test_heap_malloc_coalesce();
    // test_heap_realloc_smaller();
    // test_heap_realloc_bigger_sameplace();
    // //test_heap_realloc_bigger_end();
    // test_heap_multiple();
    // test_heap_recycle(10);

<<<<<<< HEAD
    test_heap_redzones(); // DO NOT USE unless you implemented red zone protection
=======
    //test_heap_redzones(); // DO NOT USE unless you implemented red zone protection
>>>>>>> assign4-basic

    printf("\nProgram %s completed.\n", __FILE__);
}
