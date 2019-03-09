#include "mailbox.h"
#include "fb.h"

// This prevents the GPU and CPU from caching mailbox messages
#define GPU_NOCACHE 0x40000000

typedef struct {
  unsigned int width;       // width of the display
  unsigned int height;      // height of the display
  unsigned int virtual_width;  // width of the virtual framebuffer
  unsigned int virtual_height; // height of the virtual framebuffer
  unsigned int pitch;       // number of bytes per row
  unsigned int depth;       // number of bits per pixel
  unsigned int x_offset;    // x of the upper left corner of the virtual fb
  unsigned int y_offset;    // y of the upper left corner of the virtual fb
  unsigned int framebuffer; // pointer to the start of the framebuffer
  unsigned int size;        // number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));

void fb_init(unsigned int width, unsigned int height, unsigned int depth, unsigned int mode)
{

  fb.width = width;
  fb.virtual_width = width;
  fb.height = height;
  if(mode==1) {
    //printf("mode 1\n");
    fb.virtual_height = 2*height;
  } else {
    fb.virtual_height = height;
  }
  fb.depth = depth * 8; // convert number of bytes to number of bits
  fb.x_offset = 0;
  fb.y_offset = 0; //does this change

  // the manual requires we to set these value to 0
  // the GPU will return new values
  fb.pitch = 0;
  fb.framebuffer = 0;
  fb.size = 0;

  mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
  (void) mailbox_read(MAILBOX_FRAMEBUFFER);
}

void fb_swap_buffer(void)
{
  if(fb.virtual_height==(2*fb.height)) { //checks if double buffer
    if(fb.y_offset == 0) {
      fb.y_offset = fb.height;
    } else {
      fb.y_offset = 0;
    }
    fb.pitch = 0;
    fb.framebuffer = 0;
    fb.size = 0;
    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
    (void) mailbox_read(MAILBOX_FRAMEBUFFER);
  }
}

unsigned char* fb_get_draw_buffer(void)
{
  if(fb.virtual_height==(2*fb.height)) {
    if(fb.y_offset == 0) {
      unsigned int drawBuffer = fb.framebuffer + (fb.height*fb.pitch);
      return (unsigned char *)drawBuffer;
    } else {
      return (unsigned char *)fb.framebuffer;
    }
  }
  return (unsigned char *)fb.framebuffer;
}

unsigned int fb_get_width(void)
{
    return fb.width;
}

unsigned int fb_get_height(void)
{
    return fb.height;
}

unsigned int fb_get_depth(void)
{
    return fb.depth/8;
}

unsigned int fb_get_pitch(void)
{
    return fb.pitch;
}
