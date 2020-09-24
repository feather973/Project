#include <avr/io.h>
#include "uart1.h"   
// UART1 USART1_RX_vect가 호출 한다. 
volatile int j=0;   // buffer_uart0 의 index 변수
volatile unsigned char buffer_1[MAX_QSIZE]={0};   // UART 부터 읽어온 data를 \r 이나 \n을 만날때 까지 저장할 공간
volatile int rxReadyFlag_1=0;   // \r이나 \n을 만났다는 인디케이터 flag변수

unsigned char *getRxString_1(){
	rxReadyFlag_1 = 0;   // 또 다른 message를 처리 하기 위하여 rxReadyFlag를 clear한다.
	return buffer_1;
}

// #3 인터럽트 코드를 작성 한다. 
void UART1_ISR_Receive()
{
	unsigned char data;		// UART RX register로 부터 읽어온 1byte를 저장할 변수
	data = UDR1;			// UART1의 H/W register부터 1byte읽어서 data라는 변수에 저장
//	PORTE ^= 0xff;

#if 0
     //-----------  Circular start  
	inputq(data);
	//------------  Circular end 
#else	
	if (data == '\r' || data == '\n')   // string의 끝이면
	{
		buffer_1[j] = '\0';   // 종료 문자를 NULL
		rxReadyFlag_1 = 1;	// end message를 나타내는 flag를 set한다.
		j = 0;
	}
	else
	{
		buffer_1[j++] = data;   // UART로 부터 읽어온 byte를 string buffer_uart0에 save한다.
	}	
#endif 
}
void UART1_init(void)
{
	UBRR1H = 0x00;
	UBRR1L = 207;   // 9600 bps로 설정 
	
	UCSR1A |= (1 << U2X1);   // 2배속 통신	
	UCSR1C |= 0x06;  // Async(비동기). Data길이 8bits. Non Parity(패리티비트 사용 안함). stop 1 bit
	UCSR1B = ((1<<RXEN1)|(1<<TXEN1) | (1 << RXCIE1));   // #2. 송.수신 가능 하고록 설정을 하는것이다. 
	         // RXEN1 : UART1로 수신을 가능 하게 설정
			 // TXEN1 : UART1 송신을 가능 하게 설정
			 // #2. RXCIE1 : UART1로 부터 데이터(1Byte)가 들어 오면 수신(Rx) interrupt가 발생 되도록 설정
			 //  RXEN1 : 수신을 가능 하도록 설정  	TXEN1: 송신을 가능 하도록 설정  
}
/*
*  UART1로 1 byte를 보내는 함수 이다. 
*/  
void UART1_transmit(char data)
{
	while ( !(UCSR1A & (1 << UDRE1)) )
		;
	UDR1 = data;	
}
/*
**  UART1로부터 1 byte를 읽어오는 함수 이다.
*/
unsigned char UART1_receive(void)
{
/*#if 1
    //  76543210
	//         1
	//  10000000
	//  1
	if  ( (UCSR1A & (1 << RXC1)) == 0)    // PC 부터 data가 들어 왔는지를 check한다.  
		return 0;    // 없으면 0을 리턴한다. 
	else 
		return UDR1;		       // PC로 부터 들어온 data를 읽어서 리턴한다.
#else // orgianl code 
	*/while ( !(UCSR1A & (1 << RXC1)) )   // putty로 부터 data가 들어 올때까지 기다리는것이다. 
		;
	return UDR1;		                // PC로 부터 들어온 data를 읽어서 리턴한다. 
//#endif 
}
/*
*  UART1로 string을 출력 하는 함수
*/
void UART1_printf_string(char *str)
{
	for (int i=0; str[i] != '\0'; i++)
		UART1_transmit(str[i]);	
}