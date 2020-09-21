/*
* ENEMY_DETECT_EX.c
*
* Created: 2020-08-25 오후 3:57:24
* Author : user
*/

// define and HeaderFile Include
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "uart0.h" // UART0
#include "uart1.h" // UART1 헤더

// Declaring PWM for each Motor
#define NPULSE_MIN      980
#define NPULSE_MID      2965
#define NPULSE_MAX      4950
#define WPULSE_MIN      980
#define WPULSE_MID      2965
#define WPULSE_MAX      4950
#define SPULSE_MIN      980
#define SPULSE_MID      2890
#define SPULSE_MAX      4800
#define EPULSE_MIN      980
#define EPULSE_MID      2990
#define EPULSE_MAX      5000

// Distance from Ultra Sonic Sensor Turret to Laser Turret
#define L                10

// PI = 3.14
#define PI             3.14

// struct for Enemies coordinate and Quadra Number
typedef struct{
	int x;
	int y;
	int q;
}p;

// For UART1 USART1_RX_vect -> Index Variable for buffer_uart0
extern volatile int i;									
extern volatile int j;

// Buffer for UART0, UART1 -> Parse Data Until '\r' or '\n'
extern volatile unsigned char buffer_0[MAX_QSIZE];
extern volatile unsigned char buffer_1[MAX_QSIZE];

// TRUE : When each UART reads '\r' or '\n'
extern volatile int rxReadyFlag_0;					
extern volatile int rxReadyFlag_1;

short error_cnt = 0; // error_cnt == 10 -> Program Off
p Enemy[2]; // Maximum two Enemies

// UART Interrupt Sub Routine
ISR(USART1_RX_vect){UART1_ISR_Receive();}	// ISR 내에서 CALL하는건 바람직하지 않다.
ISR(USART0_RX_vect){UART0_ISR_Receive();}	// ISR 내에서 CALL하는건 바람직하지 않다.

// Initializing Timer 1 and 3
void InitilaizerTimer(){
	// MODE 14. HIGH SPEED PWM
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12) | (1<<WGM13);		// FASE PWM MODE
	
	//Non Invertint Mode
	//TOP : 1CR1, 비교 일치값: OCR1A 레지스터
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1);		// 채널A, 채널B 비반전모드 설정
	TCCR1B |= (1<<CS11);						// TIMER1 프리스케일러 설정
	
	ICR1 = 39999;
	
	TCCR3A |= (1<<WGM31);
	TCCR3B |= (1<<WGM32) | (1<<WGM33);		// FAST PWM 모드
	
	//비반전 모드
	//TOP : 1CR1, 비교 일치값: OCR1A 레지스터
	TCCR3A |= (1<<COM3A1) | (1<<COM3B1);		// 채널A, 채널B 비반전모드 설정
	TCCR3B |= (1<<CS11);						// TIMER1 프리스케일러 설정
	
	ICR3 = 39999;						// 20ms 주기
	
	DDRB = (1<<5) | (1<<6);
	DDRE = (1<<3) | (1<<4);
}

