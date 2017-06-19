/*
 AVCLan mini, connecting 'duino to AVCLan bus. 
 Created by Kochetkov Aleksey, 04.08.2010
 Version 0.1.7
*/

#define AVCLAN_VERSION "0.1.7"

#include <EEPROM.h>
#include <AVCLanDrv.h>
#include <AVCLanCDch.h>
#include <BuffSerial.h>
#include <config.h>

byte readSeq = 0;
byte s_len	= 0;
byte s_dig	= 0;
byte s_c[2];
byte data_tmp[32];
byte i;

// адреса eeprom
#define E_MASTER1	0  // адрес головы, старший байт
#define E_MASTER2	1  // адрес головы, младший байт
#define E_READONLY  2  // режим "только чтение", если 1, то устройств не регистрируем, только слушаем шину
#define E_INIT		3  // признак инициализации EEPROM

#define LED_ON	sbi(LED_PORT, LED_OUT);
#define LED_OFF	cbi(LED_PORT, LED_OUT);

void setup(){
	// setup led
	sbi(LED_DDR,  LED_OUT);
	cbi(LED_PORT, LED_OUT);

	bSerial.begin(250000);
	avclan.begin();
	avclanDevice.begin();
	EERPOM_read_config();
	bSerial.print_p(PSTR("AVCLan mini. Kochetkov Aleksey. v"));
	bSerial.println(AVCLAN_VERSION);
	bSerial.println();
}

