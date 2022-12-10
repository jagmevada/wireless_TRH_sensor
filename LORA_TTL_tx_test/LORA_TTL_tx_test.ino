/////////Serial is on usb
/////////Serial1 is on LORA
//////// Software serial tx on pin 2 for MODBUS RTU

#include  "cactus_io_SHT31.h"
#define nop() __asm__("nop\n\t")
#define LONG_RANGE
#define MYID  26

#define M0_PIN  3
#define M1_PIN  2
#define AUX_PIN 1
#define HEAD 0xc0
#define ADDH 0xcc
#define ADDL 0xff
#define CHAN 0x1e

#define LORA_TX_WAIT 250 // x2ms wait between two calls
#define LORA_TX_MAX_TIME 5000 //x2ms
//////////////////////
cactus_io_SHT31 sht31(0x44);     // for the alternative address 
unsigned char datas[8]={ADDH,ADDL,CHAN, (HEAD | MYID), 0x01, 0x02, 0x03,0x04};
unsigned char c1[3]={0xc1, 0xc1, 0xc1};
unsigned char auxlevel=0, readytosend=0, onetime=0, updates=0, auxlevelo=0, runs=0;
unsigned long t0, t1;
////////////////////
unsigned char sensor_update=0;
unsigned char transmit_update=1, tx_ready=0, tx_sent=0;
unsigned long int tx_ready_timer=0, tx_old_timer=0;
unsigned char j=0;
unsigned int timercount=0;
float temperature=0, humidity=0;
///////////////////
float hl, tl,hr,tr;
uint8_t hl0, hl1, tl0, tl1;
///////////////////


void local_ht_print();
void auxsignal();
void lora_rx_check();
void lora_send_data();
void hex_as_ascii();

//fffffffffffffffffffffffffffffffffffff
void local_ht_print(){
Serial.print(MYID); Serial.print("\t\t");  
Serial.print(tx_old_timer); Serial.print(" Ms\t\t");      
Serial.print(hl); Serial.print(" %\t\t"); 
Serial.print(tl); Serial.println(" *C");
}
//////////////////////
void auxsignal(){
updates=1;
}
/////////////////////
void lora_rx_check(){
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
///////////////////////////
void lora_send_data(){
hl0 = floor(hl);
hl1 = (int) ((hl-(float) hl0)*100);
tl0 = floor(tl);
tl1 = (int) ((tl-(float) tl0)*100);
datas[4]=tl0;
datas[5]=hl0;
datas[6]=tl1;
datas[7]=hl1;
Serial1.write(datas,8);
Serial.write('T');
}
//fffffffffffffffffffffffffffffffffffff

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void setup(){
pinMode(M0_PIN, OUTPUT);
pinMode(M1_PIN, OUTPUT);
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

if(MYID>15)
randomSeed(1345+MYID<<1);
else
randomSeed(1<<MYID);
if (!sht31.begin()) {   
    Serial.println("Could not find the sensor. Check wiring and I2C address");
while (1) delay(1);
}
//Serial.println("Address\t\tRandom\t\tHumidity\tTemp");
/////////////////////////////
delay(1000);
readytosend=0;
onetime=1;
lora_rx_check();
digitalWrite(M0_PIN,0);
digitalWrite(M1_PIN,0);
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


//##################################################
void loop(){
  
if(sensor_update){
hl=sht31.getHumidity();
tl=sht31.getTemperature_C();
 sensor_update=0;
}
///////////////////// 
if(transmit_update){
    transmit_update=0;
    tx_ready=0;
       tx_ready_timer=random(LORA_TX_WAIT,LORA_TX_MAX_TIME);
       tx_old_timer=tx_ready_timer;
    //send data
  }
////////////////////////
 if(tx_ready){ 
  lora_send_data();
  local_ht_print();
  transmit_update=1;
  }
nop();
///////////////////////

lora_rx_check();
nop();
}

//##################################################


//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
ISR(TCB0_INT_vect) { // on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  if(tx_ready_timer>0){
    tx_ready_timer--;
  }
  else
  tx_ready=1;
  
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
