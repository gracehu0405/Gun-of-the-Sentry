#include "gl.h"
#include "font.h"

const unsigned DEPTH = 4;

void gl_init(unsigned int width, unsigned int height, unsigned int mode)
{
    fb_init(width, height, DEPTH, mode);
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    color_t color = 0xff000000;
    color = color | b;
    color = color | (g<<8);
    color = color | (r<<16);
    return color;
}

void gl_clear(color_t c)
{
  for(int y= 0; y < fb_get_height(); y++) {
    for(int x = 0; x < fb_get_pitch()/4; x++ ) {
        gl_draw_pixel(x, y, c);
    }
  }
}

void gl_draw_pixel(int x, int y, color_t c)
{
  unsigned int width = fb_get_pitch()/4;
  if(x>=0 && y>=0) {
    if(x<width && y<fb_get_height()) {

      //im is a pointer to an array, each row is width wide, each element is an unsigned int
      //pitch is number of bytes, four bytes per pixel
      unsigned int (*im)[width] = (unsigned int (*)[width])fb_get_draw_buffer();
      im[y][x] = c;
    }
  }
}

color_t gl_read_pixel(int x, int y)
{
    if(x<0 || y<0) {
      return 0;
    }
    unsigned int width = fb_get_pitch()/4;
    if(x>width || y>fb_get_height()) {
      return 0;
    }
    unsigned int (*im)[width] = (unsigned int (*)[width])fb_get_draw_buffer();
    color_t color = im[y][x];
    return color;
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
  int width = x+w;
  int height = y+h;
  for (int row = y; row < height; row++) { //less than or equal??
    for(int col = x; col < width; col ++) {
      gl_draw_pixel(col, row, c);
    }
  }
}

void gl_draw_char(int x, int y, int ch, color_t c)
{
  unsigned char buf[font_get_size()];
  bool result = font_get_char(ch, buf, font_get_size());
  for (int row = 0; row < (font_get_height()); row++) { //less than or equal??
      for(int col = 0; col < (font_get_width()); col ++) {
        if(result) {
          int location = ((row)*font_get_width()) +((col)%font_get_height());
            if(buf[location]==0xff) {
              gl_draw_pixel(col+x, row+y, c);
            }
        }
      }
  }
}

void gl_draw_string(int x, int y, const char* str, color_t c)
{
  int index = 0;
  char ch = str[index];
  while(ch != '\0') {
    gl_draw_char(x, y, ch, c);
    x+=font_get_width();
    index++;
    ch = str[index];
  }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_width();
}
