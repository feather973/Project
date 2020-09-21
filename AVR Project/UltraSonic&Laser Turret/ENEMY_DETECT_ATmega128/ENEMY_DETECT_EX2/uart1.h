/*
 * UART1.h
 *
 * Created: 2020-04-10 오전 11:58:38
 *  Author: master
 */ 


#ifndef UART1_H_
#define UART1_H_

void UART1_init(void);
void UART1_transmit(char data);
unsigned char UART1_receive(void);
void UART1_printf_string(char *str);
#define MAX_QSIZE  400   
#define TRUE 1
#define FALSE 0
#endif /* UART1_H_ */
