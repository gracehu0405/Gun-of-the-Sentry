#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "keyboard.h"
#include "malloc.h"
#include "printf.h"
#include "strings.h"
#include "pi.h"

#define LINE_LEN 80

static int (*shell_printf)(const char * format, ...);
static int isspace(char ch);
static int tokenize(const char *line, char *array[],  int max);
static char *strndup(const char *src, size_t n);
static unsigned int count = 1;
static unsigned int upCount = 1;
int cmd_history(int argc, const char *argv[]);


static const command_t commands[] = {
    {"help",   "<[cmd]> print a list of commands or description of cmd", cmd_help},
    {"echo",   "<...> echo user input to the screen", cmd_echo},
    {"reboot", "reboot the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek", "[address] print contents of memory at address", cmd_peek},
    {"poke", "[address] [value] store value at address", cmd_poke},
    {"history", "print history of recent commands", cmd_history},
};

typedef struct {
  char* command;
} history_t;

static history_t* historyArray[LINE_LEN];

int cmd_history(int argc, const char *argv[]) {
  for (int i=1; i<=count; i++) {
    char *current = historyArray[i-1]->command;
    shell_printf("   %d %s \n", i, current);
  }
  return 0;
}

int cmd_poke(int argc, const char *argv[])
{
  if(argc<3) {
    shell_printf("error: poke requires 2 arguments [address] and [value]\n");
    return 1;
  }

  //checks address
  const char ** endptr = &argv[1];
  unsigned int start = (unsigned int)*endptr;
  int address = strtonum(argv[1], endptr);

  //checks if input is zero or if address returned zero for fail
  if(address ==0 && (unsigned int )*endptr==start) {
    shell_printf("error: poke cannot convert '%s'\n", argv[1]);
    return 1;
  }

  //checks value
  endptr = &argv[2];
  start = (unsigned int)*endptr;
  int value = strtonum(argv[2], NULL);

  //checks if input is zero or if address returned zero for fail
  if(value ==0 && (unsigned int )*endptr==start) {
    shell_printf("error: poke cannot convert '%s'\n", argv[2]);
    return 1;
  }
  if(address%4 !=0) {
    shell_printf("error: poke address must be 4-byte aligned\n");
    return 1;
  }
  unsigned int* pointer = (unsigned int *)address;
  *pointer = value;
  return 0;
}

int cmd_peek(int argc, const char *argv[])
{
    if(argc!=2) {
      shell_printf("error: peek requires 1 argument [address]\n");
      return 1;
    }
    const char ** endptr = &argv[1];
    unsigned int start = (unsigned int)*endptr;
    int address = strtonum(argv[1], endptr);

    //checks if input is zero or if address returned zero for fail
    if(address ==0 && (unsigned int )*endptr==start) {
      shell_printf("error: peek cannot convert '%s'\n", argv[1]);
      return 1;
    }
    if(address%4 !=0) {
      shell_printf("error: peek address must be 4-byte aligned\n");
      return 1;
    }
    unsigned int* pointer = (unsigned int *)address;
    shell_printf("%p: %d\n", pointer, *pointer);
    return 0;
}

int cmd_reboot(int argc, const char *argv[])
{
    shell_printf("System rebooting. See ya back at the bootloader!\n %c", 4);
    pi_reboot();

    return 0;
}

int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i)
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[])
{
    //if args
    int numCommands = sizeof(commands)/12;
    if(argc>=2) {
      for(int i=0; i<numCommands; i++) {
        if(strcmp(commands[i].name, argv[1])==0) {
          shell_printf("%s: %s\n", commands[i].name, commands[i].description);
          return 0;
        }

      }
      shell_printf("error: no such command `%s`.\n", argv[1]);
      return 1;
    } else if (argc ==1) {

      //else if no args
      for(int i=0; i<numCommands; i++) {
        shell_printf("%s: %s \n", commands[i].name, commands[i].description);
      }
    }
    return 0;
}



void shell_init(formatted_fn_t print_fn)
{
    shell_printf = print_fn;
}

void shell_bell(void)
{
    uart_putchar('\a');
}

