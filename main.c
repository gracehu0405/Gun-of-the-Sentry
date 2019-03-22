#include "ultrasound.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "gun.h"
#include "gpio.h"
#include "shell.h"
#include "keyboard.h"
#include "strings.h"
#include "gl.h"
#include "graphics.h"

#define LINE_LEN 80
#define KEYBOARD_CLOCK_NEW GPIO_PIN12

static int welcome_user_and_get_mode(void);
static void interactive_mode(void);
static void auto_mode(int middleIndex);
void write_text(void);

void main(void){
    uart_init();
    timer_init();
    gpio_init();
    shell_init(printf); //console_printf and change all printf to shell_printf
    keyboard_init(KEYBOARD_CLOCK_NEW, KEYBOARD_DATA);

    int mode = welcome_user_and_get_mode();
    gun_init(mode);
    graphics_init();
    write_text();

    if(mode == AUTO){

        printf("\nNow in Auto mode...\n");
        ultraSound_init();
        int middleIndex = middleSensor();
        auto_mode(middleIndex); 

    } else {

        printf("\nNow in interactive mode...\n");
        interactive_mode();
    }
}

static void auto_mode(int middleIndex){
    int smallestIndex;
    unsigned int prev_time;

    while(1){
        smallestIndex = closestSensor();
        if(smallestIndex < middleIndex){
            rotate_clockwise();
            draw_or_clear_target(middleIndex, CLEAR);
            draw_or_clear_target(0, DRAW);
        } else if(smallestIndex > middleIndex){
            rotate_counter_clockwise();
            draw_or_clear_target(middleIndex, CLEAR);
            draw_or_clear_target(2, DRAW);
        }

        prev_time = timer_get_ticks();
        while(closestSensor() != middleIndex){
            if((timer_get_ticks() - prev_time) > TRACKING_TIMEOUT) break;
        }
        timer_delay_ms(CALIBRATION_DELAY);
        rotator_off();

        if(getDistance(closestSensor()) < MAX_RANGE){
            fire_once();
            draw_or_clear_target(0, CLEAR);
            draw_or_clear_target(2, CLEAR);
            draw_or_clear_target(1, DRAW);
        }

        while(getDistance(closestSensor()) < MAX_RANGE && closestSensor() == middleIndex){}
        draw_or_clear_target(1, CLEAR);
    }

}

static int welcome_user_and_get_mode(){

    char line[LINE_LEN];

    printf("Hello, welcome to the Raspberry Pi Sentry Gun!\n");
    printf("Please choose a mode of operation:\n\n");
    printf("Type 'A' for 'Auto mode' - gun performs auto tracking and firing\n");
    printf("Type 'I' for 'Interactive mode' - user controls gun with a keyboard\n\n");
    printf("Mode? ");

    shell_readline(line, sizeof(line));

    while(strcmp(line, "A") && strcmp(line, "I")){
        printf("\n\nPlease enter 'A' or 'I'\n");
        printf("Mode? ");
        shell_readline(line, sizeof(line));
    }

    if(strcmp(line, "A") == 0) return AUTO;
    return INTERACTIVE;
}

static void interactive_mode(void){

    key_event_t evt;

    while(1) {
        write_text();

        if(keyboard_read_next() == PS2_KEY_ESC) break;

        evt = keyboard_read_event();

        if(evt.action.what == KEY_PRESS && evt.key.ch == '\n') trigger_on();
        while(evt.key.ch == '\n'){
            if(keyboard_read_event().action.what == KEY_RELEASE) break;
        }
        trigger_off();


        if(evt.action.what == KEY_PRESS && evt.key.ch == 'a'){
            rotate_counter_clockwise();
        } else if(evt.action.what == KEY_PRESS && evt.key.ch == 'd'){
            rotate_clockwise();
        }
        while(evt.key.ch == 'a' || evt.key.ch == 'd'){
            if(keyboard_read_event().action.what == KEY_RELEASE) break;
        }

        rotator_off();
    }
}
