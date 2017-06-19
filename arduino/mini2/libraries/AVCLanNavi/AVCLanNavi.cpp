/*
  AVCLanDrv.cpp - AVCLan CD changer library for 'duino / Wiring
  Created by Kochetkov Aleksey, 06.04.2010
  Version 0.1.1
*/

#include "AVCLanDrv.h"
#include "AVCLanNavi.h"
#include <avr/pgmspace.h>

AvcInMessageTable  mtMain[] PROGMEM = {
    {ACT_REGISTER,     0x03, {0x12, 0x01, 0x00}},
    {ACT_INIT,         0x03, {0x12, 0x01, 0x01}},
};

const byte mtMainSize = sizeof(mtMain) / sizeof(AvcInMessageTable);

AvcInMessageTable  mtSearchHead[] PROGMEM = {
    {ACT_REGISTER,  0x03, {0x12, 0x01, 0x00}},           // AVC LAN register
    {ACT_REGISTER,  0x03, {0x12, 0x01, 0x01}},           // AVC LAN init
    {ACT_REGISTER,  0x03, {0x01, 0x01, 0x58}},
    {ACT_REGISTER,  0x03, {0x01, 0x01, 0x5B}},
    {ACT_REGISTER,  0x04, {0x01, 0x01, 0x5F, 0x01}},
};
const byte mtSearchHeadSize = sizeof(mtSearchHead) / sizeof(AvcInMessageTable);

AvcOutMessage CmdReset       PROGMEM =  {AVC_MSG_BROADCAST,  0x05, {0x00, 0x00, 0x00, 0x00, 0x00}}; // reset AVCLan. This causes HU to send ACT_REGISTER
AvcOutMessage CmdRegister    PROGMEM =  {AVC_MSG_DIRECT,     0x0D, {0x00, 0x01, 0x12, 0x10, 0x58, 0x24, 0x5A, 0x85, 0x59, 0x6D, 0x57, 0x68, 0xAE}}; // register navi
AvcOutMessage CmdInit        PROGMEM =  {AVC_MSG_BROADCAST,  0x09, {0x6D, 0x31, 0xF1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00}}; // init command 

// AVCLan Navi init, 
void AVCLanNavi::begin(){
	avclan.deviceAddress = 0x0178;
}

// Use the last received message to determine the corresponding action ID, store it in avclan object
void AVCLanNavi::getActionID(){
	if (avclan.headAddress == 0){
		avclan.actionID = avclan.getActionID(mtSearchHead, mtSearchHeadSize);
	}else{
		avclan.actionID = avclan.getActionID(mtMain, mtMainSize);
	}
}; 

// process action
void AVCLanNavi::processAction(AvcActionID ActionID){
	byte r;
	switch (ActionID){
		case ACT_REGISTER:                                  // register device
			if (avclan.headAddress == 0) avclan.headAddress = avclan.masterAddress;
			avclan.sendMessage(&CmdRegister);
			break;
	}
};

// process event
void AVCLanNavi::processEvent(AvcEventID EventID){
	switch (EventID){
		case EV_STATUS:
			break;
	}
};

AVCLanNavi avclanDevice;