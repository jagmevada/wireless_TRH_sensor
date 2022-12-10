#include <SimpleModbusSlave.h>
#include "cactus_io_SHT31.h"
#include <SoftwareSerial.h>
uint8_t cbuf[256], index;
uint8_t my_channel=0x17;
uint8_t sensor_channel= 0x17; 
#define HOLDING_REGS_SIZE 36
#define FLAG 208
#define ADDR 12 // if

#include "E32-TTL-100.h"
#define RX //RX

#ifdef TX
#define Device_B

#define DELAYOFFSET 500
#else
#define Device_A
#endif


#define M0_PIN  8
#define M1_PIN  7
#define SOFT_RX 5
#define SOFT_TX 6
#define AUX_PIN 4

unsigned long receive_frequency;

//cactus_io_SHT31 sht31;
cactus_io_SHT31 sht31(0x45);     // for the alternative address 
#define CLK 9
#define DIO 10
  
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t holdingRegsuf[100], data_len;
int x;
unsigned int holdingRegs[HOLDING_REGS_SIZE]; 

SoftwareSerial Serial3s(SOFT_RX, SOFT_TX);  // RX, TX
#include "function.h"

unsigned long int trs485=millis();

struct CFGstruct CFG;  
  struct MVerstruct MVer;

float hl, tl,hr,tr;
uint8_t hl0, hl1, tl0, tl1;
uint16_t datar[20][2];


void setup()
{
modbus_configure(&Serial, 9600, SERIAL_8E1, 10, 9, HOLDING_REGS_SIZE, holdingRegs);
modbus_update_comms(9600, SERIAL_8E1, 10);
  receive_frequency=millis();
  Serial3s.begin(9600);
  lora_init();  
   
#ifdef TX
  if (!sht31.begin()) {   
    //. Serial.println("Could not find the sensor. Check wiring and I2C address");
    while (1) delay(1);
  }
#endif
}


void loop() {
static unsigned long int  l1=millis(), tfetch=millis();

#ifdef Device_A
modbus_update();
static uint8_t raddr=0, tr0, tr1, hr0, hr1, sensorid;
  if(millis()-receive_frequency> 200){
  if(ReceiveMsg(holdingRegsuf, &data_len)==RET_SUCCESS){
for(int k=index-5; k<index; k++){
raddr=uint8_t (k+256-4) & 0xff;
/*
//. Serial.print("index:"); -4, 1 1 to 6
//. Serial.print(index);
//. Serial.print("\traddr:");
//. Serial.print(raddr);
//. Serial.print("\t cbuf:");
//. Serial.println(cbuf[raddr]);
*/
if(cbuf[raddr]>=208){
sensorid = uint8_t (cbuf[raddr]-208);
tr0= cbuf[uint8_t (raddr+1)];
hr0= cbuf[uint8_t (raddr+2)];
tr1= cbuf[uint8_t (raddr+3)];
hr1= cbuf[uint8_t (raddr+4)];
hr =(float) hr0+ ((float) hr1)/100.0;
tr =(float) tr0+ ((float) tr1)/100.0;
if(sensorid<=15){
holdingRegs[2*(sensorid-1)]=(int) (tr*100);
holdingRegs[2*(sensorid-1)+1]=(int) (hr*100);
//holdingRegs[10]=holdingRegs[8];
//holdingRegs[11]=holdingRegs[9];
//holdingRegs[22]=holdingRegs[20];
//holdingRegs[23]=holdingRegs[21];
//holdingRegs[30]=2450+random(0,100);
//holdingRegs[31]=5950+random(0,100);
//holdingRegs[32]=2550+random(0,100);
//holdingRegs[33]=6450+random(0,100);
//holdingRegs[34]=2650+random(0,100);
//holdingRegs[35]=6950+random(0,100);
}
uint16_t  t_avg_bhai=0;
uint16_t  t_avg_ben=0;
uint16_t  h_avg_bhai=0;
uint16_t  h_avg_ben=0;
uint32_t  t_avg_all=0;
uint32_t  h_avg_all=0;

uint8_t tn_bhai=0, tn_ben=0, hn_bhai=0, hn_ben=0, tn_all=0, hn_all=0;

for(int n=0;n<6;n++){
if(holdingRegs[2*n]!=0){
tn_bhai++;
t_avg_bhai+=holdingRegs[2*n];
}
if(holdingRegs[2*n+1]!=0){
hn_bhai++;
h_avg_bhai+=holdingRegs[2*n+1];
}
holdingRegs[24]=(unsigned int)((float)t_avg_bhai/(float) tn_bhai);
holdingRegs[25]=(unsigned int)((float)h_avg_bhai/(float) hn_bhai);

if(holdingRegs[12+2*n]!=0){
tn_ben++;
t_avg_ben+=holdingRegs[12+2*n];
}
if(holdingRegs[12+2*n+1]!=0){
hn_ben++;
h_avg_ben+=holdingRegs[12+2*n+1];
}
holdingRegs[26]=(unsigned int)((float)t_avg_ben/(float) tn_ben);
holdingRegs[27]=(unsigned int)((float)h_avg_ben/(float) hn_ben);
}

for(int n=0;n<12;n++){
if(holdingRegs[2*n]!=0){
tn_all++;
t_avg_all+=holdingRegs[2*n];
}
if(holdingRegs[2*n+1]!=0){
hn_all++;
h_avg_all+=holdingRegs[2*n+1];
}
holdingRegs[28]=(unsigned int)((double)t_avg_all/(float) tn_all);
holdingRegs[29]=(unsigned int)((double)h_avg_all/(float) hn_all);
}

/*
  //. Serial.print("sensorid-\t");
  //. Serial.print(sensorid);
  //. Serial.print("---\t");
  //. Serial.print(tr);
  //. Serial.print("\t");
  //. Serial.println(hr);
  */
  
}
}  
 
  
  }
  receive_frequency=millis(); 
}


#else
if(millis()-tfetch>980){ // get sensor data
tfetch=millis();
hl=(sht31.getHumidity());
tl=(sht31.getTemperature_C());
hl0 = floor(hl);
hl1 = (int) ((hl-(float) hl0)*100);
tl0 = floor(tl);
tl1 = (int) ((tl-(float) tl0)*100);
}

static unsigned long int t0=millis(), tt=millis();
static uint16_t timers=0;
if(millis()-t0>(DELAYOFFSET+random(30,12000))){
tt=millis();
while(SendMsg(ADDR, tl0, hl0, tl1, hl1)!=RET_SUCCESS)
delay(20);
/////. Serial.println("tx success");
t0=millis();
}
#endif
}

