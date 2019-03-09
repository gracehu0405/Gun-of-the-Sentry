#include "assert.h"
#include "timer.h"
#include "fb.h"
#include "gl.h"
#include "console.h"
#include "printf.h"
#include "rand.h"
#include "strings.h"

/* Note that to use the console, one should only have to
 * call console_init. To use the graphics library, one
 * should only have to call gl_init. If your main() requires
 * more than this, then your code will not pass tests and
 * will likely have many points deducted. Our GL tests
 * will call gl_init then invoke operations; our console
 * tests will call console_init then invoke operations.
 * To guarantee that tests will pass, make sure to also
 * run tests for each component separately.
 */

#define _WIDTH 640
#define _HEIGHT 512

#define _NROWS 10
#define _NCOLS 20

static void draw_strings(int W, int H, void(*draw_string)(int, int, char*, color_t))
{
    for (int i = 0; i < 20; i++) {
        char buf[20];
        snprintf(buf, sizeof(buf), "Number #%d", i+1);
        int x = rand() % W;
        int y = rand() % H;
        int w = strlen(buf)*gl_get_char_width();
        int h = gl_get_char_height();
        draw_string(x-w/2, y-h/2, buf, rand());
    }
}

static void draw_random_rects(int W, int H, void(*draw_rect)(int, int, int, int, color_t))
{
    for (int i = 0; i < 500; i++) {
        int x = rand() % W;
        int y = rand() % H;
        int w = rand() % 200;
        int h = rand() % 200;
        draw_rect(x-w/2, y-h/2, w, h, rand());
    }
}

static void test_gl(void)
{
    // Double buffer mode, make sure you test single buffer too!
  gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);

  // Background should be purple.
  gl_clear(gl_color(0xFF, 0, 0xFF));

  // Draw an amber pixel at an arbitrary spot.
  gl_draw_pixel(_WIDTH/3, _HEIGHT/3, GL_AMBER);
  assert(gl_read_pixel(_WIDTH/3, _HEIGHT/3) == GL_AMBER);

  // Basic rectangle should be blue in center of screen
  gl_draw_rect(_WIDTH/2 - 20, _HEIGHT/2 - 20, 40, 40, GL_BLUE);

  // String that goes off screen
  gl_draw_string(_WIDTH-200, 10, "HELLO WORLD", GL_BLUE);

  // Should write a single character
  gl_draw_char(60, 10, 'A', GL_BLUE);

  // Show buffer with drawn contents
  gl_swap_buffer();


}

static void test_words(void)
{
  gl_init(_WIDTH, _HEIGHT,  GL_SINGLEBUFFER); gl_clear(GL_WHITE);
    draw_strings(_WIDTH, _HEIGHT, &gl_draw_string);
    timer_delay(1);
}

static void test_gl_single(void) {
  // Double buffer mode, make sure you test single buffer too!
gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);

// Background should be purple.
gl_clear(gl_color(0xFF, 0, 0xFF));

// Draw an amber pixel at an arbitrary spot.
gl_draw_pixel(_WIDTH/3, _HEIGHT/3, GL_AMBER);
assert(gl_read_pixel(_WIDTH/3, _HEIGHT/3) == GL_AMBER);

//bottom of screen
gl_draw_pixel(_WIDTH/2, _HEIGHT-1, GL_AMBER);

// Draw off edge
gl_draw_rect(_WIDTH - 20, _HEIGHT-1 - 20, 40, 40, GL_BLUE);

// Should write a single character
gl_draw_char(60, 10, 'A', GL_BLUE);

// String that goes off screen
gl_draw_string(_WIDTH-20, 10, "HELLO WORLD", GL_BLUE);

// Show buffer with drawn contents
gl_swap_buffer();

}


static void test_console(void)
{
  console_init(_NROWS, _NCOLS);



// 1: "HELLO WORLD"
 console_printf("HELLO WORLD\r");
 timer_delay(3);

 // 1: "HAPPY WORLD"
 // 2: "CODING"
  console_printf("HAPPY\nCODING\n");
 timer_delay(3);

 // Clear
 console_printf("\f");

 // 1: "GOODBYE"
 console_printf("GOODBYE\n");
}

static void test_console_scroll1(void)
{
  console_init(_NROWS, _NCOLS);

  console_printf("start\n");
  for (int i=0; i<20; i++) {
    console_printf("%d\n", i);
    timer_delay(1);

  }
  // Clear
  console_printf("\f");

  // 1: "GOODBYE"
  console_printf("GOODBYE\n");

}

static void test_console_scroll2(void)
{
  console_init(_NROWS, _NCOLS);
  for (int i=0; i<10; i++) {
    console_printf("%d\n", i);

  }
  for (int i=0; i<100; i++) {
    console_printf("%d", i);
  }

}

static void test_console_backspace(void)
{
  console_init(_NROWS, _NCOLS);
  for (int i=0; i<10; i++) {
    console_printf("%d\n", i);

  }
   //console_printf("Hello\b\b");

}

//test backspace

void main(void)
{
    test_gl();
    //test_words()
  //   test_gl_single();
  //  test_console();
  //    test_console_scroll1();
  //    test_console_scroll2();
  // test_console_backspace();




    /* TODO: Add tests here to test your graphics library and console.
       For the framebuffer and graphics libraries, make sure to test
       single & double buffering and drawing/writing off the right or
       bottom edge of the frame buffer.
       For the console, make sure to test wrap-around and scrolling.
    */
}
