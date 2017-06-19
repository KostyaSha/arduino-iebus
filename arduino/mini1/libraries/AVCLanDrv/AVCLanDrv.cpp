/*
  AVCLanDrv.cpp v.01 - AVCLan Library for 'duino / Wiring
  Created by Kochetkov Aleksey, 30.06.2009
*/

#include "AVCLanDrv.h"
#include "BuffSerial.h"

// AVCLan driver & timer2 init, 
void AVCLanDrv::begin (){
	// AVCLan TX+/TX- 	read line INPUT
	cbi(DATAIN_DDR,  DATAIN);
#ifdef AVCLAN_ST485
	sbi(DATAIN_PORT, DATAIN);
#else 
	cbi(DATAIN_PORT, DATAIN);
#endif

	// AVCLan TX+/TX-		write line OUTPUT	
#ifdef AVCLAN_RESISTOR
	cbi(DATAOUT_DDR,  DATAOUT);
	cbi(DATAOUT_PORT, DATAOUT);
	cbi(ADCSRB, ACME);	// Analog Comparator Multiplexer Enable - NO
	cbi(ACSR, ACIS1);	// Analog Comparator Interrupt Mode Select
	cbi(ACSR, ACIS0);  // Comparator Interrupt on Output Toggle
	cbi(ACSR, ACD); 	// Analog Comparator Disbale - NO
#else 
	sbi(DATAOUT_DDR,  DATAOUT);
	sbi(OUTEN_DDR,  OUTEN);
	AVC_OUT_DIS;
	OUTPUT_SET_0;
#endif
	// timer2 setup, prescaler factor - 8
#if defined(__AVR_ATmega8__)
//	ASSR=0x00;
	TCCR2=0x02;
//	TCNT2=0x00;
//	OCR2=0x00;
#else   // ATMega168
//	ASSR=0x00;
//	TCCR2A=0x00;
	TCCR2B=0x02;
//	TCNT2=0x00;
//	OCR2A=0x00;
//	OCR2B=0x00;
#endif
	deviceAddress = 0x0000;
}

// Reads specified number of bits from the AVCLan.
// nbBits (byte) -> Number of bits to read.
// Return (word) -> Data value read.
word AVCLanDrv::readBits (byte nbBits){
	word data  = 0;
	_parityBit = 0;
	
	while (nbBits-- > 0){
		// Insert new bit
		data <<= 1;
		// Wait until rising edge of new bit.
		while (INPUT_IS_CLEAR){
			// Reset watchdog.
			//wdt_reset();
		}

		// Reset timer to measure bit length.
		TCNT2 = 0;
		// Wait until falling edge.
		while (INPUT_IS_SET);
		// Compare half way between a '1' (20 us) and a '0' (32 us ): 32 - (32 - 20) /2 = 26 us
		if (TCNT2 < AVC_BIT_0_HOLD_ON_MIN_LENGTH){
			// Set new bit.
			data |= 0x0001;
			// Adjust parity.
			_parityBit = ! _parityBit;
		}
	}
	return data;
}

