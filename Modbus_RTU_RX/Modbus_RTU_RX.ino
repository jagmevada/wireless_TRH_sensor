
/////////Serial is on usb 
/////////Serial1 is on LORA
//////// Software serial tx on pin 2 for MODBUS RTU
#include "HardwareSerial.h"
#include <SimpleModbusSlave.h>


#define ENABLEPIN 10
#define nop() __asm__("nop\n\t")
#define MYID  0xFF  // RECEIVER

#define M0_PIN  3
#define M1_PIN  2
#define AUX_PIN 1
#define HEAD 0xc0
#define ADDH 0xcc
#define ADDL 0xff
#define CHAN 0x1e

//////////////////////////////
#define SLAVE_ID 10
#define FLAG 208
#define HOLDING_REGS_SIZE 60
unsigned int holdingRegs[HOLDING_REGS_SIZE]; 
unsigned char modbusupdate=0;

unsigned int timercount=0,sensor_update=0, acquired=0;
unsigned char datar[5]={0x00, 0x01, 0x02, 0x03,0x04};
unsigned char c1[3]={0xc1, 0xc1, 0xc1};
unsigned char auxlevel=0, readytosend=0, onetime=0, updates=0, auxlevelo=0, runs=0;
unsigned long t0, t1;
////////////////////////////
float hl, tl,hr,tr;
uint8_t hl0, hl1, tl0, tl1;
unsigned char rc=0, rc0=0, rc1=0, rc2=0, rc3=0, rtmp=0, rold=0, rlock=0, rcount=0;
///////////////////////////
void local_ht_print();
void auxsignal();
void lora_setting_check();
void lora_receive_data();

//fffffffffffffffffffffffffffffffffffff
void auxsignal(){
updates=1;
}
/////////////////////
void lora_setting_check(){
  while(Serial1.available()>0){
  if(runs==0){
runs++;
  }
  Serial.printf("\t%.2X",(Serial1.read()));
}
if(runs==1){
Serial.println();
runs=0;
}
}
////////////////////////////
void lora_receive_data(){
while(Serial1.available()>0){
  rtmp=Serial1.read();
if(rtmp>HEAD && (!rlock)){// && old==ADDL){
    rlock=1;
    rcount=0;
}  
  if(rlock){
    switch(rcount){
      case 0:
      rc=(rtmp & 0x3f);// rxid
      break;
      case 1:
      rc0=rtmp;//temp
      break;
      case 2:
      rc1=rtmp;//humidity
      break;
      case 3:
      rc2=rtmp;//temp1
      break;
      case 4:
      rc3=rtmp;//humidity1
      rlock=0;
      acquired=1;
      break;
      default:
      break;
    }
    rcount++;
  }
  else
  ;

if(0){
Serial.print("RX\t");
Serial.print(rc);
Serial.print("\t");
Serial.print(rc0);
Serial.print('.');
Serial.print(rc2);
Serial.print("\t\t");
Serial.print(rc1);
Serial.print('.');
Serial.println(rc3);
acquired=0;
}
}
}
//fffffffffffffffffffffffffffffffffffff

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void setup() { 
  modbus_configure(&Serial, 9600, SERIAL_8E1, SLAVE_ID, ENABLEPIN, HOLDING_REGS_SIZE, holdingRegs);
//pinMode(ENABLEPIN, OUTPUT);
pinMode(M0_PIN, OUTPUT);
pinMode(M1_PIN, OUTPUT);

//digitalWrite(ENABLEPIN,1);  // RS485 ONLY WRITE //NO MODBUS
digitalWrite(M0_PIN,1);
digitalWrite(M1_PIN,1);
pinMode(AUX_PIN, INPUT_PULLUP);
attachInterrupt(AUX_PIN, auxsignal, RISING);
nop();nop();nop();
Serial1.begin(9600);
//Serial.begin(9600);
//Serial.write('A');
delay(1000);
updates=1;
if(updates){
//  Serial.write('I');
//  Serial1.write(c1,3);  // READ PARAMETER
updates=0;
}
////////////////////////////
TCB0.CCMP = 16000;  // main clock 16MHz, 2ms timer 
TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm | TCB_RUNSTDBY_bm;
TCB0.INTCTRL = TCB_CAPT_bm;
////////////////////////////
delay(1000);
onetime=1;
//lora_setting_check();
///////////////receive mode
digitalWrite(M0_PIN,0);
digitalWrite(M1_PIN,0);
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

float tr_check, hr_check;

//##################################################
void loop(){
lora_receive_data();  /// should use falling edge of auxsignal
modbus_update();
if(acquired){
if(rc<31){
tr_check =(float) rc0+ ((float) rc2)/100.0;
hr_check =(float) rc1+ ((float) rc3)/100.0;

if(tr_check>10 && tr_check<50)
tr=tr_check;
if(hr_check>20 && hr_check<100)
hr=hr_check;

holdingRegs[2*(rc-1)]=(int) (tr*100);
holdingRegs[2*(rc-1)+1]=(int) (hr*100);
}
acquired=0;
}
delay(1);
nop();
}
//##################################################


unsigned int timeout=0;
//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
ISR(TCB0_INT_vect) { // on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
 switch(timercount)
  {
    case 1:
    sensor_update=1;
         break;
    case 501:  // 500/msecond
      timercount=0;
      timeout++;
      break;
  default:
  break;
  }
  timercount++;
  if(timeout>50);
  timerout=0;
 TCB0.INTFLAGS = TCB_CAPT_bm; /* Clear the interrupt flag */
}

//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII


//#include <SoftwareSerial.h>
//#define SOFT_RX 9
//#define SOFT_TX 8
//SoftwareSerial Serialx(SOFT_RX, SOFT_TX);  // RX, TX