// test Function for UART 0 
void test_tx(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

// combining for int type to p type
p make_p(int a, int b, int c){
	p tmp;
	tmp.x = a;
	tmp.y = b;
	tmp.q = c;
	return tmp;
}

// Packet Message Parsing
// -1 : Error on Message
// 0 : Correct Message
int cmdParse(char* str){
	int x, y, q; // Coordinate, Quadra
	// For coordinate memcpy from str
	char tmp_x[4] = {0,};
	char tmp_y[4] = {0,};
	
	if(str[0]!='S') return -1; // Start Alphabet must be 'S'
	// parsing 2 info
	for(int i=0;i<2;i++){
		// Message Format : Se_x___y___e_x___y___E
		// parsing x data
		if(str[3+i*10]!='x') return -1;
		memcpy(tmp_x, str+4+i*10, 3);
		x = atoi(tmp_x);
		
		// parsing y data
		if(str[7+i*10]!='y') return -1;
		memcpy(tmp_y, str+8+i*10, 3);
		y = atoi(tmp_y);
		
		// parsing Quadra data
		if(str[1+i*10]=='e'){
			q = str[2+i*10] - '0';
			switch(str[2+i*10]-'0'){
				// setting x, y value depending on Quadra number
				case 0: x = 1000; y = 1000; break;
				case 1:break;
				case 2:	x*=-1;break;
				case 3:	x*=-1;y*=-1;break;
				case 4:	y*=-1;break;
				default:break;
			}
		}
		
		else return -1;
		
		// saving data from message
		Enemy[i] = make_p(x, y, q);
	}
	if(str[21]!='E') return -1; // End Alphabet must be 'E'
	return 0; // parsing complete
}

// true : Enemy is in range
// false : Enemy is out range
bool isInRange(int idx){
	if(Enemy[idx].x>=900||Enemy[idx].y>=900) return false;
	return true;
}

// true : motor could move
// false : motor could not move
bool possible(double x){ if(x>=0&&x<=180) return true; else return false;}

// rescaling angle data to PWM for Servo
// -1 : quadra is 0 -> no enemy
int rescale(int q, int idx, int ang){
	switch(q){
		case 1:{
			if(idx==0) return (int)((double)(ang/180.0) * (double)(NPULSE_MAX - NPULSE_MIN)) + NPULSE_MIN;
			else return (int)((double)(ang/180.0) * (double)(EPULSE_MAX - EPULSE_MIN)) + EPULSE_MIN;
			break;
		}
		case 2:{
			if(idx==0) return (int)((double)(ang/180.0) * (double)(WPULSE_MAX - WPULSE_MIN)) + WPULSE_MIN;
			else return (int)((double)(ang/180.0) * (double)(NPULSE_MAX - NPULSE_MIN)) + NPULSE_MIN;
			break;
		}
		case 3:{
			if(idx==0) return (int)((double)(ang/180.0) * (double)(SPULSE_MAX - SPULSE_MIN)) + SPULSE_MIN;
			else return (int)((double)(ang/180.0) * (double)(WPULSE_MAX - WPULSE_MIN)) + WPULSE_MIN;
			break;
		}
		case 4:{
			if(idx==0) return (int)((double)(ang/180.0) * (double)(EPULSE_MAX - EPULSE_MIN)) + EPULSE_MIN;
			else return (int)((double)(ang/180.0) * (double)(SPULSE_MAX - SPULSE_MIN)) + SPULSE_MIN;
			break;
		}
		default:break;
	}
	return -1;
}

// Quadra 1 : OCR1A, OCR1B
// Quadra 2 : OCR3A, OCR1A
// Quadra 3 : OCR3B, OCR3A
// Quadra 4 : OCR1B, OCR3B

// true : PWM Value is available
// false : unavailable
bool correctAng(int q, int idx, int m){
	int com1, com2; // MIN, MAX for each Motor
	
	switch(q){
		case 1:{
			if(idx==0) {com1 = NPULSE_MIN; com2 = NPULSE_MAX;}
			else {com1 = EPULSE_MIN; com2 = EPULSE_MAX;}
			break;
		}
		case 2:{
			if(idx==0) {com1 = WPULSE_MIN; com2 = WPULSE_MAX;}
			else {com1 = NPULSE_MIN; com2 = NPULSE_MAX;}
			break;
		}
		case 3:{
			if(idx==0) {com1 = SPULSE_MIN; com2 = SPULSE_MAX;}
			else {com1 = WPULSE_MIN; com2 = WPULSE_MAX;}
			break;
		}
		case 4:{
			if(idx==0) {com1 = EPULSE_MIN; com2 = EPULSE_MAX;}
			else {com1 = SPULSE_MIN; com2 = SPULSE_MAX;}
			break;
		}
		default: break;
	}
		if(m<com1||m>com2) return false;
		return true;
}

// move Servo Motor
void macrosel(int q, int idx, double ang){
	int m = rescale(q, idx, ang); // rescale angle to PWM
	if(m==-1||!correctAng(q, idx, m)) return; // if unavailable, end the function
	
	// move motor
	if(q==1) {
		if(idx==0)	OCR3A = m;
		else OCR1B = m;
	}
	else if(q==2){
		if(idx==0)	OCR1A = m;
		else OCR3A = m;
	}
	else if(q==3){
		if(idx==0)	OCR3B = m;
		else OCR1A = m;
	}
	else if(q==4){
		if(idx==0)	OCR1B = m;
		else OCR3B = m;
	}
}

// get Angle and move Servo
void killEnemy(int idx){
	// atan : -90~90, atan2 : 0~360
	int ang1, ang2;
	switch(Enemy[idx].q){
		case 1:{
			ang1 = (int)(ceil(atan2(Enemy[idx].y - L, Enemy[idx].x) * 180 / PI));
			ang2 = (int)(ceil(180 - atan2(Enemy[idx].x - L, Enemy[idx].y) * 180 / PI));
			break;
		}
		case 2:{
			ang1 = (int)(abs(ceil(atan2(Enemy[idx].x + L, Enemy[idx].y) * 180 / PI)));
			ang2 = (int)(ceil(atan2(Enemy[idx].y - L, Enemy[idx].x) * 180 / PI));
			break;
		}
		case 3:{
			ang1 = (int)(180 - abs(ceil(atan2(Enemy[idx].y + L, Enemy[idx].x) * 180 / PI)));
			ang2 = (int)abs(ceil(atan2(Enemy[idx].x + L, Enemy[idx].y) * 180 / PI));
			break;
		}
		case 4:{
			ang1 = (int)(ceil(180 - atan2(Enemy[idx].x - L, Enemy[idx].y) * 180 / PI));
			ang2 = (int)(ceil(180 + atan2(Enemy[idx].y + L, Enemy[idx].x) * 180 / PI));
			break;
		}
		default:break;
	}
	 macrosel(Enemy[idx].q, 0, ang1);
	 macrosel(Enemy[idx].q, 1, ang2);
}

// Motor Initialization
void motorInit(){
	OCR1A = WPULSE_MIN;
	OCR1B = EPULSE_MIN;
	OCR3A = NPULSE_MIN;
	OCR3B = SPULSE_MIN;
	_delay_ms(1000);
	_delay_ms(1000);
	_delay_ms(1000);

	OCR1A = WPULSE_MAX;
	OCR1B = EPULSE_MAX;
	OCR3A = NPULSE_MAX;
	OCR3B = SPULSE_MAX;
	_delay_ms(1000);
	_delay_ms(1000);
	_delay_ms(1000);

	OCR1A = WPULSE_MIN;
	OCR1B = EPULSE_MIN;
	OCR3A = NPULSE_MIN;
	OCR3B = SPULSE_MIN;
}
//char tmp[30]={0,};

int main(void)
{
	// pointer for buffer_uart0(400Bytes)
	unsigned char * receivedData;

	InitilaizerTimer(); // Initialize timer 1 and 3
	motorInit(); // Motor Initialize
	// UART Initialize
	UART0_init();
	UART1_init();
	sei(); // Interrupt Enable

	while (1){
		//if Message received from NodeMCU
		if(rxReadyFlag_1 == 1){
			receivedData = getRxString_1(); // get the Message
			int error = cmdParse(receivedData); // parse the Message -> if incorrect, error = -1
//			UART0_printf_string(receivedData);
			if(error==-1){
				if(++error_cnt==10) 
				  break; // if the message is wrong for 10times, programs exit
			}
			else{
			    for(int q = 0;q<2;q++){
					// if there is an Enemy, kill the Enemy
				    if(isInRange(q))
					    killEnemy(q);
			    }
			}
		}
	}
	
	return 0;
}

