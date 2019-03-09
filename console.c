
#include "console.h"
#include "gl.h"
#include "uart.h"
#include <stdarg.h>
#include "printf.h"
#include "malloc.h"

#define MAX_OUTPUT_LEN 1024


static int rows; //size passed into console init
static int cols; //size passed into console init
unsigned char *array; //array where textBuf is stored
static int top=0; //current index of top line in textBuf
static int index=0; //current index in textBuf
static int scroll=0; //0 if not scrolling, 1 if scrolling

void console_putchar(char ch, unsigned char textBuf[rows][cols]);
void drawConsole(unsigned char textBuf[rows][cols]);
void clearRow(unsigned char textBuf[rows][cols]);
void clearBuf(void);

void console_init(unsigned int nrows, unsigned int ncols)
{
		gl_init(ncols*gl_get_char_width(), nrows*gl_get_char_height(), GL_DOUBLEBUFFER);
		rows = nrows;
		cols = ncols;
		array = (unsigned char *)malloc(rows*cols);
		clearBuf();
		console_clear();
}

void console_clear(void)
{
		index=0;
		clearBuf();
		top = 0;
		scroll = 0;
		gl_clear(GL_BLACK);
}

int console_printf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	char line[MAX_OUTPUT_LEN];
	int size = vsnprintf(line, MAX_OUTPUT_LEN, format, ap);
	va_end(ap);
	unsigned char (*textBuf)[rows]=(unsigned char (*)[rows])array;
	int startIndex = 0;

	//put chars into textBuf
	while(line[startIndex]!='\0') {
		console_putchar(line[startIndex], textBuf);
		startIndex++;
	}

	//clears and redraws
	gl_clear(GL_BLACK);
	drawConsole(textBuf);
	gl_swap_buffer();
	return size;
}

void drawConsole(unsigned char textBuf[rows][cols]) {

	unsigned int drawIndex = 0;
	unsigned int charIndex = top;
	while(drawIndex<(rows*cols)) {

		//indices for drawing each character
		int r = drawIndex/cols;
		int c = drawIndex%cols;
		int cursorX = c*gl_get_char_width();
		int cursorY = r*gl_get_char_height();

		//indices for extracting each character from textbuf
		charIndex = charIndex%(rows*cols);
		int charRow = charIndex/cols;
		int charCol = charIndex%cols;
		if(textBuf[charRow][charCol]!='\0') {
			gl_draw_char(cursorX, cursorY, textBuf[charRow][charCol], GL_GREEN);
		}
		drawIndex++;
		charIndex++;
	}
}

void console_putchar(char ch, unsigned char textBuf[rows][cols]) {
	int size = cols*rows;
	//scroll
	if(index >= (size)) {
		index = index%(size);
		scroll = 1;
	}

	//move top index
	if(scroll == 1) {
		top = ((index/cols +1)*cols)%(size);

		//if new row
		if(index%cols == 0) {
			clearRow(textBuf);
		}
	}

	int r = index/cols;
	int c = index%cols;
	switch(ch) {
		case '\0':
			textBuf[r][c]='\0';
			break;
		case '\n':
			textBuf[r][c]='\0'; //fix textbuf
			index+=(cols-c);
			if(index>=(size)) {
				index = index%(size);
				scroll = 1;
			}
			if(scroll == 1) { //scroll if at the bottom
				top = ((index/cols +1)*cols)%(size);
				if(index%cols == 0) {
					clearRow(textBuf);
				}
			}
			break;
		case '\b':
			if(c>0) { //check
				index--;
				r = index/cols;
				c = index%cols;
				textBuf[r][c]=' ';
			}
			break;
		case '\r':
			index-=c;
			break;
		case '\f':
			clearBuf();
			console_clear(); //test
			break;
		default:
			textBuf[r][c]=ch;
			index++;
			break;
		}
}

void clearRow(unsigned char textBuf[rows][cols]){
	unsigned int cur = index;
	for(int i = cur; i < (cur+cols); i++) {
		int r = i/cols;
		int c = i%cols;
		textBuf[r][c] = '\0';
	}
}

void clearBuf(void) {
	for(int i = 0; i < rows*cols; i++) {
		array[i]='\0';
	}
}
