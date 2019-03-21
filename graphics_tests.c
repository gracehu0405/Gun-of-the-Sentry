#include "graphics.h"
#include "uart.h"

void main(void) {
  uart_init();
  graphics_init();
  write_text();
}
