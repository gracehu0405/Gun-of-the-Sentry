#include "assert.h"
#include "keyboard.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "shell.h"

#define ESC_SCANCODE 0x76

static void test_keyboard_scancodes(void)
{
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        unsigned char scancode = keyboard_read_scancode();
        printf("[%02x]", scancode);
        if (scancode == ESC_SCANCODE) break;
    }
    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void)
{
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        printf("%s [%02x]\n", action.what == KEY_PRESS ? "  Press" :"Release", action.keycode);
        if (action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void)
{
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.other_ch, evt.modifiers);
        if (evt.action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with key events test.\n");
}

static void test_keyboard_chars(void)
{
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c >= '\t' && c <= 0x80)
            printf("%c", c);
        else
            printf("[%02x]", c);
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }
    printf("\nDone with key chars test.\n");
}

static void test_keyboard_assert(void)
{
    char ch;

    //checking letters and numbers
    printf("\ntype 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'g');

    printf("\ntype 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'a');

    printf("\ntype '4'\n");
    ch = keyboard_read_next();
    assert(ch == '4');

    printf("\ntype '1'\n");
    ch = keyboard_read_next();
    assert(ch == '1');

    //checking punctuation
    printf("\ntype '/'\n");
    ch = keyboard_read_next();
    assert(ch == '/');

    printf("\ntype '.'\n");
    ch = keyboard_read_next();
    assert(ch == '.');

    //checking shift
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');

    printf("\nHold down Shift and type 'a'\n");
    ch = keyboard_read_next();
    assert(ch == 'A');

    printf("\nHold down Shift and type '1'\n");
    ch = keyboard_read_next();
    assert(ch == '!');

    printf("\nHold down Shift and type '3'\n");
    ch = keyboard_read_next();
    assert(ch == '#');

    printf("\nHold down Shift and type '0'\n");
    ch = keyboard_read_next();
    assert(ch == ')');

    //checking CAPSLOCK
    printf("\n CAPSLOCK and type '1'\n");
    ch = keyboard_read_next();
    assert(ch == '1');
    printf("\n CAPSLOCK to turn off\n");

    printf("\n CAPSLOCK and type 'd'\n");
    ch = keyboard_read_next();
    assert(ch == 'D');
    printf("\n CAPSLOCK to turn off\n");

}

void main(void)
{
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard module.\n");

    test_keyboard_scancodes();
    timer_delay_ms(500);

    test_keyboard_sequences();
    timer_delay_ms(500);

     test_keyboard_events();
     timer_delay_ms(500);

     test_keyboard_chars();

     test_keyboard_assert();

    printf("All done!\n\04");
}