// Read incoming messages on the AVCLan.
// Return true if success.
byte AVCLanDrv::readMessage (){
	uint8_t t = 0;
	uint8_t oldSREG = SREG;
	cli();             // disable interrupts

	// Start bit.
	while (INPUT_IS_CLEAR);
	TCCR2B=0x03;      // prescaler 32
	TCNT2 = 0;
	// Wait until falling edge.
	while (INPUT_IS_SET){
		t = TCNT2;
		if (t > 0xFF) {
			SREG = oldSREG;
			return 1;
		}
	}
	TCCR2B=0x02;      // prescaler 8
	
	if (t < AVC_START_BIT_HOLD_ON_MIN_LENGTH){
	//if (t < 0x16){
			SREG = oldSREG;
			return 2;
	}

	broadcast = readBits(1);

	masterAddress = readBits(12);
	bool p = _parityBit;
	if (p != readBits(1)){
		SREG = oldSREG;
		return 3;
	}
  
	slaveAddress = readBits(12);       
	p = _parityBit;
	if (p != readBits(1)){
		SREG = oldSREG;
		return 4;
	}

	bool forMe = ( slaveAddress == deviceAddress );

	if (forMe){
		// Send ACK.
		send1BitWord(0);
	}else{  
		readBits(1);
	}

	// Control 
	readBits(4);
	p = _parityBit;
	if (p != readBits(1)){
		SREG = oldSREG;
		return 5;
	}

	if (forMe){
		// Send ACK.
		send1BitWord(0);
	}else{  
		readBits(1);
	}

	dataSize = readBits(8);
	p = _parityBit;
	if (p != readBits(1)){
		SREG = oldSREG;
		return 6;
	}

	if (forMe){
		// Send ACK.
		send1BitWord(0);
	}else{  
		readBits(1);
	}
	if (dataSize > AVC_MAXMSGLEN){
		SREG = oldSREG;
		return 7;
	}
	byte i;
	for (i = 0; i < dataSize; i++ ){
		message[i] = readBits(8);
		p = _parityBit;
		if (p != readBits(1)){
			SREG = oldSREG;
			return 8;
		}

		if (forMe){
			// Send ACK.
			send1BitWord(0);
		}else{  
			readBits(1);
		}
	}
	SREG = oldSREG;
	return 0;   
}

// Send a start bit to the AVCLan
void AVCLanDrv::sendStartBit (){
	// Reset timer to measure bit length.
	TCCR2B=0x03;      // prescaler 32
	TCNT2 = 0;
	OUTPUT_SET_1;

	// Pulse level high duration.
	while ( TCNT2 < AVC_START_BIT_HOLD_ON_LENGTH );
	OUTPUT_SET_0;

	// Pulse level low duration until ~185 us.
	while ( TCNT2 < AVC_START_BIT_LENGTH );
	TCCR2B=0x02;      // prescaler 8

}

// Send a 1 bit word to the AVCLan
void AVCLanDrv::send1BitWord (bool data){
	// Reset timer to measure bit length.
	TCNT2 = 0;
	OUTPUT_SET_1;

	if (data){
		while (TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH);
	}else{
		while (TCNT2 < AVC_BIT_0_HOLD_ON_LENGTH);
	}

	OUTPUT_SET_0;
	while (TCNT2 <  AVC_NORMAL_BIT_LENGTH);
}

