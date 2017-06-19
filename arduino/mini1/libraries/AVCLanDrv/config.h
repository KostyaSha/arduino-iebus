/*
  config.h  - AVCLan driver config
  Created by Kochetkov Aleksey, 30.06.2009
*/

// type of AVCLan hardwire driver
#define AVCLAN_ST485
//#define AVCLAN_PCA82C250
//#define AVCLAN_RESISTOR

#ifdef AVCLAN_ST485
// define out pin (pin 11 arduino) for ST485
#define DATAOUT_DDR		DDRB
#define DATAOUT_PORT	PORTB
#define	DATAOUT_PIN		PINB
#define DATAOUT			3

// define out control pin (pin 10 arduino) for ST485
#define OUTEN_DDR	DDRB
#define OUTEN_PORT	PORTB
#define	OUTEN_PIN	PINB
#define OUTEN		2

// define in pin (pin 9 arduino) for ST485
#define DATAIN_DDR	DDRB
#define DATAIN_PORT	PORTB
#define	DATAIN_PIN	PINB
#define DATAIN		1
#endif

// define out pin (pin 6 arduino) for AVCLAN_RESISTOR
// #define DATAOUT_DDR		DDRD
// #define DATAOUT_PORT	PORTD
// #define	DATAOUT_PIN		PIND
// #define DATAOUT			6

// define in pin (pin 7 arduino) for AVCLAN_RESISTOR
// #define DATAIN_DDR	DDRD
// #define DATAIN_PORT	PORTD
// #define	DATAIN_PIN	PIND
// #define DATAIN		7


