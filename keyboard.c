#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"

static unsigned int CLK, DATA;
static void getModifiers(key_action_t currentAction, ps2_key_t currentKey);
static void updateModifier(int action, int currentModifier, int lock);
static keyboard_modifiers_t modifier = 0b00000;
static int validCaps(unsigned char keyChar);
static int checkModifiers(ps2_key_t currentKey);

void wait_for_falling_clock_edge(void) {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);

    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA);
}


unsigned char keyboard_read_scancode(void)
{
  while(1) {
    unsigned char returnValue = 0;
    unsigned char total=0;
    wait_for_falling_clock_edge();

    //Check start but is zero
    if(gpio_read(DATA)==0) {

      int timeCheck = 0;
      int currentTime = timer_get_ticks();

      //Read bits with data
      for(int i=0; i<8; i++) {
        wait_for_falling_clock_edge();
        if(timer_get_ticks() < currentTime +3000) {
          unsigned char current = gpio_read(DATA);
          total = total + current;
          returnValue += current<<i;
          currentTime = timer_get_ticks();
        } else {
          i=8; //breaks out of for loop
          timeCheck = 1; //restarts while loop
        }
      }

      if(timeCheck ==0) {
        currentTime = timer_get_ticks();
        wait_for_falling_clock_edge();

        //Check parody bit makes it odd
        unsigned char parody = gpio_read(DATA);
        total = total + parody;
        if (total%2 ==1&& (timer_get_ticks() < currentTime +3000)) {
          currentTime = timer_get_ticks();
          wait_for_falling_clock_edge();

          //Check end bit is one
          if(gpio_read(DATA)==1 && (timer_get_ticks() < currentTime +3000)) {
            return returnValue;
          }
        }
      }
    }
  }
}


key_action_t keyboard_read_sequence(void)
{
  key_action_t action;
  action.keycode = keyboard_read_scancode();

  //Modifier key
  if(action.keycode==PS2_CODE_EXTENDED) {
    action.keycode = keyboard_read_scancode();

    //Modifier key released
    if(action.keycode==PS2_CODE_RELEASE) {
      action.keycode = keyboard_read_scancode();
      action.what = KEY_RELEASE;
      return action;

      //Modifier key pressed
    }  else {
      action.what = KEY_PRESS;
      return action;
    }
  }

  //Normal key released
  if(action.keycode==PS2_CODE_RELEASE) {
    action.keycode = keyboard_read_scancode();
    action.what = KEY_RELEASE;
    return action;

    //Normal key pressed
  } else {
    action.what = KEY_PRESS;
    return action;
  }
}

key_event_t keyboard_read_event(void)
{
    key_event_t event;
    key_action_t currentAction = keyboard_read_sequence();
    event.action = currentAction;
    ps2_key_t currentKey = ps2_keys[currentAction.keycode];
    event.key = currentKey;

    //if modifier key pressed
    if(currentKey.ch>=0x90) {
      getModifiers(currentAction, currentKey);

    }
    event.modifiers = modifier;
    return event;
}


unsigned char keyboard_read_next(void) //check escape
{
  while(1) {
    key_event_t event = keyboard_read_event();
    key_action_t action = event.action;
    ps2_key_t currentKey = ps2_keys[action.keycode];
    unsigned char ch = currentKey.ch;

    if(action.what != KEY_RELEASE) {
      if(currentKey.ch <= 0x7F) {
        int checkCaps =  validCaps(currentKey.ch); //if one of the top row, returns 1

        //shift
        if((modifier>>3 & 1) ==1) {
          ch =currentKey.other_ch;

        //caps
        } else if ((modifier>>2 & 1) ==1 && checkCaps==0) {
          ch =currentKey.other_ch;

          //ctrl
        } else if((modifier>>5 & 1) ==1) {
            ch = currentKey.ch - 'a' +1;
        }
      }

      //If modifier, continue with loop and do not return ch
      if(checkModifiers(currentKey) ==0) {
        return ch;
      }
    }
  }
}

static int checkModifiers(ps2_key_t currentKey) {
  int check = 0;
  int modifiers[6] = {PS2_KEY_SHIFT, PS2_KEY_ALT, PS2_KEY_CTRL, PS2_KEY_SCROLL_LOCK,
    PS2_KEY_NUM_LOCK, PS2_KEY_CAPS_LOCK};
  for (int i=0; i<6; i++) {
    if(currentKey.ch == modifiers[i]) {
      check =1;
    }
  }
  return check;
}

static int validCaps(unsigned char keyChar) {
  if(keyChar >= 0x30 && keyChar <= 0x39) {
    return 1;
  }
  if(keyChar==0x60) { // acccent
    return 1;
  }
  if(keyChar==0x2D) { //-
    return 1;
  }
  if(keyChar==0x3D) { //=
    return 1;
  }
  return 0;
}

static void getModifiers(key_action_t currentAction, ps2_key_t currentKey) {

  char code = currentKey.ch;
  int lock = 0;
  int currentModifier=0;

  switch(code) {
    case PS2_KEY_SHIFT :
      currentModifier = KEYBOARD_MOD_SHIFT;
      break;
    case PS2_KEY_ALT :
      currentModifier = KEYBOARD_MOD_ALT;
      break;
    case PS2_KEY_CTRL :
      currentModifier = KEYBOARD_MOD_CTRL;
      break;
    case PS2_KEY_SCROLL_LOCK:
      lock = 1;
      currentModifier = KEYBOARD_MOD_SCROLL_LOCK;
      break;
    case PS2_KEY_NUM_LOCK:
      lock = 1;
      currentModifier = KEYBOARD_MOD_NUM_LOCK;
      break;
    case PS2_KEY_CAPS_LOCK :
      lock = 1;
      currentModifier = KEYBOARD_MOD_CAPS_LOCK;
      break;
  }
  updateModifier(currentAction.what, currentModifier, lock);
}

static void updateModifier(int action, int currentModifier, int lock) {
    //If not lock command
    if(lock ==0) {
      //shift pressed
      if(action == KEY_PRESS) {
        modifier = modifier | currentModifier;

        //shift released, set modifier to 0
      } else {
        modifier = modifier & ~currentModifier;
      }

      //If lock command
    } else {
      if(action==KEY_PRESS) {
        modifier = modifier ^ currentModifier;
      }
    }
}
