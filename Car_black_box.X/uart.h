/* 
 * File:   uart.h
 * Author: Acer
 *
 * Created on 30 August, 2024, 8:32 PM
 */

#ifndef UART_H
#define	UART_H



#define FOSC 20000000

void init_uart(unsigned long baud);
unsigned char getchar(void);
void putchar(unsigned char data);
void puts(const char *s);


void download_log(void);



#endif	/* UART_H */

