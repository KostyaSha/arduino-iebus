/*
  AVCLanNavi.h - AVCLan Navi library for 'duino / Wiring
  Created by Kochetkov Aleksey, 16.06.2010
  Version 0.0.1
*/

#ifndef AVCLanNavi_h
#define AVCLanNavi_h

#include <avr/pgmspace.h>
#include "AVCLanDrv.h"
#include "WProgram.h"

#define AVCLANDEVICE_NAME    " Navi"
#define AVCLANDEVICE_VERSION "0.0.1"

extern AvcOutMessage CmdReset; // reset AVCLan. This causes HU to send ACT_REGISTER
extern AvcOutMessage CmdRegister; // register CD changer
// extern AvcOutMessage *CmdTest; // test message



typedef enum{
  ACT_REGISTER = 1,
  ACT_INIT,
} AvcActionID;

typedef enum{
  EV_STATUS = 1,
} AvcEventID;

class AVCLanNavi{
	public:
		void	begin ();  		             // initialisation, obligatory method
		void	getActionID();               // get action id by recieved message, obligatory method
		void	processAction(AvcActionID);  // process action, obligatory method
		void	processEvent(AvcEventID);    // process event, obligatory method
};
#endif

extern AVCLanNavi avclanDevice;
