/*
 * UART0.h
 *
 * Created: 2020-04-10 오전 11:58:38
 *  Author: master
 */ 


#ifndef UART0_H_
#define UART0_H_

void UART0_init(void);
void UART0_transmit(char data);
unsigned char UART0_receive(void);
void UART0_printf_string(char *str);
#define MAX_QSIZE  400   
#define TRUE 1
#define FALSE 0
#endif /* UART0_H_ */
