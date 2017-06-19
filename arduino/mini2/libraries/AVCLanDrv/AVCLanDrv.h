/*
  AVCLanDrv.h - AVCLan Library for 'duino / Wiring
  Created by Kochetkov Aleksey, 04.08.2010
  Version 0.3.1
*/

#ifndef AVCLanDrv_h
#define AVCLanDrv_h

#include "Arduino.h"
#include "config.h"

#define AVCLANDRV_VERSION "0.3.1"


#ifdef AVCLAN_RESISTOR
// avclan driver on resistor
#define INPUT_IS_SET   (ACSR & _BV(ACO))
#define INPUT_IS_CLEAR (!(ACSR & _BV(ACO)))
#define OUTPUT_SET_1   sbi(PORTD, DATAOUT);
#define OUTPUT_SET_0   cbi(PORTD, DATAOUT);
#define AVC_OUT_EN     sbi(PORTD, DATAOUT); sbi(DDRD, DATAOUT);  sbi(DDRD, DATAIN); sbi(ACSR, ACD); 
#define AVC_OUT_DIS    cbi(PORTD, DATAOUT); cbi(DDRD, DATAOUT);  cbi(DDRD, DATAIN); cbi(ACSR, ACD);
#else
#ifdef AVCLAN_ST485
// avclan driver on ST485
#define INPUT_IS_SET   (bit_is_clear(DATAIN_PIN, DATAIN))
#define INPUT_IS_CLEAR (bit_is_set(DATAIN_PIN, DATAIN))
#define OUTPUT_SET_1   (cbi(DATAOUT_PORT, DATAOUT));
#define OUTPUT_SET_0   (sbi(DATAOUT_PORT, DATAOUT));
#define AVC_OUT_EN     (sbi(OUTEN_PORT, OUTEN));; 
#define AVC_OUT_DIS    (cbi(OUTEN_PORT, OUTEN));;
#else
//avclan driver on PCA82C250 & LM239N
#define INPUT_IS_SET   (bit_is_set(DATAIN_PIN, DATAIN))
#define INPUT_IS_CLEAR (bit_is_clear(DATAIN_PIN, DATAIN))
#define OUTPUT_SET_1   (cbi(DATAOUT_PORT, DATAOUT));
#define OUTPUT_SET_0   (sbi(DATAOUT_PORT, DATAOUT));
#define AVC_OUT_EN     ; 
#define AVC_OUT_DIS    ;
#endif
#endif

#define AVC_NORMAL_BIT_LENGTH           	0x4A  // 37 * (F_CPU / 1000000L / 8) 
#define AVC_BIT_1_HOLD_ON_LENGTH			0x28  // 20 uS * (F_CPU / 1000000L / 8) 
#define AVC_BIT_0_HOLD_ON_LENGTH			0x40  // 32 uS * (F_CPU / 1000000L / 8)
//#define AVC_BIT_0_HOLD_ON_MIN_LENGTH		0x34  // 26 uS * (F_CPU / 1000000L / 8)    Compare half way between a '1' (20 us) and a '0' (32 us ): 32 - (32 - 20) /2 = 26 us
#define AVC_BIT_0_HOLD_ON_MIN_LENGTH		0x3C  // 30 uS * (F_CPU / 1000000L / 8)    Compare half way between a '1' (20 us) and a '0' (32 us ): 32 - (32 - 20) /2 = 26 us
#define AVC_START_BIT_LENGTH				0x5D  // 186 uS  * (F_CPU / 1000000L / 32) ,  prescaler 32
#define AVC_START_BIT_HOLD_ON_LENGTH		0x54  // 168 uS * (F_CPU / 1000000L / 32)    prescaler 32
#define AVC_START_BIT_HOLD_ON_MIN_LENGTH	0x16  // 44 uS * (F_CPU / 1000000L / 32)      grater that AVC_NORMAL_BIT_LENGTH,  prescaler 32
#define AVC_1U_LENGTH	                    0x02  // 1 uS * (F_CPU / 1000000L / 8)      

#define AVC_MAXMSGLEN		32
#define AVC_CONTROL_FLAGS	0xF

typedef enum
{   // No this is not a mistake, broadcast = 0!
    AVC_MSG_DIRECT    = 1,
    AVC_MSG_BROADCAST = 0
} AvcTransmissionMode;

#define ACT_NONE 0  // no action
#define EV_NONE	 0	// no event

typedef struct
{
	byte	actionID;           // Action id
	byte	dataSize;           // message size (bytes)
	byte	data[12];           // message
} AvcInMessageTable;

typedef struct
{
	byte	actionID;           // Action id
	byte	dataSize;           // message size (bytes)
	byte	data[14];           // message
	word	mask;				// mask, set bit = 1 in not checked position (1<<5 or _BV(5) - datap[5] not checked)
} AvcInMaskedMessageTable;

typedef struct
{
	AvcTransmissionMode broadcast;          // Transmission mode: normal (1) or broadcast (0).
    byte                dataSize;           // message size (bytes)
    byte                data[14];           // message
} AvcOutMessage;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class AVCLanDrv{
	public:
		bool	broadcast;
		word	masterAddress;
		word	slaveAddress;
		word	deviceAddress;
		word	headAddress;
		byte	dataSize;
		byte	message[AVC_MAXMSGLEN];
		byte	event;
		byte	actionID;
		bool	readonly;
		void	begin ();
		byte	readMessage (void);
		byte	sendMessage (void);
		byte	sendMessage (AvcOutMessage*);
		void 	printMessage (bool incoming);
		bool	isAvcBusFree (void);
		byte	getActionID (AvcInMessageTable messageTable[], byte mtSize);
		byte	getActionID (AvcInMaskedMessageTable messageTable[], byte mtSize);
		void	loadMessage (AvcOutMessage*);
	private:
		bool	_parityBit;
		word	readBits (byte nbBits);
		byte	_readMessage (void);
		byte	_sendMessage (void);
		void	sendStartBit (void);
		void	send1BitWord (bool data);
		void	send4BitWord (byte data);
		void	send8BitWord (byte data);
		void	send12BitWord (word data);
		bool	readAcknowledge (void);
		bool	handleAcknowledge (void);
};

extern AVCLanDrv avclan;
#endif