void shell_readline(char buf[], int bufsize)
{
  int index = 0;
  int maxIndex = 0;
    while(1) {
      unsigned char ch = keyboard_read_next();
      if(index>=(bufsize-1)) {
        break;
      }
      if(ch=='\n') {
        shell_printf("\n");
        break;
      }
      switch(ch) {
        case 1: //ctrl a
          while(index>=0) { //fix bug
            index--;
            shell_printf("%c", '\b');
          }
          ch = PS2_KEY_NONE;
          break;
        case 5: //ctrl e
          while(index<maxIndex) { //fix bug
            index++;
            shell_printf("%c", buf[index]);
          }
          ch = PS2_KEY_NONE;
          break;
        case PS2_KEY_ARROW_LEFT: //left arrow
          if(index>0) {
            index--;
            shell_printf("%c", '\b');
          } else {
            shell_bell();
          }
          break;
        case PS2_KEY_ARROW_RIGHT: //right arrow
          if(index<maxIndex) {
            shell_printf("%c", buf[index]);
            index++;
          } else {
            shell_bell();
          }
          break;
        case PS2_KEY_ARROW_UP: //up arrow
          //clear
          while(index>0) {
            index--;
            shell_printf("%c", '\b');
            buf[index] = ' ';
            shell_printf("%c", buf[index]);
            shell_printf("%c", '\b');
          }

          if(upCount>1) { //checks to see if there are commands entered yet
            char *current = historyArray[upCount-2]->command;
            //
            index = 0;
            while(*current != '\0') {
              buf[index] = *current;
              shell_printf("%c", buf[index]);
              index++;
              current++;
            }
            upCount--;
          }
          break;
        case '\b': //backspace
          if(index>0) {
            index--;
            shell_printf("%c", '\b');
            buf[index] = ' ';
            shell_printf("%c", buf[index]);
            shell_printf("%c", '\b');
          } else {
            shell_bell();
          }
          break;
        default :
          buf[index] = ch;
          shell_printf("%c", buf[index]);
          index++;
          maxIndex++;
          buf[index] = '\0';
          break;
      }
    }
}

int shell_evaluate(const char *line)
{

    //If no text entered
    if (*line == '\0') {
      return -1;
    }

    //If all space
    int lineIndex = 0;
    while (line[lineIndex]!= '\0') {
      if(line[lineIndex]!=' '){
        break;
      }
      lineIndex++;
    }
    if(line[lineIndex]=='\0') {
      return -1;
    }

    char* array[LINE_LEN];
    int length = strlen(line);
    int ntokens = tokenize(line, array, length);
    int returnValue = -1;

    //execute command
    char* command = array[0];
    int numCommands = sizeof(commands)/12;
    for(int i=0; i<numCommands; i++) {
      if(strcmp((commands[i].name), command)==0) {
        returnValue = commands[i].fn(ntokens, (const char **)array);
        return returnValue;
      }
    }
    shell_printf("error: no such command '%s' \n", line);
    return -1;
}

void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");

    while (1)
    {
        upCount = count;
        char line[LINE_LEN];
        shell_printf("[%d] Pi> ", count);
        shell_readline(line, sizeof(line));
        history_t* lineptr = (history_t*)malloc(LINE_LEN);
        lineptr->command = malloc(LINE_LEN);
        memcpy(lineptr->command, line, LINE_LEN);
        historyArray[count-1]=lineptr;
        shell_evaluate(line);
        count++;
    }
}

//Function taken from Lab 4: https://github.com/cs107e/lab4/blob/master/code/heapclient/string_main.c
static int isspace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

//Function taken from Lab 4: https://github.com/cs107e/lab4/blob/master/code/heapclient/string_main.c
static int tokenize(const char *line, char *array[],  int max)
{
    int ntokens = 0;

    while (*line != '\0' && ntokens < max) {
        while (isspace(*line)) line++;  // skip past spaces
        if (*line == '\0') break; // no more non-white chars
        const char *start = line;
        while (*line != '\0' && !isspace(*line)) line++; // advance to next space/null
        size_t nchars = line - start;
        array[ntokens++] = strndup(start, nchars);   // make heap-copy, add to array
    }
    return ntokens;
}

//Function taken from Lab 4: https://github.com/cs107e/lab4/blob/master/code/heapclient/string_main.c
static char *strndup(const char *src, size_t n)
{
    char* current = malloc(n);

    for(int i=0; i<n; i++) {
      current[i] = src[i];
    }
    if(n>0) {
      current[n] = '\0';
    }
    return current;
}
