#ifndef SERIAL_H
#define SERIAL_H 




void uart0_init(void);
void putc(unsigned char c);
unsigned char getc(void);
int isDigit(unsigned char c);
int isLetter(unsigned char c);


#endif