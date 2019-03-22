#include "graphics.h"
#include "uart.h"
#include "timer.h"

void main(void) {
  uart_init();
  graphics_init();
  write_text();
  draw_or_clear_target(1, DRAW);
  draw_or_clear_target(0, DRAW);
  draw_or_clear_target(2, DRAW);

  timer_delay(2);

  draw_or_clear_target(1, CLEAR);
  draw_or_clear_target(0, CLEAR);
  draw_or_clear_target(2, CLEAR);

  timer_delay(2);

  draw_or_clear_target(0, DRAW);
  timer_delay(1);
  draw_or_clear_target(0, CLEAR);
  draw_or_clear_target(1, DRAW);


}
