//////////////// NO MODBUS
/////////Serial is on usb 
/////////Serial1 is on LORA
//////// Software serial tx on pin 2 for MODBUS RTU

#define ENABLEPIN 10
#define nop() __asm__("nop\n\t")
#define MYID  0xFF  // RECEIVER

#define M0_PIN  3
#define M1_PIN  2
#define AUX_PIN 1
#define HEAD 0xc0
#define ADDH 0xff
#define ADDL 0xff
#define CHAN 0x1e

unsigned int timercount=0,sensor_update=0, acquired=0;
unsigned char datar[5]={0x00, 0x01, 0x02, 0x03,0x04};
unsigned char c1[3]={0xc1, 0xc1, 0xc1};
unsigned char auxlevel=0, readytosend=0, onetime=0, updates=0, auxlevelo=0, runs=0;
unsigned long t0, t1;
////////////////////////////
float hl, tl,hr,tr;
uint8_t hl0, hl1, tl0, tl1;
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
  static unsigned char c=0, c0=0, c1=0, c2=0, c3=0, tmp=0, old=0, lock=0, count=0;
while(Serial1.available()>0){
  tmp=Serial1.read();
if(tmp>HEAD && (lock==0)){// && old==ADDL){
    lock=1;
    count=0;
}  
  if(lock){
    switch(count){
      case 0:
      c=(tmp & 0x3f);// rxid
      break;
      case 1:
      c0=tmp;//temp
      break;
      case 2:
      c1=tmp;//humidity
      break;
      case 3:
      c2=tmp;//temp1
      break;
      case 4:
      c3=tmp;//humidity1
      lock=0;
      acquired=1;
      break;
      default:
      break;
    }
    count++;
  }
  else
  ;
if(acquired){
Serial.print("RX\t");
Serial.print(c);
Serial.print("\t");
Serial.print(c0);
Serial.print('.');
Serial.print(c2);
Serial.print("\t\t");
Serial.print(c1);
Serial.print('.');
Serial.println(c3);
acquired=0;
}
}
}
//fffffffffffffffffffffffffffffffffffff

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void setup() {  
pinMode(ENABLEPIN, OUTPUT);
pinMode(M0_PIN, OUTPUT);
pinMode(M1_PIN, OUTPUT);

digitalWrite(ENABLEPIN,1);  // RS485 ONLY WRITE //NO MODBUS
digitalWrite(M0_PIN,1);
digitalWrite(M1_PIN,1);
pinMode(AUX_PIN, INPUT_PULLUP);
attachInterrupt(AUX_PIN, auxsignal, RISING);
nop();nop();nop();
Serial1.begin(9600);
Serial.begin(9600);
Serial.write('A');
delay(1000);
updates=1;
if(updates){
  Serial.write('I');
  Serial1.write(c1,3);  // READ PARAMETER
updates=0;
}
////////////////////////////
TCB0.CCMP = 16000;  // main clock 16MHz, 2ms timer 
TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm | TCB_RUNSTDBY_bm;
TCB0.INTCTRL = TCB_CAPT_bm;
////////////////////////////
delay(1000);
onetime=1;
lora_setting_check();
///////////////receive mode
digitalWrite(M0_PIN,0);
digitalWrite(M1_PIN,0);
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


//##################################################
void loop(){
lora_receive_data();  /// should use falling edge of auxsignal
//lora_setting_check();
delay(1);
nop();
}
//##################################################



//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
ISR(TCB0_INT_vect) { // on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
 switch(timercount)
  {
    case 1:
    sensor_update=1;
         break;
    case 501:  // 500/second
      timercount=0;
      break;
  default:
  break;
  }
  timercount++;
 TCB0.INTFLAGS = TCB_CAPT_bm; /* Clear the interrupt flag */
}

//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII


//#include <SoftwareSerial.h>
//#define SOFT_RX 9
//#define SOFT_TX 8
//SoftwareSerial Serialx(SOFT_RX, SOFT_TX);  // RX, TX
