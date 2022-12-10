#include "HardwareSerial.h"
#include <SimpleModbusSlave.h>
#include "E32-TTL-100.h"

#define nop() __asm__("nop\n\t")
#define SLAVE_ID 10
#define FLAG 208
uint8_t cbuf[256], index;
uint8_t my_channel=0x17;
#define M0_PIN  3
#define M1_PIN  2
#define AUX_PIN 1
//#include "function.h"
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
struct CFGstruct CFG;  
  struct MVerstruct MVer;

float hl, tl,hr,tr;
uint8_t hl0, hl1, tl0, tl1;
uint16_t datar[20][2];


unsigned char transmit_update=1, tx_ready=0, tx_sent=0;
unsigned long int tx_ready_timer=0, tx_old_timer=0;
unsigned char j=0;
unsigned int timercount=0;
float temperature=0, humidity=0;
unsigned char tcount=0;
static uint8_t raddr=0, tr0, tr1, hr0, hr1, sensorid;

#define HOLDING_REGS_SIZE 60
unsigned int holdingRegs[HOLDING_REGS_SIZE]; 
unsigned char modbusupdate=0;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void setup()
{
  Serial.begin(9600);
  //Serial1.begin(9600);
  //PORTA.PIN2CTRL = 0x08;
  //lora_init();
TCB0.CCMP = 40000;  // 8MHz, 100ms timer 
TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm | TCB_RUNSTDBY_bm;
TCB0.INTCTRL = TCB_CAPT_bm;
modbus_configure(&Serial, 9600, SERIAL_8E1, SLAVE_ID, 10, HOLDING_REGS_SIZE, holdingRegs);
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



//#######################################
void loop() {
modbus_update();
if(modbusupdate){
    for(int j=0;j<HOLDING_REGS_SIZE;j++)
    holdingRegs[j]=450+random(0,100);
    modbusupdate=0;
}
nop();
}
//#####################################




///IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
ISR(TCB0_INT_vect) { 
  if(tcount>100){
        modbusupdate=1;
        tcount=0;
    }
  tcount++;
  switch(timercount)
  {
    case 1:
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

//IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
