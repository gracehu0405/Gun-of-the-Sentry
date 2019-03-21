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
    gun_init();
    shell_init(printf); //console_printf and change all printf to shell_printf
    keyboard_init(KEYBOARD_CLOCK_NEW, KEYBOARD_DATA);

    mode = welcome_user_and_get_mode();
    graphics_init();

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
    while(1){
        //write_text();
        /* while(1) {
           printf("distance_0 = %d inches, distance_1 = %d inches, distance_2 = %d inches\n", getDistance(0), getDistance(1), getDistance(2));
           timer_delay_ms(250);
           }*/

        smallestIndex = closestSensor();
        if(smallestIndex < middleIndex){
            rotate_clockwise();
        } else if(smallestIndex > middleIndex){
            rotate_counter_clockwise();
        }

        while(closestSensor() != middleIndex){}
        timer_delay_ms(100);
	rotator_off();

        if(getDistance(closestSensor()) < MAX_RANGE){
            fire_once();
        }

        while(getDistance(closestSensor()) < MAX_RANGE && closestSensor() == middleIndex){}
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
