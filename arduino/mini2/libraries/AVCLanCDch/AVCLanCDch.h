/*
  AVCLanDrv.h - AVCLan CD changer library for 'duino / Wiring
  Created by Kochetkov Aleksey, 04.08.2010
  Version 0.2.2
*/

#ifndef AVCLanCDch_h
#define AVCLanCDch_h

#include <avr/pgmspace.h>
#include "AVCLanDrv.h"
#include "Arduino.h"

// timer1 overflow every 1 sec 
#define TI1_H	(((word)-(F_CPU / 1024)) >> 8)
#define TI1_L	(((word)-(F_CPU / 1024)) & 0xff )

#if defined(__AVR_ATmega8__)
#define ENABLE_TIMER1_INT  (sbi(TIMSK, TOIE1));
#define DISABLE_TIMER1_INT (cbi(TIMSK, TOIE1));
#else
#define ENABLE_TIMER1_INT  (sbi(TIMSK1, TOIE1));
#define DISABLE_TIMER1_INT (cbi(TIMSK1, TOIE1));
#endif

#define AVCLANDEVICE_NAME    " CD changer"
#define AVCLANDEVICE_VERSION "0.2.2"

//extern AvcOutMessage CmdReset; // reset AVCLan. This causes HU to send ACT_REGISTER
//extern AvcOutMessage CmdRegister; // register CD changer
// extern AvcOutMessage *CmdTest; // test message



typedef enum{
	ACT_REGISTER = 1,
	ACT_INIT,
	ACT_DEVSTATUS_E0,
	ACT_DEVSTATUS_E2,
	ACT_DEVSTATUS_E4,
	ACT_PLAY_REQ1,
	ACT_PLAY_REQ2,
	ACT_PLAY_IT,
	ACT_STOP_REQ1,
	ACT_STOP_REQ2,
	ACT_LAN_STATUS1,
	ACT_LAN_STATUS2,
	ACT_LAN_STATUS3,
	ACT_LAN_STATUS4,
	ACT_LAN_STATUS5,
	ACT_LAN_CHECK,
	ACT_SCAN_ON,
	ACT_SCAN_OFF,
	ACT_SCAN_D_ON,
	ACT_SCAN_D_OFF,
	ACT_REPEAT_ON,
	ACT_REPEAT_OFF,
	ACT_REPEAT_D_ON,
	ACT_REPEAT_D_OFF,
	ACT_RANDOM_ON,
	ACT_RANDOM_OFF,
	ACT_RANDOM_D_ON,
	ACT_RANDOM_D_OFF,
} AvcActionID;

typedef enum{
	EV_STATUS = 1,
} AvcEventID;

typedef enum{
	stWait = 0x03,
	stPlay = 0x10,
	stStop = 0x30,
} cdStatus;

typedef enum{
	pmScan    = 0x40,
	pmScanD   = 0x20,
	pmRepeat  = 0x10,
	pmRepeatD = 0x08,
	pmRandom  = 0x04,
	pmRandomD = 0x02,
	pmNormal  = 0x00,
} cdPlayMode;

class AVCLanCDch{
	public:
		byte		cd_min;                      // minutes play
		byte		cd_sec;                      // seconds play
		cdStatus	cd_status;                   // cd changer status
		byte		cd_playmode;                 // play mode (scan, random etc..)
		void		begin ();                    // initialisation, obligatory method
		void		getActionID();               // get action id by recieved message, obligatory method
		void		processAction(AvcActionID);  // process action, obligatory method
		void		processEvent(AvcEventID);    // process event, obligatory method
		byte		sendStatus();                // send CD-changer status to head
		byte		hexInc(byte data);
		byte		hexDec(byte data);
};
#endif

extern AVCLanCDch avclanDevice;