// Send a 4 bit word to the AVCLan
void AVCLanDrv::send4BitWord (byte data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 4; nbBits++ ){
		// Reset timer to measure bit length.
		TCNT2 = 2;
		OUTPUT_SET_1;

		if (data & 0x8){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( TCNT2 < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		OUTPUT_SET_0;
		// Hold output low until end of bit.
		while ( TCNT2 < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

// Send a 8 bit word to the AVCLan
void AVCLanDrv::send8BitWord (byte data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 8; nbBits++ ){
		// Reset timer to measure bit length.
		TCNT2 = 2;
		OUTPUT_SET_1;

		if (data & 0x80){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( TCNT2 < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		OUTPUT_SET_0;
		// Hold output low until end of bit.
		while ( TCNT2 < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

// Send a 12 bit word to the AVCLan
void AVCLanDrv::send12BitWord (word data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 12; nbBits++ ){
		// Reset timer to measure bit length.
		TCNT2 = 2;
		OUTPUT_SET_1;

		if (data & 0x0800){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( TCNT2 < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		OUTPUT_SET_0;
		// Hold output low until end of bit.
		while ( TCNT2 < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

//  determine whether the bus is free (no tx/rx).
//  return TRUE is bus is free.
bool AVCLanDrv::isAvcBusFree (void){
	// Reset timer.
	TCNT2 = 0;

	while (INPUT_IS_CLEAR){
		// We assume the bus is free if anything happens for the length of 1 bit.
		if (TCNT2 > AVC_NORMAL_BIT_LENGTH){   
			return true;
		}
	}
    
	return false;
}

// reads the acknowledge bit the AVCLan
// return TRUE if ack detected else FALSE.
bool AVCLanDrv::readAcknowledge (void){
	// The acknowledge pattern is very tricky: the sender shall drive the bus for the equivalent
	// of a bit '1' (20 us) then release the bus and listen. At this point the target shall have
	// taken over the bus maintaining the pulse until the equivalent of a bit '0' (32 us) is formed.

	// Reset timer to measure bit length.
	TCNT2 = 0;
	OUTPUT_SET_1;

	// Generate bit '0'.
	while (TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH);
	OUTPUT_SET_0;
	
	AVC_OUT_DIS;
	
	while (TCNT2 < AVC_BIT_1_HOLD_ON_LENGTH + AVC_1U_LENGTH);
	// Measure final resulting bit.
	while ( INPUT_IS_SET );

	// Sample half-way through bit '0' (26 us) to detect whether the target is acknowledging.
	if (TCNT2 > AVC_BIT_0_HOLD_ON_MIN_LENGTH){
		// Slave is acknowledging (ack = 0). Wait until end of ack bit.
		while (INPUT_IS_SET );
		AVC_OUT_EN;
		return true;
	}

	// No sign of life on the bus.
	return false;    
}

// sends ack bit if I am broadcasting otherwise wait and return received ack bit.
// return FALSE if ack bit not detected.
bool AVCLanDrv::handleAcknowledge (void){
	if (broadcast == AVC_MSG_BROADCAST){   
		// Acknowledge.    
		send1BitWord(0);
		return true;
	}

	// Return acknowledge bit.
	return readAcknowledge();
} 

// sends the message in global registers on the AVC LAN bus.
// return TRUE if successful else FALSE.
byte AVCLanDrv::sendMessage (void){  
	uint8_t oldSREG = SREG;
	cli();             // disable interrupts
	while (!isAvcBusFree());
	
	AVC_OUT_EN;
	
	// Send start bit.
	sendStartBit();

	// Broadcast bit.
	send1BitWord(broadcast);

	// Master address = me.
	send12BitWord(masterAddress);
	send1BitWord(_parityBit);

	// Slave address = head unit (HU).
	send12BitWord(slaveAddress);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		AVC_OUT_DIS;
		SREG = oldSREG;
		return 1;
	}

	// Control flag + parity.
	send4BitWord(AVC_CONTROL_FLAGS);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		AVC_OUT_DIS;
		SREG = oldSREG;
		return 2;
	}

	// Data length + parity.
	send8BitWord(dataSize);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		AVC_OUT_DIS;
		SREG = oldSREG;
		return 3;
	}

	for (byte i = 0; i < dataSize; i++){
		send8BitWord(message[i]);
		send1BitWord(_parityBit);
		if (!handleAcknowledge()){
			AVC_OUT_DIS;
			SREG = oldSREG;
			return false;
		}
	}
	AVC_OUT_DIS;
	SREG = oldSREG;
	return 0;
}

// print message to serial
void AVCLanDrv::printMessage(bool incoming){
	if (incoming){
		bSerial.print("< ");
	}else{
		bSerial.print("> ");
	}  
	if (broadcast == AVC_MSG_BROADCAST){
		bSerial.print("b ");
	}else{ 
		bSerial.print("d ");
	}
	bSerial.printHex4(masterAddress >> 8);
	bSerial.printHex8(masterAddress);
	bSerial.print(" ");

	bSerial.printHex4(slaveAddress >> 8);
	bSerial.printHex8(slaveAddress);
	bSerial.print(" ");
	bSerial.printHex8(dataSize);

	for (byte i = 0; i < dataSize; i++){
		bSerial.printHex8(message[i]);
	}
	bSerial.println(); 

}
