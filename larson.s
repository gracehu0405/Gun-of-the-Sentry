/*
 * LARSON SCANNER EXTENSION
 *
 * Steffi Andersen
 * This code lights up LEDs back and forth across a breadboard in a scanning pattern with change of brightness.
 */

.equ DELAY, 0x000500
.equ NUMLED, 8
.equ DIM1, 0x003000
.equ BLINKNUM, 0x000040

// configure GPIO 20 for output
ldr r0, FSEL2
mov r1, #1

// store the value 1 in r2 to set current GPIO to output
mov r2, #1

// store the value 111 in r3 to mask the values not in the 3-bit GPIO window
mov r3, #7

// store one less than the number of LEDs in r4 to count the number of loops to complete
mov r4, #NUMLED
sub r4, r4, #1

// loop that configures GPIOs to output

outputloop:
	lsl r3, r3, #3
	lsl r2, r2, #3
	bic r1, r1, r3
	orr r1, r2
	subs r4, r4, #1
	bne outputloop

// store final value of output configurations in r1
str r1, [r0]

// set 11111 bits in a row 
mov r1, #7
lsl r1, r1, #2
add r1, r1, #3
lsl r1, r1, #18

//set 11011 bits
mov r9, #3
lsl r9, r9, #3
add r9, r9, #3
lsl r9, r9, #18

//set 10001 bits
mov r11, #1
lsl r11, r11, #4
add r11, r11, #1
lsl r11, r11, #18

// initialize r3 as counter and r4 with one less than the number of LEDs
mov r3, #0
mov r4, #NUMLED
sub r4, r4, #1

// loops to turn off and move the LEDs
loop:

//Sets counter for number of loops LEDs blink
mov r10, #BLINKNUM

//loops to change brightness
loopouter:

// set current GPIO high
ldr r0, SET0
str r1, [r0] 

// delay
mov r2, #DELAY
wait1:
    subs r2, #1
    bne wait1

// dims outer LEDs

//set current GPIO high
ldr r7, CLR0
str r11, [r7] 

//delay
mov r8, #DIM1
wait3:
  subs r8, #1
  bne wait3

// dims middle LEDs

// set current GPIO high
ldr r7, CLR0
str r9, [r7] 

// delay
mov r13, #DIM1
wait4:
    subs r13, #1
    bne wait4

// repeats loopouter DIMDELAY number of times
subs r10, #1
bne loopouter

// turns off all LEDs

// set current GPIO low
ldr r0, CLR0
str r1, [r0] 

// delay
mov r2, #DELAY
wait2:
    subs r2, #1
    bne wait2

// compares counter (r3) to zero, if plus or zero, left shifts r1, and if minus, right shifts r1
cmp r3, #0
lslpl r1, #1
lslpl r9, #1
lslpl r11, #1
lsrmi r1, r1, #1
lsrmi r9, r9, #1
lslmi r11, r11, #1
// increments counter (r3)
add r3, r3, #1

// compares counter (r3) to one less than the number of LEDs, if equal, makes r3 negative
cmp r3, r4
mov r5, #0
subeq r3, r5, r4

b loop

FSEL0: .word 0x20200000
FSEL1: .word 0x20200004
FSEL2: .word 0x20200008
SET0:  .word 0x2020001C
SET1:  .word 0x20200020
CLR0:  .word 0x20200028
CLR1:  .word 0x2020002C

