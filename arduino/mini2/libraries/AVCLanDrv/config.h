/*
  config.h  - AVCLan driver config
  Created by Kochetkov Aleksey, 04.08.2010
*/

// type of AVCLan hardwire driver
//#define AVCLAN_ST485
#define AVCLAN_PCA82C250
//#define AVCLAN_MINI_PCA82C250
//#define AVCLAN_RESISTOR

// maximum number of attempts to send a message
#define MAXSENDATTEMP	3

//********** arduino & driver based on ST485 **********
#ifdef AVCLAN_ST485
#define AVCLANDRIVER_TYPE    "Arduino - ST485"
// define out pin (pin 11 arduino) for ST485
#define DATAOUT_DDR	DDRB
#define DATAOUT_PORT	PORTB
#define	DATAOUT_PIN	PINB
#define DATAOUT		3

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

// LED connected to digital pin 13
#define LED_DDR		DDRB
#define LED_PORT	PORTB
#define	LED_PIN		PINB
#define LED_OUT		5

// AZFM board activate
#define AZFM_INIT
#define AZFM_ON  
#define AZFM_OFF 

#endif

//********** arduino & driver based on PCA82C250 **********
#ifdef AVCLAN_PCA82C250
#define AVCLANDRIVER_TYPE    "Arduino - PCA82C250"
// define out pin (pin 8 arduino) for AVCLAN_PCA82C250
#define DATAOUT_DDR	DDRB
#define DATAOUT_PORT	PORTB
#define	DATAOUT_PIN	PINB
#define DATAOUT		0

// define in pin (pin 9 arduino) for AVCLAN_PCA82C250
#define DATAIN_DDR	DDRB
#define DATAIN_PORT	PORTB
#define	DATAIN_PIN	PINB
#define DATAIN		1

// LED connected to digital pin 13
#define LED_DDR		DDRB
#define LED_PORT	PORTB
#define	LED_PIN		PINB
#define LED_OUT		5

// AZFM board activate
#define AZFM_INIT
#define AZFM_ON  
#define AZFM_OFF 

#endif

//********** avclan-mini board based on PCA82C250 **********
#ifdef AVCLAN_MINI_PCA82C250
#define AVCLANDRIVER_TYPE    "AVCLan-mini - PCA82C250"
// define out pin D7 for AVCLAN_MINI_PCA82C250
#define DATAOUT_DDR	DDRD
#define DATAOUT_PORT	PORTD
#define	DATAOUT_PIN	PIND
#define DATAOUT		7

// define in pin B0 for AVCLAN_MINI_PCA82C250
#define DATAIN_DDR	DDRB
#define DATAIN_PORT	PORTB
#define	DATAIN_PIN	PINB
#define DATAIN		0

// LED connected to digital pin 13
#define LED_DDR		DDRC
#define LED_PORT	PORTC
#define	LED_PIN		PINC
#define LED_OUT		0

// AZFM board activate
#define AZFM_DDR	DDRD
#define AZFM_PORT	PORTD
#define	AZFM_PIN	PIND
#define AZFMOUT		6
#define AZFM_INIT sbi(AZFM_DDR,  AZFMOUT); cbi(AZFM_PORT, AZFMOUT);
#define AZFM_ON   sbi(AZFM_PORT, AZFMOUT);
#define AZFM_OFF  cbi(AZFM_PORT, AZFMOUT);

#endif

//********** arduino & driver based on resistors **********
#ifdef AVCLAN_RESISTOR
#define AVCLANDRIVER_TYPE    "Arduino - resistor"
// define out pin (pin 6 arduino) for AVCLAN_RESISTOR
#define DATAOUT_DDR	DDRD
#define DATAOUT_PORT	PORTD
#define	DATAOUT_PIN	PIND
#define DATAOUT		6

// define in pin (pin 7 arduino) for AVCLAN_RESISTOR
#define DATAIN_DDR	DDRD
#define DATAIN_PORT	PORTD
#define	DATAIN_PIN	PIND
#define DATAIN		7

// LED connected to digital pin 13
#define LED_DDR		DDRB
#define LED_PORT	PORTB
#define	LED_PIN		PINB
#define LED_OUT		5

// AZFM board activate
#define AZFM_INIT
#define AZFM_ON  
#define AZFM_OFF 

#endif


