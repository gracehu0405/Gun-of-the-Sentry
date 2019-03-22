#include "ultrasound.h"
#include "shell.h"
#include "uart.h"
#include "timer.h"
#include "gun.h"
#include "gpio.h"
#include "shell.h"
#include "keyboard.h"
#include "strings.h"
#include "gl.h"
#include "graphics.h"
#include "console.h"
#include "shell.h"

#define LINE_LEN 80
#define KEYBOARD_CLOCK_NEW GPIO_PIN12

static int welcome_user_and_get_mode(void);
static void interactive_mode(void);
static void auto_mode(int middleIndex);
static void update_screen(void);

void main(void){
    uart_init();
    timer_init();
    gpio_init();
    shell_init(console_printf);
    console_init(NROWS, NCOLS);
    keyboard_init(KEYBOARD_CLOCK_NEW, KEYBOARD_DATA);

    int mode = welcome_user_and_get_mode();
    gun_init(mode);

    if(mode == AUTO){
        graphics_init();
        console_printf("\nNow in Auto mode...\n");
        timer_delay(2);
        gl_clear(GL_BLACK);
        write_text();

        ultraSound_init();
        int middleIndex = middleSensor();
        auto_mode(middleIndex); 

    } else {

        console_printf("\nNow in interactive mode...\n");
        interactive_mode();
    }
}


/** FUNCTION: auto_mode
 *
 * This function puts the system in an infinite loop whereby the 
 * gun runs in its automatic configuration
 *
 * @param middleIndex - the index of the middle sensor
 */
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
            update_screen(); 
        }
        draw_fire_status(RESTING);

        while(getDistance(closestSensor()) < MAX_RANGE && closestSensor() == middleIndex){}
        draw_or_clear_target(1, CLEAR);
        draw_target_status(OFF_CENTERED);
    }
}


/** FUNCTION: welcome_user_and_get_mode
 *
 * This function prints a welcome message to the console and prompts
 * the user to set the mode of operation. It will reprompt the user
 * until they enter a valid mode
 *
 * @return - the mode given by the user
 */
static int welcome_user_and_get_mode(){

    char line[LINE_LEN];

    console_printf("Hello, welcome to the Raspberry Pi Sentry Gun!\n");
    console_printf("Please choose a mode of operation:\n\n");
    console_printf("Type 'A' for 'Auto mode' - gun performs auto tracking and firing\n");
    console_printf("Type 'I' for 'Interactive mode' - user controls gun with a keyboard\n\n");
    console_printf("Mode? ");

    shell_readline(line, sizeof(line));

    while(strcmp(line, "A") && strcmp(line, "I")){
        console_printf("\n\nPlease enter 'A' or 'I'\n");
        console_printf("Mode? ");
        shell_readline(line, sizeof(line));
    }

    if(strcmp(line, "A") == 0) return AUTO;
    return INTERACTIVE;
}


/** FUNCTION: update_screen
 *
 * This function is called every time the gun fires a bullet.
 * It updates the graphics with new information about the target,
 * bullets, etc.
 */
static void update_screen(void){
    draw_or_clear_target(0, CLEAR);
    draw_or_clear_target(2, CLEAR);
    draw_or_clear_target(1, DRAW);
    draw_remaining_darts();
    draw_target_status(CENTERED);
    draw_target_distance(getDistance(closestSensor()));
    draw_fire_status(FIRING);
}


/** FUNCTION: interactive_mode
 *
 * This function puts the system in an infinite loop whereby the gun operates
 * in its interactive configuration with a keyboard. The controls are as follows:
 *
 * press a - rotate counter clockwise
 * press d - rotate clockwise
 * press return - fire
 * press esc - quit
 */
static void interactive_mode(void){

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
