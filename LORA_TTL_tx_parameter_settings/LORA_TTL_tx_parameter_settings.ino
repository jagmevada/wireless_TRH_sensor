/////////Serial is on usb
/////////Serial1 is on LORA
//////// Software serial tx on pin 2 for MODBUS RTU
#define MYID  15
#define nop() __asm__("nop\n\t")

//#include <SoftwareSerial.h>
//#define SOFT_RX 9
//#define SOFT_TX 8
//SoftwareSerial Serialx(SOFT_RX, SOFT_TX);  // RX, TX

#define M0_PIN  3
#define M1_PIN  2
#define AUX_PIN 1

/*
#define M0_PIN  8
#define M1_PIN  7
#define AUX_PIN 4
*/


unsigned char c1[3]={0xc1, 0xc1, 0xc1};
unsigned char c3[3]={0xc3, 0xc3, 0xc3};
unsigned char auxlevel=0, readytosend=0, onetime=0, updates=0, auxlevelo=0, runs=0;
unsigned long t0, t1;
void auxsignal();

void auxsignal(){
//auxlevel=digitalRead(AUX_PIN);
//if(auxlevelo==(!auxlevel))
updates=1;
Serial.write(0x22);
//auxlevelo=auxlevel;
}


//  C0 POWER DOWN SAVED PARAMETER AS FOLLOWS

//OPTION
#define TRANSPARENT 0  //default
#define FIXED_TX 0b10000000  //default
#define TXAUX_PUSHPULL_RX_PULLUP 0b01000000  //default
#define WAKE250MS 0  //default
#define FECON   0b00000100  //default
#define TX10DBM 0b00000011  //default
//SPED
#define UART_8N1 0  //default
#define UART_TTL_9600BPS 0b00011000 //default

#define UART_AIR_4800BPS 0b00000011 //10ms time time for 6 bytes
#define UART_AIR_2400BPS 0b00000010 //20ms time for 6 bytes default
#define UART_AIR_1200BPS 0b00000001 //40ms time
#define UART_AIR_1200BPS 0b00000000 //120ms time

#define HEAD 0xc0
#define ADDH 0xff
#define ADDL MYID
#define SPED (UART_8N1 | UART_TTL_9600BPS | UART_AIR_2400BPS)
#define CHAN 0x1e  // default 433MHz 0x17
#define OPTION (FIXED_TX | TXAUX_PUSHPULL_RX_PULLUP | WAKE250MS | FECON | TX10DBM)  

unsigned char c0[6]={HEAD,ADDH, ADDL, SPED, CHAN, OPTION};
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void setup() {  

pinMode(M0_PIN, OUTPUT);
pinMode(M1_PIN, OUTPUT);
digitalWrite(M0_PIN,1);
digitalWrite(M1_PIN,1);
pinMode(AUX_PIN, INPUT_PULLUP);
attachInterrupt(AUX_PIN, auxsignal, RISING);

Serial1.begin(9600);
Serial.begin(9600);
Serial.write(0x99);


delay(1000);
Serial.write(0x88);
Serial1.write(c0,6);  // SET PARAMETER C1 FF FF 1A 1E 17
delay(1000);
Serial.write(0x77);
if(updates){
  Serial1.write(c1,3);  // READ PARAMETER
updates=0;}
readytosend=0;
onetime=1;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


//##################################################
void loop(){

while(Serial1.available()>0){
  if(runs==0){
//Serial.println();
runs++;
  } 
  Serial.write(Serial1.read()); 
}
if(runs==1)
//Serial.println();
runs=0;
delay(1000);
nop();
}

//##################################################
