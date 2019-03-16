/*
 * File: shell.c
 * ---------------
 *
 * Name: Michael Oduoza
 *
 * This file, shell.c, contains the implementation of a simple
 * shell for the raspberry pi. It included the help, echo, reboot,
 * peek, and poke commands.
 */

#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "keyboard.h"
#include "malloc.h"
#include "strings.h"
#include "pi.h"

#define LINE_LEN 80
#define ASCII_MAX 0x7f
#define NUM_COMMANDS 5 

static int (*shell_printf)(const char * format, ...);
static char *strndup(const char *src, int n);
static int isspace(char ch);
static int tokenize(const char *line, char *array[],  int max);

static const command_t commands[] = {
    {"help",   "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"echo",   "<...> echos the user input to the screen", cmd_echo},
    {"reboot", "reboot the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek",   "peek [address] prints the contents (4 bytes) of memory at address", cmd_peek},
    {"poke",   "poke [address] [value] stores value into memory at address", cmd_poke},
}; 


/** FUNCTIONS: commands
 *
 * The 5 functions below implement all the 5 shell commands. 
 *
 * @param argc - the number of arguments passed
 * @param argv - an array containing only the string arguments
 * @return - 0 if the command was successful and 1 otherwise
 */
int cmd_echo(int argc, const char *argv[]) 
{
    shell_printf("\n");
    for (int i = 0; i < argc; i++) 
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}


int cmd_reboot(int argc, const char* argv[])
{   
    shell_printf("\nRebooting...\n\04"); // send EOT byte to rpi-install.py
    pi_reboot();
    return 0;
}


int cmd_peek(int argc, const char* argv[])
{
    if(argc == 0){
      shell_printf("\nerror: peek expects 1 argument [address]\n");
      return 1;
    }   

    const char* endptr = NULL;
    int addressNum = strtonum(argv[0], &endptr);

    if(*endptr != '\0'){
       shell_printf("\nerror: peek cannot convert '%s'\n", argv[0]);
       return 1;
    }

    if(addressNum % 4 != 0){
      shell_printf("\nerror: peek address must be 4-byte aligned\n");
      return 1;
    }

    int peekValue = *((int*) addressNum);
    shell_printf("\n%p:   %08x\n", (void*) addressNum, peekValue);
    return 0;
}


int cmd_poke(int argc, const char* argv[])
{
   if(argc < 2){
      shell_printf("\nerror: poke expects 2 arguments [address] [value]\n");
      return 1;
    }   

    const char* address_endptr = NULL;
    int addressNum = strtonum(argv[0], &address_endptr);

    const char* value_endptr = NULL;
    int value = strtonum(argv[1], &value_endptr);

    if(*address_endptr != '\0'){
       shell_printf("\nerror: poke cannot convert '%s'\n", argv[0]);
       return 1;
    }

    if(*value_endptr != '\0'){
       shell_printf("\nerror: poke cannot convert '%s'\n", argv[1]);
       return 1;
    }

    if(addressNum % 4 != 0){
      shell_printf("\nerror: poke address must be 4-byte aligned\n");
      return 1;
    }

    *((int*) addressNum) = value;
    shell_printf("\n");
    return 0;
}


int cmd_help(int argc, const char *argv[]) 
{
    command_t cmd;

    int cmdNum = 0;
    for(cmdNum = 0; cmdNum < NUM_COMMANDS; cmdNum++){
       cmd = commands[cmdNum];
        // If we have been provided with a name 
        if(argc != 0){
            if(strcmp(cmd.name, argv[0]) == 0){
                shell_printf("\n%s: %s\n", cmd.name, cmd.description);
                break;
            }
        // No command name given, so print all of them
        } else {
            shell_printf("\n%s: %s", cmd.name, cmd.description);
            if(cmdNum == (NUM_COMMANDS - 1)) shell_printf("\n"); // print a new line after the last command
        }
    }

    // User provided an invalid name
    if(argc != 0 && cmdNum == NUM_COMMANDS){
       shell_printf("\nerror: no such command '%s'.\n", argv[0]);
       return 1;
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


/** FUNCTION: shell_readline
 *
 * This functions reads in input as the user types commands into the shell.
 * It also prints out the characters as the user is typing, for the user to see.
 *
 * @param buf - buffer to store user input
 * @param bufsize - size of the buffer in bytes
 */
void shell_readline(char buf[], int bufsize)
{
    unsigned int index = 0;
    char ch;
    while(index < (bufsize - 1)){

        ch = keyboard_read_next();
        if(ch == '\n') break;
        if(ch == '\b') {
          if(index != 0){  
             shell_printf("\b \b"); // back up, output a space, then back up again
             index--;
          } else {
             shell_bell();
          }
        } else if(ch <= ASCII_MAX){
          buf[index] = ch;
          shell_printf("%c", ch);
          index++;
        }
    }
  buf[index] = '\0'; // null terminate the entry
}


/** FUNCTION: shell_evaluate
 *
 * This function sorts a user inputed line on the shell into tokens
 * and then calls the appropriate command with the appropriate arguments, based
 * on the tokens the user provided. Prints and error message in the case of an 
 * invalid command
 *
 * @param line - the input line read from the user
 * @return - 0 if successful and 1 if unsuccessful
 */
int shell_evaluate(const char *line)
{
    int max = strlen(line);
    char* tokensArray[max];
    int numTokens = tokenize(line, tokensArray, max);

    // empty line (or all spaces)
    if(numTokens == 0){
        shell_printf("\n");
        return 1; 
    }
    
    command_t cmd;
    int cmdNum = 0;
    for(cmdNum = 0; cmdNum < NUM_COMMANDS; cmdNum++){
       cmd = commands[cmdNum];
       if(strcmp(cmd.name, tokensArray[0]) == 0) break; // found the command      
    }

    // This implies that the command was not found 
    if(cmdNum == NUM_COMMANDS){
      shell_printf("\nerror: no such command '%s'.\n", tokensArray[0]);
      return 1;
    }
    
    int toReturn = (cmd.fn)(numTokens - 1, (const char**) tokensArray + 1);
    for(int i = 0; i < numTokens; i++) free(tokensArray[i]); // free the malloc-ed memory
    return toReturn;
}


/** FUNCTION: shell_run
 *
 * This function executes the shell on the raspberry pi in a
 * never-ending loop
 */
void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1) 
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}


/* All code from this point on belongs originally to an external source
 *
 * SOURCE: CS107E Winter 2019 Assign4-starter code (heap.c file)
 */
static char *strndup(const char *src, int n)
{
    char *copy = malloc(n+1);
    copy[n] = '\0';
    return memcpy(copy, src, n);
}

static int isspace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, char *array[],  int max)
{
    int ntokens = 0;

    while (*line != '\0' && ntokens < max) {
        while (isspace(*line)) line++;  // skip past spaces
        if (*line == '\0') break; // no more non-white chars
        const char *start = line;
        while (*line != '\0' && !isspace(*line)) line++; // advance to next space/null
        int nchars = line - start;
        array[ntokens++] = strndup(start, nchars);   // make heap-copy, add to array
    }
    return ntokens;
}

