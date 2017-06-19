/*
  BuffSerial.cpp - serial with transmit buffer library for Wiring
  Created by Kochetkov Aleksey, 28.11.2009
  Version 0.1.2
*/
#include <stdio.h>

#include "BuffSerial.h"

// serial init
void BuffSerial::begin(long speed){
#if defined(__AVR_ATmega8__)
	UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXCIE) | _BV(TXEN);   // enable rx, tx inerrputs
	UBRRH = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;       // usart speed
	UBRRL = ((F_CPU / 16 + speed / 2) / speed - 1);
#else
	UCSR0B = (_BV(RXCIE0) | _BV(RXEN0) | _BV(TXCIE0) | _BV(TXEN0));  // enable rx, tx inerrputs
	UBRR0H = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;            // usart speed
	UBRR0L = ((F_CPU / 16 + speed / 2) / speed - 1);
#endif
	rxBegin = rxEnd = 0;
	txBegin = txEnd = txOverflow = 0;
	txFull  = 0;
}

//USART Rx Complete
#if defined(__AVR_ATmega8__)
SIGNAL(SIG_UART_RECV)
#else
SIGNAL(USART_RX_vect)
#endif
{
#if defined(__AVR_ATmega8__)
	bSerial.rxBuffer[bSerial.rxEnd] = UDR;
#else
	bSerial.rxBuffer[bSerial.rxEnd] = UDR0;
#endif
	if (bSerial.rxEnd < RX_BUFF_SIZE) bSerial.rxEnd++;
}

//USART Tx Complete
#if defined(__AVR_ATmega8__)
SIGNAL(SIG_UART_TRANS)
#else
SIGNAL(USART_TX_vect)
#endif
{
	if (bSerial.txEnd != bSerial.txBegin || bSerial.txFull != 0){
#if defined(__AVR_ATmega8__)
		UDR  = bSerial.txBuffer[bSerial.txBegin];  // Send buffer
#else
		UDR0 = bSerial.txBuffer[bSerial.txBegin];  // Send buffer
#endif
		bSerial.txFull = 0;
		bSerial.txBegin++;
		if (bSerial.txBegin == TX_BUFF_SIZE) bSerial.txBegin = 0;
	}

}

// send byte to serial or buffer if bisy
void BuffSerial::sendByte(uint8_t data){
	if (txFull){
		txOverflow++;
	}else{
		uint8_t oldSREG = SREG;
		cli();
#if defined(__AVR_ATmega8__)
		if (txEnd != txBegin || (UCSRA & _BV(UDRE)) == 0){
#else
		if (txEnd != txBegin || (UCSR0A & _BV(UDRE0)) == 0){
#endif
			txBuffer[txEnd] = data;
			txEnd++;
			if (txEnd == TX_BUFF_SIZE) txEnd = 0;
			if (txEnd == txBegin) txFull = 1;          // buffer overflow
		}else{
#if defined(__AVR_ATmega8__)
			UDR  = data;
#else
			UDR0 = data; 
#endif
		}
		SREG = oldSREG;
	}
}

// print string
void BuffSerial::print(const char *pBuf){
	while (*pBuf)	{
		sendByte(*pBuf++);
	}
}

void BuffSerial::print(const char pBuf){
	sendByte(pBuf);
}

// print string from flash
void BuffSerial::print_p(const char *pBuf){
	char c;
	while ((c = pgm_read_byte_near( pBuf++ )))	{
		sendByte(c);
	}
}

void BuffSerial::println(const char *pBuf){
	print(pBuf);
	println();
}

void BuffSerial::println(const char pBuf){
	print(pBuf);
	println();
}

void BuffSerial::println(void){
	print("\r\n");
}

void BuffSerial::println_p(const char *pBuf){
	print_p(pBuf);
	println();
}

void BuffSerial::printHex4(uint8_t data){
	uint8_t c = data & 0x0f;
	c += c < 10 ? '0' : 'A' - 10 ;
	sendByte(c);
}

void BuffSerial::printHex8(uint8_t data){
    printHex4(data >> 4);
    printHex4(data);
} 

void BuffSerial::printDec(uint8_t data){
	uint8_t buf[3]; 
	uint8_t i = 0;
	if (data == 0){
		sendByte('0');
		return;
	} 

	while (data > 0){
		buf[i++] = data % 10;
		data /= 10;
	}
	for (; i > 0; i--)
		sendByte((buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
}

// check rx buffer not empty
bool BuffSerial::rxEnabled(void){  
	return rxEnd;
}

uint8_t BuffSerial::rxRead(void){
#if defined(__AVR_ATmega8__)
	cbi(UCSRB, RXCIE);                           // disable RX complete interrupt
#else
	cbi(UCSR0B, RXCIE0);                         // disable RX complete interrupt
#endif
	uint8_t readkey = rxBuffer[rxBegin];         // read begin of received Buffer
	rxBegin++;
	if (rxBegin == rxEnd) rxBegin = rxEnd = 0;   // if Buffer is empty reset Buffer
#if defined(__AVR_ATmega8__)
	sbi(UCSRB, RXCIE);                           // enable RX complete interrupt
#else
	sbi(UCSR0B, RXCIE0);                         // enable RX complete interrupt
#endif

	return readkey;
}

BuffSerial bSerial;