void loop(){

	if (INPUT_IS_SET){
		LED_ON;
		byte res = avclan.readMessage();
		LED_OFF;
		if (!res){
			if (!avclan.readonly) avclanDevice.getActionID();
			if (avclan.actionID != ACT_NONE) {
				avclanDevice.processAction((AvcActionID)avclan.actionID);
			}
		}
	}

	if (avclan.event != EV_NONE){
		avclanDevice.processEvent((AvcEventID)avclan.event);
		avclan.event = EV_NONE;
	}

	if (bSerial.rxEnabled()) {
		uint8_t readkey = bSerial.rxRead();

		switch (readkey){
		case 'P':                                  // print config
			bSerial.print_p(PSTR("head="));
			bSerial.printHex8(avclan.headAddress >> 8);
			bSerial.printHex8(avclan.headAddress);
			bSerial.print_p(PSTR(", eeprom="));
			bSerial.printHex8(EEPROM.read(E_MASTER1));
			bSerial.printHex8(EEPROM.read(E_MASTER2));
			bSerial.println();
			bSerial.print_p(PSTR("device="));
			bSerial.printHex8(avclan.deviceAddress >> 8);
			bSerial.printHex8(avclan.deviceAddress);
			bSerial.println();
			bSerial.print_p(PSTR("readonly="));
			bSerial.printHex8(avclan.readonly);
			bSerial.println();
			bSerial.print_p(PSTR("Overflow: "));
			bSerial.printHex8(bSerial.txOverflow >> 8);
			bSerial.printHex8(bSerial.txOverflow);
			bSerial.println();
			break;
		case 'T':  //send test message
			bSerial.println("S");
			sendMess();
			break;
		case 'M':  //readonly mode on/off
			avclan.readonly ^= (1 << 0);
			EEPROM.write(E_READONLY, avclan.readonly);
			break;
		case 'h':  // set hi level 
			bSerial.println("H set");
			AVC_OUT_EN;
			OUTPUT_SET_1;
			break;
		case 'l':  // set low level
			bSerial.println("L set");
			AVC_OUT_EN;
			OUTPUT_SET_0;
			break;
		case 'i': // output disable. Input mode on
			bSerial.println("Out dis");
			AVC_OUT_DIS;
			break;
		case 'V':                      // version
			bSerial.print_p  (PSTR("AVCLan mini. Kochetkov Aleksey. v"));
			bSerial.println_p(PSTR(AVCLAN_VERSION));
			bSerial.print_p  (PSTR(" Driver type: "));
			bSerial.println_p(PSTR(AVCLANDRIVER_TYPE));
			bSerial.print_p  (PSTR(" BuffServial v"));
			bSerial.println_p(PSTR(BUFFSERIAL_VERSION));
			bSerial.print_p  (PSTR(" AVCLanDrv v"));
			bSerial.println_p(PSTR(AVCLANDRV_VERSION));
			bSerial.print_p  (PSTR(AVCLANDEVICE_NAME));
			bSerial.print_p  (PSTR(". v"));
			bSerial.println_p(PSTR(AVCLANDEVICE_VERSION));
			break;
		case '?':
			bSerial.println_p(PSTR("P - config\r\nV - version\r\nM - readonly mode on/off\r\n-- send command --\r\nS - start\r\nW - end direct\r\nQ - end broadcast\r\nH - end of set Head ID, 0000 - auto\r\n-- debug command --\r\nh - Output mode. Set Hi level\r\nl - Output mode. Set Low level\r\ni - Input mode"));
			break;
		case 'S':	                   // start command
			readSeq = 1;       
			s_len=0;
			s_dig=0;
			s_c[0] = s_c[1] = 0;
			break;
		case 'W':                      // end of direct command
			readSeq=0;                     
			avclan.dataSize      = s_len;
			avclan.broadcast     = AVC_MSG_DIRECT;
			avclan.masterAddress = avclan.deviceAddress;
			avclan.slaveAddress  = avclan.headAddress;
			for (i=0; i<s_len; i++)
				avclan.message[i]=data_tmp[i];
			avclan.sendMessage();
			break;
		case 'Q' :                     // end of broadcast command
			readSeq=0;                     
			avclan.dataSize      = s_len;
			avclan.broadcast     = AVC_MSG_BROADCAST;
			avclan.masterAddress = avclan.deviceAddress;
			avclan.slaveAddress  = 0x01FF;
			for (i=0; i<s_len; i++)
				avclan.message[i]=data_tmp[i];
			avclan.sendMessage();
			break;
		case 'H' :                     // end of set Head Unid ID 
			readSeq=0;
			avclan.headAddress = (data_tmp[0] << 8) + data_tmp[1];
			EEPROM.write(E_MASTER1, data_tmp[0]);
			EEPROM.write(E_MASTER2, data_tmp[1]);
			break;
		default :
			if (readSeq==1) {
				if (readkey!=' ') {
					s_c[s_dig]=readkey;
	 				s_dig++;
	  				if (s_dig==2) {
						if (s_c[0]<':'){
							s_c[0] -= 48;
						}else{
							s_c[0] -= 55;
						}
						data_tmp[s_len] = 16 * s_c[0];
						if (s_c[1]<':'){
							s_c[1] -= 48;
						}else{
							s_c[1] -= 55;
						}
						data_tmp[s_len] += s_c[1];
						s_len++;
						s_dig=0;
						s_c[0]=s_c[1]=0;
					}
				}
			}
		}
	}
}

void sendMess(){
	avclan.broadcast = AVC_MSG_DIRECT;
	avclan.masterAddress = 0x0360;
	avclan.slaveAddress  = 0x0140;
	avclan.dataSize      = 0x05;
	avclan.message[0]    = 0x00;
	avclan.message[1]    = 0x01;
	avclan.message[2]    = 0x12;
	avclan.message[2]    = 0x10;
	avclan.message[3]    = 0x63;
	byte res = avclan.sendMessage();
}

// Чтение конфигурации из EEPROM
void EERPOM_read_config(){
	if (EEPROM.read(E_INIT) != 'T'){
		EEPROM.write(E_MASTER1, 0x01);
		EEPROM.write(E_MASTER2, 0x40);
		EEPROM.write(E_READONLY, 0);
		EEPROM.write(E_INIT, 'T');
	}else{
		avclan.headAddress = (EEPROM.read(E_MASTER1) << 8) + EEPROM.read(E_MASTER2);
		avclan.readonly    = EEPROM.read(E_READONLY);
	}
}


