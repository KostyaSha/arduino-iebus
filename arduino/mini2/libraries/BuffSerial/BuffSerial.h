/*
  BuffSerial.h - serial with transmit buffer library for Wiring
  Created by Kochetkov Aleksey, 28.11.2009
  Version 0.1.2
*/

#ifndef BuffSerial_h
#define BuffSerial_h

#include <avr/pgmspace.h>
#include "Arduino.h"

#define TX_BUFF_SIZE		240               // max 65535 
#define RX_BUFF_SIZE		25                // max 255
#define TX_BUFF_MAX_LEN		TX_BUFF_SIZE - 1
#define BUFFSERIAL_VERSION "0.1.2"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class BuffSerial{
	public:
		uint8_t		rxBuffer[RX_BUFF_SIZE];
		uint8_t		rxBegin;
		uint8_t		rxEnd;
		uint8_t		txBuffer[TX_BUFF_SIZE];
		uint16_t	txBegin;
		uint16_t	txEnd;
		uint8_t		txFull;
		uint16_t	txOverflow;
		void		begin(long);
		void 		sendByte(uint8_t);
		void		print(const char*);
		void		print(const char);
		void		print_p(const char*);
		void		println(const char*);
		void		println(const char);
		void		println(void);
		void		println_p(const char*);
		void		printHex4(uint8_t);
		void		printHex8(uint8_t);
		void		printDec(uint8_t);
		bool		rxEnabled(void);
		uint8_t		rxRead(void);	
};

extern BuffSerial bSerial;

#endif

