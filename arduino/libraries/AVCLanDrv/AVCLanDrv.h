/*
  AVCLanDrv.h v.01 - AVCLan Library for 'duino / Wiring
  Created by Kochetkov Aleksey, 30.06.2009
*/

#ifndef AVCLanDrv_h
#define AVCLanDrv_h

#include "WProgram.h"
#include "config.h"

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
#define OUT_SET_1      (cbi(DATAOUT_PORT, DATAOUT));
#define OUT_SET_0      (sbi(DATAOUT_PORT, DATAOUT));
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
    AVC_MSG_NORMAL    = 1,
    AVC_MSG_BROADCAST = 0
} AvcTransmissionMode;

typedef enum
{
    ACT_NONE,
    
    // ACT_AUX_IN_USE,
    // ACT_TUNER_IN_USE,
    // ACT_TAPE_IN_USE,
    // ACT_CD_IN_USE,
    
    // ACT_EJECT_CD,
    // ACT_NO_CD,
    
    // ACT_STATUS,    
    // ACT_REGISTER,
    // ACT_INIT,    
    // ACT_CHECK
} AvcActionID;

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
		byte	dataSize;
		byte	message[AVC_MAXMSGLEN];
		void	begin ();
		byte	readMessage ();
		byte	sendMessage (void);
		void 	printMessage(bool incoming);
		bool	isAvcBusFree (void);
	private:
		bool	_parityBit;
		word	readBits (byte nbBits);
		void	sendStartBit (void);
		void	send1BitWord (bool data);
		void	send4BitWord (byte data);
		void	send8BitWord (byte data);
		void	send12BitWord (word data);
		bool	readAcknowledge (void);
		bool	handleAcknowledge (void);
};
#endif
