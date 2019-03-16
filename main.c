#include "ultrasound.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "gun.h"
#include "gpio.h"
#include "shell.h"
#include "keyboard.h"
#include "strings.h"

#define LINE_LEN 80

static int welcome_user_and_get_mode(void);
static void interactive_mode(void);

void main(void){
    uart_init();
    timer_init();
    gpio_init();
    gun_init();
    shell_init(printf);
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    int mode = welcome_user_and_get_mode(); 

    if(mode == AUTO){
        printf("\nNow in Auto mode...\n");
        ultraSound_init();
        int middleIndex = middleSensor();

        while(1) {
           // printf("distance_0 = %d inches, distance_1 = %d inches, distance_2 = %d inches\n", getDistance(0), getDistance(1), getDistance(2));
           // timer_delay_ms(250);
            
            int smallestIndex = closestSensor();
            if(smallestIndex < middleIndex){
                rotate_clockwise();
            } else if(smallestIndex > middleIndex){
                rotate_counter_clockwise();
            }

            while(closestSensor() != middleIndex){}
            rotator_off();

            //TODO: make get smallest distance a function
            if(getDistance(closestSensor()) < MAX_RANGE){
               // fire_once();
               trigger_on();
            }

            while(getDistance(closestSensor()) < MAX_RANGE && closestSensor() == middleIndex){}
            trigger_off();
        }
        // User interactive mode    
    } else {
        interactive_mode();
    }
}

static int welcome_user_and_get_mode(){

    char line[LINE_LEN];

    printf("Hello, welcome to the Raspberry Pi Nerf Gun!\n");
    printf("Please choose a mode of operation:\n\n");
    printf("Type 'Auto' for 'Auto mode' - gun performs auto tracking and firing\n");
    printf("Type 'Interactive' for 'Interactive mode' - user controls gun with a keyboard\n\n");
    printf("Mode? ");

    shell_readline(line, sizeof(line));

    while(strcmp(line, "Auto") && strcmp(line, "Interactive")){
        printf("\n\nPlease enter 'Auto' or 'Interactive'\n");
        printf("Mode? ");
        shell_readline(line, sizeof(line));
    }

    if(strcmp(line, "Auto") == 0) return AUTO;
    return INTERACTIVE;
}

static void interactive_mode(void){

    printf("\nNow in interactive mode...\n");
    key_event_t evt;
    while(1) {

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



