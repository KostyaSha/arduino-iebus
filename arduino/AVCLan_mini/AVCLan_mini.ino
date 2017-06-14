
#include <AVCLanDrv.h>
#include <BuffSerial.h>
#include <config.h>


int ledPin = 13;                 // LED connected to digital pin 13
AVCLanDrv avclan;
bool first;
byte t;
byte count;

void setup(){
  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
  bSerial.begin(250000);
  avclan.begin();
  bSerial.println("AVCLan");
  first = true;
}

void loop(){


  if (INPUT_IS_SET){
//    LedOn();
    byte res = avclan.readMessage();
    if (!res){
      avclan.printMessage(true);
    }else{
      bSerial.print("R");
      bSerial.printHex4(res);
      bSerial.println();
      while (!avclan.isAvcBusFree());
    }
//    LedOff();
  }else{
    if (first){
      first = false;
      avclan.deviceAddress = 0x240;
      avclan.broadcast = AVC_MSG_BROADCAST;
      avclan.masterAddress = 0x0360;
      avclan.slaveAddress  = 0x01FF;
      avclan.dataSize      = 0x04;
      avclan.message[0]    = 0x01;
      avclan.message[1]    = 0x11;
      avclan.message[2]    = 0x13;
      avclan.message[3]    = 0x63;
//      byte res = avclan.sendMessage();
//      if (!res){
//        avclan.printMessage(false);
//      }else{
//        bSerial.print("W");
//        bSerial.printHex4(res);
//        bSerial.println();
//      }
    }  
  }
  if (bSerial.rxEnabled()) {
    cbi(UCSR0B, RXCIE0);								// disable RX complete interrupt
    uint8_t readkey = bSerial.rxRead();

    switch (readkey){
      case 'P':                                  // print config
        bSerial.print("Overflow: ");
        bSerial.printHex8(bSerial.txOverflow >> 8);
        bSerial.printHex8(bSerial.txOverflow);
        bSerial.println();
        break;
      case 'M':
        count = 0;     
        while(1){
          while (INPUT_IS_SET);
          uint8_t oldSREG = SREG;
          cli();             // disable interrupts
          while (INPUT_IS_CLEAR);
          TCNT2 = 0;
          while (INPUT_IS_SET){
            t = TCNT2;
            if (t > 0xF0) break;
          };
          t = t / 2; 
          SREG = oldSREG;
          bSerial.printDec(t);
          bSerial.println();
          count++;
          if (count > 30){
            delay(300);
            count = 0;
          }
        }
        break;
      case 'S':
        bSerial.println("S");
        sendMess();
        break;
      case 'H':
        bSerial.println("H set");
        AVC_OUT_EN;
        OUTPUT_SET_1;
        break;
      case 'L':
        bSerial.println("L set");
        AVC_OUT_EN;
        OUTPUT_SET_0;
        break;
      case 'D':
        bSerial.println("Out dis");
        AVC_OUT_DIS;
        break;
    }
  }
}

void sendMess(){
      avclan.deviceAddress = 0x360;
      avclan.broadcast = AVC_MSG_NORMAL;
      avclan.masterAddress = 0x0360;
      avclan.slaveAddress  = 0x0140;
      avclan.dataSize      = 0x05;
      avclan.message[0]    = 0x00;
      avclan.message[1]    = 0x01;
      avclan.message[2]    = 0x12;
      avclan.message[2]    = 0x10;
      avclan.message[3]    = 0x63;
      byte res = avclan.sendMessage();
      if (!res){
        avclan.printMessage(false);
      }else{
        bSerial.print("W");
        bSerial.printHex4(res);
        bSerial.println();
      }
} 

void LedOn(){
  digitalWrite(ledPin, HIGH);
}

void LedOff(){
  digitalWrite(ledPin, LOW);
}
